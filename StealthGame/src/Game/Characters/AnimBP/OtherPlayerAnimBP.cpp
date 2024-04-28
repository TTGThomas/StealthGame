#include "OtherPlayerAnimBP.h"

#include "../OtherPlayer.h"
#include "../../GameScene.h"

#include "../../SceneLoader/SceneLoader.h"

void OtherPlayerAnimBP::Init()
{
	GlobalData& gData = GlobalData::Get();

	// tex is 4 * 8
	// idleTex is 32 * 32
	// player size is 0.2f
	//pixelSize = 0.2f / 16.0f;
	{
		Quad leftArmQuad;
		leftArmQuad.SetPos({ 0.0f, 0.0f });
		leftArmQuad.SetRadius({ 0.2f, 0.2f });
		m_leftArm = leftArmQuad.GetUUID().GetUUID();
		RenderQuadInitDesc desc;
		desc.m_depth = SceneLoader::Depth(0.25f);
		desc.m_followCameraOffset = true;
		desc.m_shaderUUID = gData.m_defaultShader;
		desc.m_textureUUID = gData.m_texPlayer;
		gData.m_scene->AddQuad(leftArmQuad, desc);
	}
	{
		Quad rightArmQuad;
		rightArmQuad.SetPos({ 0.0f, 0.0f });
		rightArmQuad.SetRadius({ 0.2f, 0.2f });
		m_rightArm = rightArmQuad.GetUUID().GetUUID();
		RenderQuadInitDesc desc;
		desc.m_depth = SceneLoader::Depth(0.25f);
		desc.m_followCameraOffset = true;
		desc.m_shaderUUID = gData.m_defaultShader;
		desc.m_textureUUID = gData.m_texPlayer;
		gData.m_scene->AddQuad(rightArmQuad, desc);
	}
	gData.m_scene->GetRenderQuads()[m_leftArm].SetFrameIndex(1);
	gData.m_scene->GetRenderQuads()[m_leftArm].SetSideFrames(4);
	gData.m_scene->GetRenderQuads()[m_rightArm].SetFrameIndex(1);
	gData.m_scene->GetRenderQuads()[m_rightArm].SetSideFrames(4);

	m_animNodes.push_back(AnimNode(
		[](OtherPlayer* player) -> bool
		{
			return
				player->GetVelocity() == 0.0f &&
				!player->GetIsCrouching();
		},
		State::IDLE,
		[](OtherPlayerAnimBP* animBP, OtherPlayer* player)
		{
		},
		0.0f,
		4,
		7,
		7
		));

	m_animNodes.push_back(AnimNode(
		[](OtherPlayer* player) -> bool
		{
			return
				player->GetVelocity() != 0.0f &&
				!player->GetIsCrouching();
		},
		State::WALKING,
		[](OtherPlayerAnimBP* animBP, OtherPlayer* player)
		{
		},
		30.0f,
		4,
		8,
		15
		));

	m_animNodes.push_back(AnimNode(
		[](OtherPlayer* player) -> bool
		{
			return
				player->GetVelocity() == 0.0f &&
				player->GetIsCrouching();
		},
		State::CROUCHIDLE,
		[](OtherPlayerAnimBP* animBP, OtherPlayer* player)
		{
		},
		0.0f,
		4,
		2,
		2
		));

	m_animNodes.push_back(AnimNode(
		[](OtherPlayer* player) -> bool
		{
			return
				player->GetVelocity() != 0.0f &&
				player->GetIsCrouching();
		},
		State::CROUCHWALKING,
		[this](OtherPlayerAnimBP* animBP, OtherPlayer* player)
		{
		},
		10.0f,
		4,
		3,
		6
		));
}

void OtherPlayerAnimBP::Tick(OtherPlayer* player, float leftRot, float rightRot)
{
	uint64_t uuid = player->GetUUID(2).GetUUID();

	for (AnimNode& node : m_animNodes)
	{
		if (node.m_determineFunc(player))
		{
			node.m_execFunc(this, player);
			AnimationPlayer::PlayAnimation(uuid, node.m_framesPerSecond, node.m_sideFrames, node.m_startFrame, node.m_endFrame);
			break;
		}
	}
	ChangeDisguise(player, player->GetDisguise());

	UpdateArms(player, leftRot, rightRot);
}

void OtherPlayerAnimBP::ChangeDisguise(OtherPlayer* player, Identities type)
{
	GlobalData& gData = GlobalData::Get();
	uint64_t uuid = player->GetUUID(2).GetUUID();

	uint64_t textureUUID = 0;
	if (type == Identities::STANDARD)
		textureUUID = gData.m_texPlayer;
	else if (type == Identities::GUEST)
		textureUUID = gData.m_texNPC0;
	else if (type == Identities::VIPGUEST)
		textureUUID = gData.m_texNPC1;
	else if (type == Identities::GUARD)
		textureUUID = gData.m_texNPC2;
	else if (type == Identities::VIPGUARD)
		textureUUID = gData.m_texNPC3;

	gData.m_scene->GetRenderQuads()[uuid].SetTextureUUID(textureUUID);
	gData.m_scene->GetRenderQuads()[m_leftArm].SetTextureUUID(textureUUID);
	gData.m_scene->GetRenderQuads()[m_rightArm].SetTextureUUID(textureUUID);
}

void OtherPlayerAnimBP::UpdateArms(OtherPlayer* player, float leftRot, float rightRot)
{
	GlobalData& gData = GlobalData::Get();

	float pixelSize = 0.2f / 16.0f;

	Quad& leftArm = gData.m_scene->GetQuads()[m_leftArm];
	Quad& rightArm = gData.m_scene->GetQuads()[m_rightArm];

	if (!player->GetIsCrouching())
	{
		leftArm.SetPos(player->GetPos() + glm::vec2(-5.0f, 0.0f) * glm::vec2(pixelSize));
		rightArm.SetPos(player->GetPos() + glm::vec2(5.0f, 0.0f) * glm::vec2(pixelSize));
	}
	else
	{
		leftArm.SetPos(player->GetPos() + glm::vec2(-5.0f, -3.0f) * glm::vec2(pixelSize));
		rightArm.SetPos(player->GetPos() + glm::vec2(5.0f, -3.0f) * glm::vec2(pixelSize));
	}

	leftArm.SetRotation(leftRot);
	rightArm.SetRotation(rightRot);

	leftArm.ChangePos(14.0f * pixelSize * glm::vec2(glm::sin(glm::radians(leftRot)), glm::cos(glm::radians(leftRot))));
	rightArm.ChangePos(14.0f * pixelSize * glm::vec2(glm::sin(glm::radians(rightRot)), glm::cos(glm::radians(rightRot))));
}

float OtherPlayerAnimBP::GetAngleFromVec(glm::vec2 vec)
{
	glm::vec2 front = glm::vec2(0.0f, 1.0f);

	float degree = glm::degrees(glm::acos(glm::dot(vec, front)));
	return (vec.x > 0.0f ? degree : 360.0f - degree);
}
