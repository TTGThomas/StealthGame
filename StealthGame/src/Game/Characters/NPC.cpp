#include "NPC.h"

// TODO: deprecate std::string
#include <string>

#include "Player.h"
#include "../GameScene.h"

#include "../Items/Disguise.h"

std::unordered_set<uint64_t> NPC::m_detectedDeadNPCs;

NPC::~NPC()
{
	m_shouldTerminateCalc = true;
	m_routeThread.join();
}

void NPC::Init(std::vector<QuadInitDesc> desc, std::string name)
{
	Entity::Init(desc);
	m_name = name;

	GetQuad(0)->SetRadius(m_normalScale);

	m_animBP.Init(this);

	// DEAD:
	//
	// +--------+-->+--------+
	// |DeadTick|   |DragTick|
	// +--------+<--+--------+
	//               ^      |
	//               |      |
	//			  +-----------+
	//			  |DisposeTick|
	//			  +-----------+

	ResetNodeGraph();

	InitNodeGraph();
	
	CompileNodeGraph();
}

void NPC::NPCTick(GameTickDesc& desc)
{
	glm::vec2 lastPos = GetPos();

	// below are funcs that must get called every frame
	m_animBP.Tick(this);
	if (m_health > 0)
	{
		SetPos(GetPos());
		ApplyDamage();
		DetectEverything();
	}

	//if (m_health > 0)
	//{
	//	SetPos(GetPos());
	//	ApplyDamage();
	//	if (m_health <= 0)
	//		return;
	//	DetectEverything();
	//	if (m_type == Identities::GUEST || m_type == Identities::VIPGUEST)
	//		TickGuest(desc);
	//	else if (m_type == Identities::GUARD || m_type == Identities::VIPGUARD)
	//		TickGuard(desc);
	//	TickNonStatic(desc);
	//}
	//else
	//{
	//	TickDead(desc);
	//}

	Node& node = m_nodes[m_nodePos];
	float time = (float)glfwGetTime() - m_timeWhenEnter;
	int frame = m_frameFromEnter;
	node.m_func(desc, time, frame);
	m_frameFromEnter++;
	for (int bridgeIndex : node.m_relatedBridges)
	{
		Bridge& bridge = m_bridges[bridgeIndex];
		if (bridge.m_determineFunc(time, frame))
		{
			m_nodePos = bridge.m_destIndex;
			m_frameFromEnter = 0;
			m_timeWhenEnter = (float)glfwGetTime();
		}
	}

	m_velocity = GetPos() - lastPos;

	// update dir
	float t0 = m_targetDir - m_dir;
	float t1 = 360 - glm::abs(t0);
	float amount = 0.0f;
	if (glm::abs(t0) < glm::abs(t1))
		amount = t0;
	else
		amount = (m_dir + t1 > 360.0f ? t1 : -t1);
	float dirSpeed = desc.m_tickTimer->Second() * 200.0f;
	if (amount > 0.0f)
		m_dir += glm::min(dirSpeed, amount);
	else
		m_dir -= glm::min(dirSpeed, -amount);

	while (m_dir > 360.0f)
		m_dir -= 360.0f;
	while (m_dir < 0.0f)
		m_dir += 360.0f;
}

void NPC::SetPos(glm::vec2 newPos)
{
	GetQuad(0)->SetPos(newPos);
	GetQuad(1)->SetPos(newPos);
	if (m_health > 0)
		SetDirPos(newPos);
}

void NPC::ChangePos(glm::vec2 pos)
{
	GetQuad(0)->ChangePos(pos);
	GetQuad(1)->ChangePos(pos);
	if (m_health > 0)
		SetDirPos(GetQuad(0)->GetPos());
}

void NPC::EliminateMyself()
{
	std::shared_ptr<Disguise> disguise = std::make_shared<Disguise>();
	disguise->Init(m_type, GetPos(), 0.0f, GlobalData::Get().m_defaultShader);
	GlobalData::Get().m_gameScene->GetItems().AddItem(disguise);

	m_playerDetected = 0.0f;
	m_health = 0;
	ResetNodeGraph();
	NodeGraphDead();
	CompileNodeGraph();
	GlobalData::Get().m_gameScene->DeleteTarget(GetNPCUUID().GetUUID());
}

