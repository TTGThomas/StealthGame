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
	DetectEverything();
	SetPos(GetPos());

	if (m_health > 0)
	{
		TickNonStatic(desc);
		if (m_type == Identities::GUEST || m_type == Identities::VIPGUEST)
			TickGuest(desc);
		else if (m_type == Identities::GUARD || m_type == Identities::VIPGUARD)
			TickGuard(desc);
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
	SetDirPos(newPos);
}

void NPC::ChangePos(glm::vec2 pos)
{
	GetQuad(0)->ChangePos(pos);
	GetQuad(1)->ChangePos(pos);
	SetDirPos(GetQuad(0)->GetPos());
}

void NPC::EliminateMyself()
{
	m_health = 0;
}

void NPC::SetDirPos(glm::vec2 pos)
{
	Scene* scene = GlobalData::Get().m_scene;
	if (m_health == 0)
	{
		scene->GetRenderQuads()[GetUUID(2).GetUUID()].SetVisibility(false);
		return;
	}
	glm::vec2 p = pos;
	p.x += glm::sin(glm::radians(m_dir)) * scene->GetQuads()[GetUUID(2).GetUUID()].GetRadius().x;
	p.y += glm::cos(glm::radians(m_dir)) * scene->GetQuads()[GetUUID(2).GetUUID()].GetRadius().x;
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
			m_state = State::WITNESS;
		}
	}

	if (m_state == State::WITNESS)
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
			m_state = State::WITNESS;
			gData.m_globalState = GlobalState::ALERT;
			gData.m_disguiseStates[(int)Identities::STANDARD] = DisguiseState::ALERT;
		}
	}

	if (gData.m_globalState == GlobalState::ALERT)
	{
		m_state = State::WITNESS;
		glm::vec2 pos = glm::normalize(GetQuad(0)->GetPos() - player->GetPos());
		pos *= 1.5f;
		MoveToTarget(desc.m_tickTimer->Second(), player->GetPos() + pos);
		PointAtPoint(player->GetPos());
	}

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
			gData.m_globalState = GlobalState::ALERT;
			if (m_detectedDeadNPCs.find(uuid) == m_detectedDeadNPCs.end())
			{
				m_detectedDeadNPCs.insert(uuid);
				gData.m_bodiesFound++;
			}
		}
	}
}

void NPC::TickNonStatic(GameTickDesc& desc)
{
	GlobalData& gData = GlobalData::Get();
	Player* player = &gData.m_gameScene->GetPlayer();

	if (m_state != State::WITNESS)
	{
		if (IsPlayerDetected())
			m_state = State::NORMAL;

		float dst = glm::distance(GetPos(), player->GetPos());
		if (dst < 0.2f)
		{
			m_suspiciousMeter = 1.0f;
		}

		else if (dst < 0.5f)
		{
			if (glm::length(player->GetVelocity()) != 0.0f)
			{
				if (!player->GetIsCrouching())
				{
					m_suspiciousMeter = 1.0f;
				}
			}
		}

		if (m_suspiciousMeter > 0.0f)
		{
			m_state = State::SUSPICIOUS;
			m_suspiciousMeter -= desc.m_tickTimer->Second();
		}
		else
		{
			m_suspiciousMeter = 0.0f;
			m_state = State::NORMAL;
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
	
	if (m_isBeingDragged)
	{
		// do stuffs related to being dragged
		Player* player = &GlobalData::Get().m_gameScene->GetPlayer();
		if (!player->GetIsCrouching())
			m_isBeingDragged = false;
		if (glm::distance(GetPos(), player->GetPos()) > 0.5f)
		{
			glm::vec2 pos = glm::normalize(GetQuad(0)->GetPos() - player->GetPos());
			pos *= 0.5f;
			SetPos(player->GetPos() + pos);
		}
		scene->GetQuads()[GetUUID(1).GetUUID()].SetRotation(AngleFromPoint(player->GetPos()) + 90.0f);
	}
}

bool NPC::IsThetaInView(float cosTheta)
{
	//return glm::acos(cosTheta) < glm::radians(30.0f);
	//return cosTheta < glm::cos(glm::radians(30.0f));
	// cos(radians(30.0f)) = 0.866025388;
	// somehow i need to flip the result to be correct;
	return cosTheta > 0.866025388f;
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
