#include "OtherPlayer.h"

OtherPlayer::OtherPlayer()
{
}

void OtherPlayer::Init(std::vector<QuadInitDesc>& descs, bool isSpecial, bool isDoor)
{
	Entity::Init(descs);
	m_animBP.Init();
}

void OtherPlayer::ClearResources()
{
}

void OtherPlayer::Tick(std::array<unsigned char, PATCHSIZE>& data)
{
	// decrypt position
	PassData recvData = *(PassData*)data.data();
	glm::vec2 pos = { recvData.m_x, recvData.m_y };
	m_velocity = recvData.m_vel;

	m_animBP.Tick(this, recvData.m_leftRot, recvData.m_rightRot);
	SetPos(pos);
	m_disguise = (Identities)recvData.m_disguise;

	// sync
	GetQuad(2)->SetPos(pos);

	m_lastTime = (float)glfwGetTime();
}

void OtherPlayer::HidePlayer(glm::vec2 pos)
{
	m_isHidden = true;
	m_posBeforeHidden = GetPos();
	SetPos(pos);
}

void OtherPlayer::UnHidePlayer()
{
	SetPos(m_posBeforeHidden);
	m_isHidden = false;
	m_isCrouching = false;
}