void NPC::SetDirPos(glm::vec2 pos)
{
	Scene* scene = GlobalData::Get().m_scene;

	glm::vec2 p = pos;
	p.x += glm::sin(glm::radians(m_dir)) * scene->GetQuads()[GetUUID(2).GetUUID()].GetRadius().y;
	p.y += glm::cos(glm::radians(m_dir)) * scene->GetQuads()[GetUUID(2).GetUUID()].GetRadius().y;
	scene->GetQuads()[GetUUID(2).GetUUID()].SetPos(p);
	scene->GetQuads()[GetUUID(2).GetUUID()].SetRotation(m_dir);
}

void NPC::ApplyDamage()
{
	GlobalData& gData = GlobalData::Get();

	CollisionPayload payload = m_collision->Collide(2, GetUUID(0));
	if (payload.m_hasHit)
	{
		EliminateMyself();
	}
}

bool NPC::IsPlayerInSight()
{
	GlobalData& gData = GlobalData::Get();
	Player* player = &gData.m_gameScene->GetPlayer();

	float dst = glm::distance(GetPos(), player->GetPos());
	if (dst > 3.0f)
		return false;

	glm::vec2 vPlayer = glm::normalize(player->GetPos() - GetPos());

	float cosTheta = glm::dot(m_frontVec, vPlayer);

	if (!IsThetaInView(cosTheta))
		return false;

	Scene* scene = gData.m_scene;
	SpecialBlockManager& sp = gData.m_gameScene->GetSpecialBlockManager();

	for (int i = 0; i < sp.GetObjects().size(); i++)
		if (sp.GetInteracts()[i]->GetType() != Interaction::Type::DOOR)
			scene->GetAABBs()[sp.GetObjects()[i].GetUUID(0).GetUUID()].SetEnabled(false);

	CollisionPayload ret = m_collision->Collide(0, GetPos(), player->GetPos());
	
	for (int i = 0; i < sp.GetObjects().size(); i++)
		if (sp.GetInteracts()[i]->GetType() != Interaction::Type::DOOR)
			scene->GetAABBs()[sp.GetObjects()[i].GetUUID(0).GetUUID()].SetEnabled(true);

	return !ret.m_hasHit;
}

bool NPC::IsPlayerDetected()
{
	return m_playerDetected > 0.0f;
}

void NPC::DetectPlayer()
{
	if (m_health == 0)
	{
		m_playerDetected = 0.0f;
		return;
	}

	if (IsPlayerInSight())
	{
		m_playerDetected = MEMTIME;
	}

	m_playerDetected -= GlobalData::Get().m_deltaTime;
}

void NPC::DetectItems()
{
	m_detectedItems = {};
	GlobalData& gData = GlobalData::Get();
	Player* player = &gData.m_gameScene->GetPlayer();
	GameScene& gameScene = *gData.m_gameScene;

	for (auto& [uuid, item] : gameScene.GetItems().GetItems())
	{
		glm::vec2 itemPos = item->GetQuad().GetPos();

		float dst = glm::distance(GetPos(), itemPos);
		if (dst > 3.0f)
			continue;

		glm::vec2 vItem = glm::normalize(itemPos - GetPos());

		float cosTheta = glm::dot(m_frontVec, vItem);

		if (!IsThetaInView(cosTheta))
			continue;

		Scene* scene = GlobalData::Get().m_scene;

		bool ret = !m_collision->Collide(0, GetPos(), player->GetPos()).m_hasHit;
		if (ret)
			m_detectedItems.emplace_back(uuid);
	}
}

void NPC::DetectNPCs()
{
	m_detectedNPCs = {};
	GlobalData& gData = GlobalData::Get();
	Player* player = &gData.m_gameScene->GetPlayer();
	GameScene& gameScene = *gData.m_gameScene;

	for (auto& [uuid, npc] : gameScene.GetNPCs())
	{
		if (uuid == m_uuid.GetUUID())
			continue;

		glm::vec2 npcPos = npc->GetPos();

		float dst = glm::distance(GetPos(), npcPos);
		if (dst > 3.0f)
			continue;

		glm::vec2 vNPC = glm::normalize(npcPos - GetPos());

		float cosTheta = glm::dot(m_frontVec, vNPC);

		if (!IsThetaInView(cosTheta))
			continue;

		Scene* scene = GlobalData::Get().m_scene;

		bool ret = !m_collision->Collide(0, GetPos(), npcPos).m_hasHit;
		if (ret)
			m_detectedNPCs.emplace_back(uuid);
	}
}

