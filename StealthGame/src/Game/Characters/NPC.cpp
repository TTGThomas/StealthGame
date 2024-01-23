#include "NPC.h"

#include "Player.h"

void NPC::NPCTick(GameTickDesc& desc)
{
	DetectPlayer();
	SetPos(GetPos());

	if (m_health > 0)
	{
		TickNonStatic(desc);
		if (m_type == Type::GUEST || m_type == Type::VIPGUEST)
			TickGuest(desc);
		else if (m_type == Type::GUARD || m_type == Type::VIPGUARD)
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
	glm::vec2 front{};
	front.x += glm::sin(glm::radians(m_dir));
	front.y += glm::cos(glm::radians(m_dir));

	glm::vec2 player = glm::normalize(m_player->GetPos() - GetPos());

	float cosTheta = glm::dot(front, player);

	if (glm::acos(cosTheta) > glm::radians(30.0f))
		return false;

	float dst = glm::distance(GetPos(), m_player->GetPos());
	if (dst > 3.0f)
		return false;

	Scene* scene = GlobalData::Get().m_scene;

	scene->GetAABBs()[GetUUID(0).GetUUID()].SetEnabled(false);
	scene->GetAABBs()[m_player->GetUUID(0).GetUUID()].SetEnabled(false);
	bool ret = !m_collision->Collide(GetPos(), m_player->GetPos()).m_hasHit;
	scene->GetAABBs()[m_player->GetUUID(0).GetUUID()].SetEnabled(true);
	scene->GetAABBs()[GetUUID(0).GetUUID()].SetEnabled(true);
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

void NPC::TickGuest(GameTickDesc& desc)
{
	if (IsPlayerDetected())
	{
		if (m_player->GetActionType() == Player::ActionType::ILLEGAL)
		{
			m_state = State::WITNESS;
		}
	}

	if (m_state == State::WITNESS)
	{
		//MoveToTarget(desc.m_tickTimer->Second(), m_player->GetPos(), false);
		//PointAtPoint(m_player->GetPos());
		EliminateMyself();
	}
}

void NPC::TickGuard(GameTickDesc& desc)
{
	if (IsPlayerDetected())
	{
		if (m_player->GetActionType() == Player::ActionType::ILLEGAL)
		{
			m_state = State::WITNESS;
			GlobalData::Get().m_globalState = GlobalState::ALERT;
		}
	}

	if (GlobalData::Get().m_globalState == GlobalState::ALERT)
	{
		m_state = State::WITNESS;
		glm::vec2 pos = glm::normalize(GetQuad(0)->GetPos() - m_player->GetPos());
		pos *= 1.5f;
		MoveToTarget(desc.m_tickTimer->Second(), m_player->GetPos() + pos, false);
		PointAtPoint(m_player->GetPos());
	}
}

void NPC::TickNonStatic(GameTickDesc& desc)
{
	if (m_state != State::WITNESS)
	{
		if (IsPlayerDetected())
			m_state = State::NORMAL;

		float dst = glm::distance(GetPos(), m_player->GetPos());
		if (dst < 0.2f)
		{
			m_suspiciousMeter = 1.0f;
		}
		else if (dst < 0.5f)
		{
			if (glm::length(m_player->GetVelocity()) != 0.0f)
			{
				if (!m_player->GetIsCrouching())
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
			PointAtPoint(m_player->GetPos());
		}

		if (m_state == State::NORMAL)
		{
			if (m_route.size() > 0)
			{
				if (MoveToTarget(desc.m_tickTimer->Second(), m_route[m_targetRouteIndex].m_pos))
				{
					m_targetRouteIndex++;
					if (m_targetRouteIndex >= m_route.size())
						m_targetRouteIndex -= m_route.size();
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
	float dirSpeed = desc.m_tickTimer->Second() * 500.0f;
	if (amount > 0.0f)
		m_dir += glm::min(dirSpeed, amount);
	else
		m_dir -= glm::min(dirSpeed, -amount);
}

void NPC::TickDead(GameTickDesc& desc)
{
	Scene* scene = GlobalData::Get().m_scene;
	scene->GetRenderQuads()[GetUUID(1).GetUUID()].SetTextureUUID(GlobalData::Get().m_texNPCDead);
}

bool NPC::MoveToTarget(float dt, glm::vec2 point, bool snapp)
{
	Scene* scene = GlobalData::Get().m_scene;

	glm::vec2 add{};
	float speed = m_speed * dt;

#if 1
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
	scene->GetAABBs()[m_player->GetUUID(0).GetUUID()].SetEnabled(false);

	Move(m_collision, add.x, add.y);

	scene->GetAABBs()[m_player->GetUUID(0).GetUUID()].SetEnabled(true);
	scene->GetAABBs()[GetUUID(0).GetUUID()].SetEnabled(false);

	if (glm::distance(GetPos(), point) < 0.1f && snapp)
		SetPos(point);
	return GetPos() == point;
}

void NPC::PointAtPoint(glm::vec2 point)
{
	point = glm::normalize(point - GetPos());
	glm::vec2 front = glm::vec2(0.0f, 1.0f);

	float degree = glm::degrees(glm::acos(glm::dot(point, front)));
	m_targetDir = (point.x > 0.0f ? degree : 360.0f - degree);
}

glm::vec2 NPC::GetAddFromTarget(glm::vec2 target)
{
	const float BFSPrecision = 0.4f;

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
