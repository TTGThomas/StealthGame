#include "Player.h"

void Player::BindCamera(Camera* camera)
{
	m_bindedCamera = camera;
}

void Player::BindNPCs(std::vector<NPC>* npcs)
{
	m_npcs = npcs;
}

void Player::PlayerTick(GameTickDesc& desc)
{
	if (!m_inputEnabled)
		return;

	MovePlayer(desc);
	InteractPlayer(desc);
}

void Player::EliminateNPC(NPC& victim)
{
	victim.EliminateMyself();
	SetPos(victim.GetPos());
}

void Player::MovePlayer(GameTickDesc& desc)
{
	float speed = (m_isCrouching ? m_crouchSpeed : m_normalSpeed) * desc.m_tickTimer->Second();
	glm::vec2 add{};
	if (KeyBoard::IsKeyDown(GLFW_KEY_W))
		add.y += speed;
	if (KeyBoard::IsKeyDown(GLFW_KEY_A))
		add.x -= speed;
	if (KeyBoard::IsKeyDown(GLFW_KEY_S))
		add.y -= speed;
	if (KeyBoard::IsKeyDown(GLFW_KEY_D))
		add.x += speed;

	Move(desc.m_collision, add.x, add.y);

	desc.m_camera->SetPos(GetQuad(0)->GetPos());

	// crouch / unCrouch accoding to the input
	if (KeyBoard::IsKeyPressDown(GLFW_KEY_C))
		m_isCrouching = !m_isCrouching;

	GetQuad(0)->SetScale(m_isCrouching ? m_crouchScale : m_normalScale);
	// check if in wall
	if (desc.m_collision->Collide(GetIndex(0)).m_hasHit)
		m_isCrouching = true;

	// synch hitbox and character
	GetQuad(2)->SetScale(GetQuad(0)->GetScale());
	GetQuad(2)->SetPos(GetQuad(0)->GetPos());
}

void Player::InteractPlayer(GameTickDesc& desc)
{
	m_map->GetQuads()[GetIndex(1)].SetVisibility(false);
	for (NPC& npc : *m_npcs)
	{
		glm::vec2 diff = npc.GetPos() - GetPos();
		if (glm::length(diff) < 0.5f)
		{
			m_map->GetQuads()[GetIndex(1)].SetPos(npc.GetPos());
			m_map->GetQuads()[GetIndex(1)].SetVisibility(true);
			if (KeyBoard::IsKeyPressDown(GLFW_KEY_E))
			{
				EliminateNPC(npc);
			}
			break;
		}
	}
}