void NPC::DetectEverything()
{
	UpdateFrontVec();
	DetectPlayer();
	DetectItems();
	DetectNPCs();
}

void NPC::UpdateFrontVec()
{
	m_frontVec = {};
	m_frontVec.x += glm::sin(glm::radians(m_dir));
	m_frontVec.y += glm::cos(glm::radians(m_dir));
}

bool NPC::IsThetaInView(float cosTheta)
{
	//return glm::acos(cosTheta) < glm::radians(70.0f);
	//return cosTheta < glm::cos(glm::radians(70.0f));
	// cos(radians(70.0f)) = 0.342020154;
	// somehow i need to flip the result to be correct;
	return cosTheta > 0.342020154f;
}

bool NPC::MoveToTarget(float dt, glm::vec2 point, bool snapp)
{
	GlobalData& gData = GlobalData::Get();
	Player* player = &gData.m_gameScene->GetPlayer();
	Scene* scene = gData.m_scene;

	float speed = m_speed * dt;
	glm::vec2 add{};

	if (GetPos().x < point.x)
		add.x += speed;
	if (GetPos().x > point.x)
		add.x -= speed;
	if (GetPos().y < point.y)
		add.y += speed;
	if (GetPos().y > point.y)
		add.y -= speed;

	if (snapp)
	{
		glm::vec2 offset = point - GetPos();
		if (glm::abs(offset.x) < glm::abs(add.x))
			add.x = offset.x;
		if (glm::abs(offset.y) < glm::abs(add.y))
			add.y = offset.y;
	}

	NPCMove(add);

	return GetPos() == point;
}

bool NPC::MoveToLocation(float dt)
{
	bool ret = false;
	if (m_isDynamicRouteCalculated && m_dynamicTargetRouteIndex < m_dynamicRoute.size())
	{
		ret = MoveToTarget(dt, m_dynamicRoute[m_dynamicTargetRouteIndex].m_pos);
		if (ret && m_dynamicTargetRouteIndex < m_dynamicRoute.size() - 1)
			m_dynamicTargetRouteIndex++;
	}
	return ret && m_dynamicTargetRouteIndex == m_dynamicRoute.size() - 1;
}

void NPC::StartMoveToLocation(glm::vec2 location)
{
	// add task to thread
	m_dynamicTargetRouteIndex = 0;
	m_dynamicRoute = {};
	m_isDynamicRouteCalculated = false;
	if (m_routeThread.get_id() != std::thread::id())
	{
		m_shouldTerminateCalc = true;
		m_routeThread.join();
		m_shouldTerminateCalc = false;
	}
	m_routeThread.~thread();

	NPCCalcDynamicIn in;
	in.m_isRouteCalculated = &m_isDynamicRouteCalculated;
	in.m_shouldTerminate = &m_shouldTerminateCalc;
	in.m_route = &m_dynamicRoute;
	in.m_location = location;
	in.m_start = GetPos();
	m_routeThread = std::thread(CalculateDynamicRoute, in);
}

void NPC::PointAtPoint(glm::vec2 point)
{
	m_targetDir = AngleFromPoint(point);
}

float NPC::AngleFromPoint(glm::vec2 point)
{
	point = glm::normalize(point - GetPos());
	if (point.x == FP_NAN || point.y == FP_NAN)
		point = { 0.0f, 1.0f };
	glm::vec2 front = glm::vec2(0.0f, 1.0f);

	float degree = glm::degrees(glm::acos(glm::dot(point, front)));
	return (point.x > 0.0f ? degree : 360.0f - degree);
}

void NPC::NPCMove(glm::vec2 vec)
{
	Move(m_collision, vec.x, vec.y);
}

void NPC::CompileNodeGraph()
{
	for (int i = 0; i < m_bridges.size(); i++)
	{
		Bridge& bridge = m_bridges[i];
		for (int o : bridge.m_originIndexes)
			m_nodes[o].m_relatedBridges.emplace_back(i);
	}
}

