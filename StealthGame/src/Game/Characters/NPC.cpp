#include "NPC.h"

#include "Player.h"
#include "../GameScene.h"

std::unordered_set<uint64_t> NPC::m_detectedDeadNPCs;

void NPC::Init(std::vector<QuadInitDesc> desc, const char* name)
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
	m_health = 0;
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

	scene->GetAABBs()[player->GetUUID(0).GetUUID()].SetEnabled(false);
	bool ret = !m_collision->Collide(GetPos(), player->GetPos()).m_hasHit;
	scene->GetAABBs()[player->GetUUID(0).GetUUID()].SetEnabled(true);
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

		scene->GetAABBs()[player->GetUUID(0).GetUUID()].SetEnabled(false);
		bool ret = !m_collision->Collide(GetPos(), player->GetPos()).m_hasHit;
		scene->GetAABBs()[player->GetUUID(0).GetUUID()].SetEnabled(true);
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

		scene->GetAABBs()[player->GetUUID(0).GetUUID()].SetEnabled(false);
		bool ret = !m_collision->Collide(GetPos(), npcPos).m_hasHit;
		scene->GetAABBs()[player->GetUUID(0).GetUUID()].SetEnabled(true);
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
				else if (npc->GetState() == State::SEARCHING && m_state != State::PANIC)
					m_state = State::SEARCHING;
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
			pos *= 0.5f;
			SetPos(player->GetPos() + pos);
		}
		float angle = AngleFromPoint(player->GetPos());
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

	if (MoveToTarget(desc.m_tickTimer->Second(), m_miniSearchPos))
	{
		if (m_miniSearchingMeter > 0.0f)
		{
			m_miniSearchingMeter -= desc.m_tickTimer->Second();
		}
		else
		{
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
		PointAtPoint(m_miniSearchPos);
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

#if 0
	add = GetAddFromTarget(point);
#else
	if (GetPos().x < point.x)
		add.x += speed;
	if (GetPos().x > point.x)
		add.x -= speed;
	if (GetPos().y < point.y)
		add.y += speed;
	if (GetPos().y > point.y)
		add.y -= speed;
#endif

	scene->GetAABBs()[GetUUID(0).GetUUID()].SetEnabled(true);
	scene->GetAABBs()[player->GetUUID(0).GetUUID()].SetEnabled(false);

	Move(m_collision, add.x, add.y);

	scene->GetAABBs()[player->GetUUID(0).GetUUID()].SetEnabled(true);
	scene->GetAABBs()[GetUUID(0).GetUUID()].SetEnabled(false);

	if (glm::distance(GetPos(), point) < 0.01f && snapp)
		Move(m_collision, point.x - GetPos().x, point.y - GetPos().y);
	return GetPos() == point;
}

void NPC::PointAtPoint(glm::vec2 point)
{
	m_targetDir = AngleFromPoint(point);
}

float NPC::AngleFromPoint(glm::vec2 point)
{
	point = glm::normalize(point - GetPos());
	glm::vec2 front = glm::vec2(0.0f, 1.0f);

	float degree = glm::degrees(glm::acos(glm::dot(point, front)));
	return (point.x > 0.0f ? degree : 360.0f - degree);
}

glm::vec2 NPC::GetAddFromTarget(glm::vec2 target)
{
	const float BFSPrecision = 0.04f;

	struct Node
	{
		glm::vec2 pos{};
		int add;
	};

	bool assignAdd = true;
	glm::vec2 pos = GetQuad(0)->GetPos();
	Node finalNode{};
	std::queue<Node> bfs;
	bfs.push({ GetQuad(0)->GetPos(), {} });
	//int countDown = 1000;

	float dx[] = { 0.0f, 1.0f, 1.0f,  1.0f,  0.0f, -1.0f, -1.0f, -1.0f };
	float dy[] = { 1.0f, 1.0f, 0.0f, -1.0f, -1.0f, -1.0f,  0.0f,  1.0f };

	std::vector<glm::vec2> notedPos;

	while (!bfs.empty())
	{
		Node curNode = bfs.front();
		glm::vec2 now = curNode.pos;
		bfs.pop();

		GetQuad(0)->SetPos(now);
		if (m_collision->Collide(GetUUID(0)).m_hasHit)
			continue;

		if (glm::distance(now, target) < BFSPrecision)
		{
			finalNode = curNode;
			break;
		}
		
		bool repeated = false;
		for (glm::vec2& i : notedPos)
		{
			if (glm::distance(i, now) < BFSPrecision)
			{
				repeated = true;
				break;
			}
		}

		if (repeated)
			continue;
		else
			notedPos.emplace_back(pos);

		for (int i = 0; i < 7; i++)
		{
			glm::vec2 newPos = now + (glm::vec2(dx[i], dy[i]) * BFSPrecision);
			if (assignAdd)
			{
				bfs.push({ newPos, i });
			}
			else
			{
				bfs.push({ newPos, curNode.add });
			}
		}
		assignAdd = false;
		//countDown--;

		//if (countDown == 0)
		//{
			//finalNode = { {}, glm::normalize(target - pos) };
			//break;
		//}
	}
	GetQuad(0)->SetPos(pos);
	return { dx[finalNode.add], dy[finalNode.add] };
}
