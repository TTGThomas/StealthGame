#include "NPC.h"

#include "Player.h"
#include "../GameScene.h"

std::unordered_set<uint64_t> NPC::m_detectedDeadNPCs;

void NPC::Init(std::vector<QuadInitDesc> desc, std::string name)
{
	Entity::Init(desc);
	m_name = name;
}

void NPC::NPCTick(GameTickDesc& desc)
{
	if (m_health > 0)
	{
		SetPos(GetPos());
		DetectEverything();
		if (m_type == Identities::GUEST || m_type == Identities::VIPGUEST)
			TickGuest(desc);
		else if (m_type == Identities::GUARD || m_type == Identities::VIPGUARD)
			TickGuard(desc);
		TickNonStatic(desc);
		ApplyDamage();
	}
	else
	{
		TickDead(desc);
	}

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
	m_isPlayerDetected = false;
	m_health = 0;
	m_state = State::NORMAL;
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

	gData.m_scene->GetAABBs()[GetUUID(0).GetUUID()].SetEnabled(true);
	CollisionPayload payload = m_collision->Collide(2, GetUUID(0));
	if (payload.m_hasHit)
	{
		EliminateMyself();
	}
	gData.m_scene->GetAABBs()[GetUUID(0).GetUUID()].SetEnabled(false);
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

	Scene* scene = GlobalData::Get().m_scene;

	bool ret = !m_collision->Collide(0, GetPos(), player->GetPos()).m_hasHit;
	return ret;
}

bool NPC::IsPlayerDetected()
{
	return m_isPlayerDetected;
}

