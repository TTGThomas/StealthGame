#include "PlayerAnimBP.h"

#include "../Player.h"
#include "../../GameScene.h"

void PlayerAnimBP::Init()
{
	GlobalData& gData = GlobalData::Get();

	Texture playerArmTexture("res/Player/playerArm.png");
	m_playerArmTexture = playerArmTexture.GetUUID().GetUUID();
	gData.m_renderer->AddTexture(playerArmTexture);

	// tex is 4 * 8
	// idleTex is 32 * 32
	// player size is 0.2f
	float pixelSize = 0.2f / 16.0f;
	float halfPSize = pixelSize / 2.0f;
	{
		Quad leftArmQuad;
		leftArmQuad.SetPos({0.0f, 0.0f});
		leftArmQuad.SetRadius({ 4.0f * halfPSize, 8.0f * halfPSize });
		m_leftArm = leftArmQuad.GetUUID().GetUUID();
		RenderQuadInitDesc desc;
		desc.m_depth = 0.25f;
		desc.m_followCameraOffset = true;
		desc.m_shaderUUID = gData.m_defaultShader;
		desc.m_textureUUID = m_playerArmTexture;
		gData.m_scene->AddQuad(leftArmQuad, desc);
	}
	{
		Quad rightArmQuad;
		rightArmQuad.SetPos({ 0.0f, 0.0f });
		rightArmQuad.SetRadius({ 4.0f * halfPSize, 8.0f * halfPSize });
		m_rightArm = rightArmQuad.GetUUID().GetUUID();
		RenderQuadInitDesc desc;
		desc.m_depth = 0.25f;
		desc.m_followCameraOffset = true;
		desc.m_shaderUUID = gData.m_defaultShader;
		desc.m_textureUUID = m_playerArmTexture;
		gData.m_scene->AddQuad(rightArmQuad, desc);
	}

	m_animNodes.push_back(AnimNode(
		[](Player* player) -> bool
		{
			return
				glm::length(player->GetVelocity()) == 0.0f &&
				!player->GetIsCrouching();
		},
		State::IDLE,
		[](PlayerAnimBP* animBP, Player* player)
		{
		},
		"res/Player/playerIdle.png",
		0.0f,
		1,
		1
	));

	m_animNodes.push_back(AnimNode(
		[](Player* player) -> bool
		{
			return
				glm::length(player->GetVelocity()) != 0.0f &&
				!player->GetIsCrouching();
		},
		State::WALKING,
		[](PlayerAnimBP* animBP, Player* player)
		{
		},
		"res/Player/playerWalk.png",
		30.0f,
		3,
		8
	));

	m_animNodes.push_back(AnimNode(
		[](Player* player) -> bool
		{
			return
				glm::length(player->GetVelocity()) == 0.0f &&
				player->GetIsCrouching();
		},
		State::CROUCHIDLE,
		[](PlayerAnimBP* animBP, Player* player)
		{
		},
		"res/Player/playerCrouchIdle.png",
		0.0f,
		1,
		1
	));

	m_animNodes.push_back(AnimNode(
		[](Player* player) -> bool
		{
			return
				glm::length(player->GetVelocity()) != 0.0f &&
				player->GetIsCrouching();
		},
		State::CROUCHWALKING, 
		[this, pixelSize](PlayerAnimBP* animBP, Player* player)
		{
		},
		"res/Player/playerCrouchWalk.png",
		10.0f,
		2,
		4
	));
}

void PlayerAnimBP::Tick(Player* player)
{
	uint64_t uuid = player->GetUUID(2).GetUUID();

	Quad& leftArm = GlobalData::Get().m_scene->GetQuads()[m_leftArm];
	Quad& rightArm = GlobalData::Get().m_scene->GetQuads()[m_rightArm];
	glm::vec2 leftArmPos = leftArm.GetPos() + glm::vec2(0.0f, -1.0f);
	glm::vec2 rightArmPos = rightArm.GetPos() + glm::vec2(0.0f, -1.0f);

	for (AnimNode& node : m_animNodes)
	{
		if (node.m_determineFunc(player))
		{
			if (m_state != node.m_representState)
				AnimationPlayer::SetAnimationAtlas(uuid, node.m_filePath);
			m_state = node.m_representState;
			node.m_execFunc(this, player);
			AnimationPlayer::PlayAnimation(uuid, node.m_framesPerSecond, node.m_sideFrames, node.m_validFrames);
			break;
		}
	}

	float pixelSize = 0.2f / 16.0f;

	if (player->GetInventory().GetEquippiedType() == Inventory::Type::GUN)
	{
		float rotation = player->GetInventory().GetEquipped()->GetQuad(1).GetRotation();
		glm::vec2 pos = player->GetInventory().GetEquipped()->GetQuad(1).GetPos();
		if (rotation > 180.0f)
		{
			leftArmPos = pos;
			if (player->GetIsDragging())
				rightArmPos = GlobalData::Get().m_gameScene->GetNPCs()[player->GetDraggedNPCID().GetUUID()].GetPos();
		}
		else
		{
			rightArmPos = pos;
			if (player->GetIsDragging())
				leftArmPos = GlobalData::Get().m_gameScene->GetNPCs()[player->GetDraggedNPCID().GetUUID()].GetPos();
		}
	}
	else if (player->GetIsDragging())
	{
		leftArmPos = rightArmPos = GlobalData::Get().m_gameScene->GetNPCs()[player->GetDraggedNPCID().GetUUID()].GetPos();
	}

	if (player->GetInventory().GetEquippiedType() == Inventory::Type::FIBERWIRE)
	{
		float averageRot = leftArm.GetRotation();
		glm::vec2 pos = leftArm.GetPos() + rightArm.GetPos();
		pos /= 2.0f;
		pos.x += 3.0f * pixelSize * glm::sin(glm::radians(averageRot));
		pos.y += 3.0f * pixelSize * glm::cos(glm::radians(averageRot));
		player->GetInventory().GetEquipped()->GetQuad(1).SetPos(pos);
	}

	UpdateArms(player, leftArmPos, rightArmPos);
}

void PlayerAnimBP::UpdateArms(Player* player, glm::vec2 leftLook, glm::vec2 rightLook)
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

	float leftAngle = GetAngleFromVec(glm::normalize(leftLook - leftArm.GetPos()));
	float rightAngle = GetAngleFromVec(glm::normalize(rightLook - rightArm.GetPos()));

	leftArm.SetRotation(leftAngle);
	rightArm.SetRotation(rightAngle);

	leftArm.ChangePos(leftArm.GetRadius().x * glm::vec2(glm::sin(glm::radians(leftAngle)), glm::cos(glm::radians(leftAngle))));
	rightArm.ChangePos(rightArm.GetRadius().x * glm::vec2(glm::sin(glm::radians(rightAngle)), glm::cos(glm::radians(rightAngle))));
}

float PlayerAnimBP::GetAngleFromVec(glm::vec2 vec)
{
	glm::vec2 front = glm::vec2(0.0f, 1.0f);

	float degree = glm::degrees(glm::acos(glm::dot(vec, front)));
	return (vec.x > 0.0f ? degree : 360.0f - degree);
}
