#include "Player.h"

void Player::BindCamera(Camera* camera)
{
	m_bindedCamera = camera;
}

void Player::PlayerTick(GameTickDesc& desc)
{
	m_actionType = ActionType::NORMAL;
	GlobalData::Get().m_scene->GetAABBs()[GetUUID(1).GetUUID()].SetEnabled(false);
	if (!m_inputEnabled)
		return;

	MovePlayer(desc);

	if (!m_isCrouching)
		m_isDragging = false;

	if (KeyBoard::IsKeyPressDown(GLFW_KEY_SPACE))
	{
		if (m_isHidden)
			UnHidePlayer();
	}
}

void Player::HidePlayer(glm::vec2 pos)
{
	m_isHidden = true;
	m_posBeforeHidden = GetPos();
	SetPos(pos);
}

void Player::UnHidePlayer()
{
	SetPos(m_posBeforeHidden);
	m_isHidden = false;
	m_isCrouching = false;
}

void Player::EliminateNPC(NPC& victim)
{
	SetPos(victim.GetPos());
	m_actionType = ActionType::ILLEGAL;
}

void Player::MovePlayer(GameTickDesc& desc)
{
	float speed = (m_isCrouching ? m_crouchSpeed : m_normalSpeed) * desc.m_tickTimer->Second();
	m_velocity = {};
	if (KeyBoard::IsKeyDown(GLFW_KEY_W))
		m_velocity.y += speed;
	if (KeyBoard::IsKeyDown(GLFW_KEY_A))
		m_velocity.x -= speed;
	if (KeyBoard::IsKeyDown(GLFW_KEY_S))
		m_velocity.y -= speed;
	if (KeyBoard::IsKeyDown(GLFW_KEY_D))
		m_velocity.x += speed;

	Move(desc.m_collision, m_velocity.x, m_velocity.y);

	desc.m_camera->SetPos(GetQuad(2)->GetPos());

	// crouch / unCrouch accoding to the input
	if (KeyBoard::IsKeyPressDown(GLFW_KEY_C))
		m_isCrouching = !m_isCrouching;

	GetQuad(0)->SetRadius(m_isCrouching ? m_crouchScale : m_normalScale);
	// check if in wall
	if (desc.m_collision->Collide(GetUUID(0)).m_hasHit)
		m_isCrouching = true;

	// synch hitbox and character
	GetQuad(2)->SetRadius(GetQuad(0)->GetRadius());
	GetQuad(2)->SetPos(GetQuad(0)->GetPos());
}