void NPC::NodeGraphDead()
{
	// nodes
	{
		Node& node = m_nodes.emplace_back(Node());
		node.m_func = [this](GameTickDesc& desc, float timeFromEnter, int frameFromEnter)
			{
				Scene* scene = desc.m_scene;
				scene->GetRenderQuads()[GetUUID(1).GetUUID()].SetTextureUUID(GlobalData::Get().m_texNPCDead);
				scene->GetRenderQuads()[GetUUID(2).GetUUID()].SetVisibility(false);

			};
	}
	int deadTick = m_nodes.size() - 1;
	{
		Node& node = m_nodes.emplace_back(Node());
		node.m_func = [this](GameTickDesc& desc, float timeFromEnter, int frameFromEnter)
			{
				Scene* scene = desc.m_scene;
				Player* player = &GlobalData::Get().m_gameScene->GetPlayer();
				m_isBeingDragged = player->GetIsDragging();
				if (!m_isBeingDragged)
					return;

				if (glm::distance(GetPos(), player->GetPos()) > 0.5f)
				{
					glm::vec2 vec = glm::normalize(GetQuad(0)->GetPos() - player->GetPos());
					if (vec.x == FP_NAN || vec.y == FP_NAN)
						vec = { 0.0f, 1.0f };
							vec *= 0.5f;
					SetPos(player->GetPos() + vec);
				}
				float angle;
				if (glm::length(player->GetPos() - GetQuad(0)->GetPos()) != 0.0f)
					angle = AngleFromPoint(player->GetPos());
				else
					angle = 90.0f;
				scene->GetQuads()[GetUUID(1).GetUUID()].SetRotation(angle + 90.0f);
		
				{
					glm::vec2 pos;
					pos = scene->GetQuads()[GetUUID(0).GetUUID()].GetPos();
					pos.x += glm::sin(glm::radians(angle)) * GetQuad(1)->GetRadius().x;
					pos.y += glm::cos(glm::radians(angle)) * GetQuad(1)->GetRadius().x;
					scene->GetQuads()[GetUUID(1).GetUUID()].SetPos(pos);
				}
			};
	}
	int dragTick = m_nodes.size() - 1;
	{
		Node& node = m_nodes.emplace_back(Node());
		node.m_func = [this](GameTickDesc& desc, float timeFromEnter, int frameFromEnter)
			{
				GetQuad(0)->SetRadius({});
				GetQuad(1)->SetPos(GetQuad(0)->GetPos());
				GetQuad(1)->SetRotation(90);
			};
	}
	int disposeTick = m_nodes.size() - 1;

	// bridges
	{
		Bridge& bridge = m_bridges.emplace_back(Bridge());
		bridge.m_originIndexes.emplace_back(deadTick);
		bridge.m_destIndex = dragTick;
		bridge.m_determineFunc = [this](float time, int frame) -> bool
			{
				return GetIsBeingDragged();
			};
	}
	{
		Bridge& bridge = m_bridges.emplace_back(Bridge());
		bridge.m_originIndexes.emplace_back(dragTick);
		bridge.m_destIndex = deadTick;
		bridge.m_determineFunc = [this](float time, int frame) -> bool
			{
				bool ret = !GetIsBeingDragged() && !GetIsDisposed();
				if (ret)
				{
					GetQuad(0)->SetPos(GetQuad(1)->GetPos());
				}
				return ret;
			};
	}
	{
		Bridge& bridge = m_bridges.emplace_back(Bridge());
		bridge.m_originIndexes.emplace_back(dragTick);
		bridge.m_destIndex = disposeTick;
		bridge.m_determineFunc = [this](float time, int frame) -> bool
			{
				return GetIsDisposed();
			};
	}
}

glm::vec2 NPC::GetGridPos(glm::vec2 location)
{
	location += glm::vec2(MAP_RADIUS, -MAP_RADIUS);
	glm::vec2 pos = glm::round(location / MAP_SCALE) * MAP_SCALE;
	pos += glm::vec2(-MAP_RADIUS, MAP_RADIUS);
	return pos;
}