void NPC::DetectPlayer()
{
	if (m_health == 0)
	{
		m_isPlayerDetected = false;
		return;
	}

	if (IsPlayerInSight())
	{
		m_isPlayerDetected = true;
		return;
	}

	m_isPlayerDetected = false;
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

		glm::vec2 npcPos = npc.GetPos();

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

void NPC::TickGuest(GameTickDesc& desc)
{
	GlobalData& gData = GlobalData::Get();
	Player* player = &gData.m_gameScene->GetPlayer();

	if (IsPlayerDetected())
	{
		if (player->GetActionType() == Player::ActionType::ILLEGAL)
		{
			m_isWitness = true;
		}
	}

	if (m_isWitness)
	{
		//MoveToTarget(desc.m_tickTimer->Second(), m_player->GetPos(), false);
		//PointAtPoint(m_player->GetPos());
		//EliminateMyself();
	}
}

void NPC::TickGuard(GameTickDesc& desc)
{
	GlobalData& gData = GlobalData::Get();
	Player* player = &gData.m_gameScene->GetPlayer();

	if (IsPlayerDetected())
	{
		if (player->GetActionType() == Player::ActionType::ILLEGAL)
		{
			m_state = State::PANIC;
			m_disguiseStates[(int)Identities::STANDARD] = DisguiseState::ALERT;
		}
	}

	if (m_state == State::PANIC)
	{
		glm::vec2 pos = glm::normalize(GetQuad(0)->GetPos() - player->GetPos());
		pos *= 1.5f;
		MoveToTarget(desc.m_tickTimer->Second(), player->GetPos() + pos);
		PointAtPoint(player->GetPos());
		if (IsPlayerDetected())
		{
			// shoot
		}
	}
	else if (m_state == State::SEARCHING)
	{
		if (m_searchType == SearchType::DEADBODY)
			TickGuardSearchBody(desc);
		else if (m_searchType == SearchType::GUNSHOT)
			TickGuardSearchGunShot(desc);
		else if (m_searchType == SearchType::ILLEGALWEAPON)
			TickGuardSearchILLEGALWEAPON(desc);
	}

	if (m_state != State::PANIC && m_state != State::SEARCHING)
	{
		// goes through every item the npc sees
		for (uint64_t& uuid : m_detectedItems)
		{
			Item* item = gData.m_gameScene->GetItems().GetItem(uuid).get();
		}

		// goes through every npc the npc sees
		for (uint64_t& uuid : m_detectedNPCs)
		{
			NPC* npc = &gData.m_gameScene->GetNPCs()[uuid];
			if (npc->GetHealth() < 1)
			{
				if (m_detectedDeadNPCs.find(uuid) == m_detectedDeadNPCs.end())
				{
					m_detectedDeadNPCs.insert(uuid);
					gData.m_bodiesFound++;

					Search(npc->GetPos(), 20.0f, SearchType::DEADBODY);
				}
			}
			else
			{
				if (npc->GetState() == State::PANIC)
					m_state = State::PANIC;
			}
		}
	}
}

void NPC::TickNonStatic(GameTickDesc& desc)
{
	GlobalData& gData = GlobalData::Get();
	Player* player = &gData.m_gameScene->GetPlayer();

	if (m_state != State::PANIC && m_state != State::SEARCHING)
	{
		float dst = glm::distance(GetPos(), player->GetPos());
		if (dst < 0.2f)
		{
			m_suspiciousMeter = 1.0f;
			m_state = State::SUSPICIOUS;
		}
		else if (dst < 0.5f)
		{
			if (glm::length(player->GetVelocity()) != 0.0f)
			{
				if (!player->GetIsCrouching())
				{
					m_suspiciousMeter = 1.0f;
					m_state = State::SUSPICIOUS;
				}
			}
		}

		if (m_state == State::SUSPICIOUS)
		{
			m_suspiciousMeter -= desc.m_tickTimer->Second();
			if (m_suspiciousMeter < 0.0f)
			{
				m_suspiciousMeter = 0.0f;
				m_state = State::NORMAL;
			}
		}

		if (m_state == State::SUSPICIOUS)
		{
			PointAtPoint(player->GetPos());
		}

		if (m_state == State::NORMAL)
		{
			if (m_route.size() > 0)
			{
				if (MoveToTarget(desc.m_tickTimer->Second(), m_route[m_targetRouteIndex].m_pos))
				{
					m_targetRouteIndex++;
					if (m_targetRouteIndex >= (int)m_route.size())
						m_targetRouteIndex -= (int)m_route.size();
				}
				PointAtPoint(m_route[m_targetRouteIndex].m_pos);
			}
			else
			{
				m_targetDir = 0.0f;
			}
		}
	}

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
}

void NPC::TickDead(GameTickDesc& desc)
{
	Scene* scene = GlobalData::Get().m_scene;
	scene->GetRenderQuads()[GetUUID(1).GetUUID()].SetTextureUUID(GlobalData::Get().m_texNPCDead);

	scene->GetRenderQuads()[GetUUID(2).GetUUID()].SetVisibility(false);

	if (m_isBeingDragged && !m_isDisposed)
	{
		// do stuffs related to being dragged
		Player* player = &GlobalData::Get().m_gameScene->GetPlayer();
		m_isBeingDragged = player->GetIsDragging();
		if (glm::distance(GetPos(), player->GetPos()) > 0.5f)
		{
			glm::vec2 pos = glm::normalize(GetQuad(0)->GetPos() - player->GetPos());
			if (pos.x == FP_NAN || pos.y == FP_NAN)
				pos = { 0.0f, 1.0f };
			pos *= 0.5f;
			SetPos(player->GetPos() + pos);
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

		if (!m_isBeingDragged)
		{
			GetQuad(0)->SetPos(GetQuad(1)->GetPos());
		}
	}

	if (m_isDisposed)
	{
		GetQuad(0)->SetRadius({});
		GetQuad(1)->SetRadius({ m_normalScale.x, m_normalScale.y });
		GetQuad(1)->SetRotation(90);
	}
}

void NPC::TickGuardSearchBody(GameTickDesc& desc)
{
	// TODO: Make guards drag the body to somewhere safe after pathfinding is done

	Player* player = &GlobalData::Get().m_gameScene->GetPlayer();

	m_searchingMeter -= desc.m_tickTimer->Second();
	if (IsPlayerDetected())
	{
		static bool first = true;
		if (first)
		{
			first = false;
			if (glm::distance(player->GetPos(), m_searchPos) < 1.0f)
				m_state = State::PANIC;
		}
	}

	m_speed = m_runningSpeed;
	
	static float m_miniSearchingMeter = 4.0f;
	static bool m_isLocationNew = true;

	if (m_isLocationNew)
		StartMoveToLocation(m_miniSearchPos);

	if (MoveToLocation(desc.m_tickTimer->Second()))
	{
		if (m_miniSearchingMeter > 0.0f)
		{
			m_miniSearchingMeter -= desc.m_tickTimer->Second();
		}
		else
		{
			m_isLocationNew = true;

			glm::vec2 add = {};
			float deg = (float)rand() / (float)RAND_MAX * glm::radians(360.0f);
			add.x = 0.5f * glm::sin(deg);
			add.y = 0.5f * glm::cos(deg);
			m_miniSearchPos = m_searchPos + add;
			m_miniSearchingMeter = 2.0f;
		}
	}
	else
	{
		m_isLocationNew = false;
	}

	if (m_searchingMeter < 0.0f)
	{
		m_state = State::NORMAL;
		m_searchingMeter = 0.0f;
		m_speed = m_normalSpeed;
	}
}

void NPC::TickGuardSearchGunShot(GameTickDesc& desc)
{
}

void NPC::TickGuardSearchILLEGALWEAPON(GameTickDesc& desc)
{
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

	glm::vec2 add{};
	float speed = m_speed * dt;

	if (GetPos().x < point.x)
		add.x += speed;
	if (GetPos().x > point.x)
		add.x -= speed;
	if (GetPos().y < point.y)
		add.y += speed;
	if (GetPos().y > point.y)
		add.y -= speed;

	gData.m_scene->GetAABBs()[GetUUID(0).GetUUID()].SetEnabled(true);
	NPCMove(add);

	if (glm::distance(GetPos(), point) < 0.01f && snapp)
		NPCMove(point - GetPos());

	if (gData.m_collision->Collide(0, GetUUID(0)).m_hasHit)
		NPCMove(GetPos() - point);
	gData.m_scene->GetAABBs()[GetUUID(0).GetUUID()].SetEnabled(false);

	return GetPos() == point;
}

bool NPC::MoveToLocation(float dt)
{
	bool ret = false;
	if (m_isDynamicRouteCalculated && m_dynamicTargetRouteIndex < m_dynamicRoute.size())
	{
		ret = MoveToTarget(dt, m_dynamicRoute[m_dynamicTargetRouteIndex].m_pos);
		PointAtPoint(m_dynamicRoute[m_dynamicTargetRouteIndex].m_pos);
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
	std::thread pathSearch = std::thread(CalculateDynamicRoute, &GlobalData::Get(), (void*)&m_dynamicRoute, (void*)&m_isDynamicRouteCalculated, GetPos(), location);
	pathSearch.detach();
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

glm::vec2 NPC::GetGridPos(glm::vec2 location)
{
	location += glm::vec2(MAP_RADIUS, -MAP_RADIUS);
	glm::vec2 pos = glm::round(location / MAP_SCALE) * MAP_SCALE;
	pos += glm::vec2(-MAP_RADIUS, MAP_RADIUS);
	return pos;
}

void CalculateDynamicRoute(GlobalData* gData, void* route, void* isRouteCalculated, glm::vec2 start, glm::vec2 location)
{
	std::vector<NPCRoutePoint>* routePoints = (std::vector<NPCRoutePoint>*)route;
	bool* isDynamicRouteCalculated = (bool*)isRouteCalculated;

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

	glm::vec2 end = NPC::GetGridPos(location);
	if (gData->m_collision->Collide(0, end).m_hasHit)
	{
		*routePoints = { NPCRoutePoint(start) };
		*isDynamicRouteCalculated = true;
		return;
	}

	static int dx[] = { 0, 0, 1, -1 };
	static int dy[] = { 1, -1, 0, 0 };

	std::vector<Node> locations;
	std::set<int> openList;
	std::set<int> closedList;

	locations.emplace_back(NPC::GetGridPos(start), -1).SetH(end);
	openList.insert(0);

	bool routeFound = false;
	int routeIndexFound = -1;

	while (true)
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
		DebugManager::AddDebugQuad(now.m_pos, { 0.1f, 0.1f }, 0.0f);

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

			if (gData->m_collision->Collide(0, neighbour.m_pos).m_hasHit)
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
	}

	*isDynamicRouteCalculated = true;
}
