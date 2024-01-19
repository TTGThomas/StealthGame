#include "NPC.h"

#include "NPCStats.h"

#include "Player.h"

void NPC::NPCTick(GameTickDesc& desc)
{
	DetectPlayer();
	SetPos(GetPos());

	if (m_state != State::DEAD)
	{
		if (m_type != Type::STATICGUEST)
			TickNonStatic(desc);
		else
			TickStaticGuest(desc);
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
	m_state = State::DEAD;
}

void NPC::SetDirPos(glm::vec2 pos)
{
	Scene* scene = GlobalData::Get().m_scene;
	if (m_state == State::DEAD)
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
	if (m_state == State::DEAD)
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

void NPC::TickStaticGuest(GameTickDesc& desc)
{
}

void NPC::TickGuest(GameTickDesc& desc)
{
}

void NPC::TickGuard(GameTickDesc& desc)
{
}

void NPC::TickNonStatic(GameTickDesc& desc)
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
	}
}

void NPC::TickDead(GameTickDesc& desc)
{
	Scene* scene = GlobalData::Get().m_scene;
	scene->GetRenderQuads()[GetUUID(1).GetUUID()].SetTextureIndex(NPCStats::GetDeadBodyTextureIndex());
	GetQuad(1)->ChangeRotation(0.01f);
}

bool NPC::MoveToTarget(float dt, glm::vec2 point)
{
	Scene* scene = GlobalData::Get().m_scene;

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

	scene->GetAABBs()[m_player->GetUUID(0).GetUUID()].SetEnabled(false);
	scene->GetAABBs()[GetUUID(0).GetUUID()].SetEnabled(true);

	Move(m_collision, add.x, add.y);

	scene->GetAABBs()[GetUUID(0).GetUUID()].SetEnabled(false);
	scene->GetAABBs()[m_player->GetUUID(0).GetUUID()].SetEnabled(true);

	if (glm::distance(GetPos(), point) < 0.1f)
		SetPos(point);
	return GetPos() == point;
}

void NPC::PointAtPoint(glm::vec2 point)
{
	point = glm::normalize(point - GetPos());
	glm::vec2 front = glm::vec2(0.0f, 1.0f);

	float degree = glm::degrees(glm::acos(glm::dot(point, front)));
	m_dir = (point.x > 0.0f ? degree : -degree);
}