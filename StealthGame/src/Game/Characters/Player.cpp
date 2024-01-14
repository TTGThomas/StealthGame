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
}

void Player::EliminateNPC(NPC& victim)
{
	SetPos(victim.GetPos());
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