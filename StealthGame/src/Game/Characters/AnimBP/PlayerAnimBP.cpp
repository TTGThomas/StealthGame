#include "PlayerAnimBP.h"

#include "../Player.h"
#include "../../GameScene.h"

#include "../../SceneLoader/SceneLoader.h"

void PlayerAnimBP::Init()
{
	GlobalData& gData = GlobalData::Get();

	// idleTex is 32 * 32
	// player scale is 0.45
	// pixelSize = 0.45f / 32;
	{
		Quad leftArmQuad;
		leftArmQuad.SetPos({0.0f, 0.0f});
		leftArmQuad.SetRadius(glm::vec2(MAP_RADIUS));
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
		rightArmQuad.SetRadius(glm::vec2(MAP_RADIUS));
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
		0.0f,
		4,
		7,
		7
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
		30.0f,
		4,
		8,
		15
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
		0.0f,
		4,
		2,
		2
	));

	m_animNodes.push_back(AnimNode(
		[](Player* player) -> bool
		{
			return
				glm::length(player->GetVelocity()) != 0.0f &&
				player->GetIsCrouching();
		},
		State::CROUCHWALKING, 
		[this](PlayerAnimBP* animBP, Player* player)
		{
		},
		10.0f,
		4,
		3,
		6
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
			node.m_execFunc(this, player);
			AnimationPlayer::PlayAnimation(uuid, node.m_framesPerSecond, node.m_sideFrames, node.m_startFrame, node.m_endFrame);
			break;
		}
	}

	float pixelSize = MAP_SCALE / 32.0f;

	if (player->GetInventory().GetEquippiedType() == Inventory::Type::GUN)
	{
		float rotation = player->GetInventory().GetEquipped()->GetQuad(1).GetRotation();
		glm::vec2 pos = player->GetInventory().GetEquipped()->GetQuad(1).GetPos();
		if (rotation > 180.0f)
		{
			leftArmPos = pos;
			if (player->GetIsDragging())
				rightArmPos = GlobalData::Get().m_gameScene->GetNPCs()[player->GetDraggedNPCID().GetUUID()]->GetPos();
		}
		else
		{
			rightArmPos = pos;
			if (player->GetIsDragging())
				leftArmPos = GlobalData::Get().m_gameScene->GetNPCs()[player->GetDraggedNPCID().GetUUID()]->GetPos();
		}
	}
	else if (player->GetIsDragging())
	{
		leftArmPos = rightArmPos = GlobalData::Get().m_gameScene->GetNPCs()[player->GetDraggedNPCID().GetUUID()]->GetPos();
	}

	if (player->GetInventory().GetEquippiedType() == Inventory::Type::FIBERWIRE)
	{
		float averageRot = leftArm.GetRotation();

		glm::vec2 pos = leftArm.GetPos() + rightArm.GetPos();
		pos /= 2.0f;
		pos.x += -8.0f * pixelSize * glm::sin(glm::radians(averageRot));
		pos.y += -8.0f * pixelSize * glm::cos(glm::radians(averageRot));
		player->GetInventory().GetEquipped()->GetQuad(1).SetPos(pos);
	}
	else if (player->GetInventory().GetEquippiedType() == Inventory::Type::COIN)
	{
		glm::vec2 pos = leftArm.GetPos();
		pos.x += -8.0f * pixelSize * glm::sin(glm::radians(leftArm.GetRotation()));
		pos.y += -8.0f * pixelSize * glm::cos(glm::radians(leftArm.GetRotation()));
		player->GetInventory().GetEquipped()->GetQuad(1).SetPos(pos);
	}
	else if (player->GetInventory().GetEquippiedType() == Inventory::Type::POISON)
	{
		glm::vec2 pos = leftArm.GetPos();
		pos.x += -8.0f * pixelSize * glm::sin(glm::radians(leftArm.GetRotation()));
		pos.y += -8.0f * pixelSize * glm::cos(glm::radians(leftArm.GetRotation()));
		player->GetInventory().GetEquipped()->GetQuad(1).SetPos(pos);
	}

	UpdateArms(player, leftArmPos, rightArmPos);
}

void PlayerAnimBP::ChangeDisguise(Player* player, Identities type)
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

void PlayerAnimBP::UpdateArms(Player* player, glm::vec2 leftLook, glm::vec2 rightLook)
{
	GlobalData& gData = GlobalData::Get();

	float pixelSize = MAP_SCALE / 32.0f;
	
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

	m_leftRot = GetAngleFromVec(glm::normalize(leftLook - leftArm.GetPos()));
	m_rightRot = GetAngleFromVec(glm::normalize(rightLook - rightArm.GetPos()));

	leftArm.SetRotation(m_leftRot);
	rightArm.SetRotation(m_rightRot);

	leftArm.ChangePos(14.0f * pixelSize * glm::vec2(glm::sin(glm::radians(m_leftRot)), glm::cos(glm::radians(m_leftRot))));
	rightArm.ChangePos(14.0f * pixelSize * glm::vec2(glm::sin(glm::radians(m_rightRot)), glm::cos(glm::radians(m_rightRot))));
}

float PlayerAnimBP::GetAngleFromVec(glm::vec2 vec)
{
	glm::vec2 front = glm::vec2(0.0f, 1.0f);

	float degree = glm::degrees(glm::acos(glm::dot(vec, front)));
	return (vec.x > 0.0f ? degree : 360.0f - degree);
}