void CalculateDynamicRoute(NPCCalcDynamicIn in)
{
	GlobalData& gData = GlobalData::Get();
	std::vector<NPCRoutePoint>* routePoints = (std::vector<NPCRoutePoint>*)in.m_route;
	bool* isDynamicRouteCalculated = (bool*)in.m_isRouteCalculated;

	struct Node
	{
		Node(glm::vec2 pos, int parentIndex)
			: m_pos(pos), m_parentIndex(parentIndex) {}

		void SetH(glm::vec2 end)
		{
			glm::vec2 offset = glm::abs(m_pos - end);
			m_h = offset.x + offset.y;
		}

		float F() { return m_g + m_h; }

		glm::vec2 m_pos;
		int m_parentIndex;

		// movement from start to m_pos
		float m_g = 0.0f;
		// estimated movement from m_pos to location
		// i used manhattan
		float m_h = 0.0f;
	};

	glm::vec2 end = NPC::GetGridPos(in.m_location);
	if (gData.m_collision->Collide(0, end).m_hasHit)
	{
		*routePoints = { NPCRoutePoint(in.m_start) };
		*isDynamicRouteCalculated = true;
		return;
	}

	static int dx[] = { 0, 0, 1, -1 };
	static int dy[] = { 1, -1, 0, 0 };

	std::vector<Node> locations;
	std::set<int> openList;
	std::set<int> closedList;

	locations.emplace_back(NPC::GetGridPos(in.m_start), -1).SetH(end);
	openList.insert(0);

	bool routeFound = false;
	int routeIndexFound = -1;

	while (!openList.empty())
	{
		// Get Node with lowest F cost and move it to closedList
		int index = -1;
		for (int i : openList)
			if (index == -1 || locations[i].F() < locations[index].F())
				index = i;
		Node now = locations[index];
		openList.erase(index);
		closedList.insert(index);

		// add debug quad
		//DebugManager::AddDebugQuad(now.m_pos, { 0.1f, 0.1f }, 0.0f);

		if (*(bool*)in.m_shouldTerminate)
			return;

		// return if route is found
		if (glm::distance(now.m_pos, end) < 0.1f)
		{
			routeFound = true;
			routeIndexFound = index;
			break;
		}

		// for every neighbour
		for (int i = 0; i < 4; i++)
		{
			Node neighbour = now;
			neighbour.m_pos += glm::vec2((float)dx[i] * MAP_SCALE, (float)dy[i] * MAP_SCALE);
			neighbour.m_parentIndex = index;

			if (gData.m_collision->Collide(0, neighbour.m_pos).m_hasHit)
				continue;
			bool inClosed = false;
			for (int i : closedList)
			{
				if (locations[i].m_pos == neighbour.m_pos)
				{
					inClosed = true;
					break;
				}
			}
			if (inClosed)
				continue;

			bool inOpen = false;
			int neighbourIndex = -1;
			for (int i : openList)
			{
				if (locations[i].m_pos == neighbour.m_pos)
				{
					neighbourIndex = i;
					inOpen = true;
					break;
				}
			}

			if (inOpen)
			{
				if (now.m_g + MAP_RADIUS < locations[neighbourIndex].m_g)
				{
					locations[neighbourIndex].m_parentIndex = index;
					locations[neighbourIndex].m_g = now.m_g + MAP_RADIUS;
				}
			}
			else
			{
				neighbour.m_g = now.m_g + MAP_RADIUS;
				neighbour.SetH(end);
				locations.emplace_back(neighbour);
				openList.insert(locations.size() - 1);
			}
		}
		//std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}
	
	if (routeFound)
	{
		if (routeIndexFound == 0)
		{
			routePoints->clear();
			routePoints->emplace_back(NPC::GetGridPos(in.m_start));
			*isDynamicRouteCalculated = true;
			return;
		}

		while (routeIndexFound != 0)
		{
			routePoints->emplace_back(locations[routeIndexFound].m_pos);
			routeIndexFound = locations[routeIndexFound].m_parentIndex;
		}

		// flip the routes
		int n = routePoints->size() / 2;
		for (int front = 0; front < n; front++)
		{
			int back = routePoints->size() - 1 - front;
			// swap back and front
			NPCRoutePoint backVal = (*routePoints)[back];
			(*routePoints)[back] = (*routePoints)[front];
			(*routePoints)[front] = backVal;
		}
	}
	else
	{
		routePoints->clear();
		routePoints->emplace_back(NPC::GetGridPos(in.m_start));
	}

	*isDynamicRouteCalculated = true;
}
