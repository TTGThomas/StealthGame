#include "NPCAnimBP.h"

#include "../NPC.h"
#include "../../GameScene.h"

void NPCAnimBP::Init(NPC* npc)
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
		desc.m_depth = 0.25f;
		desc.m_followCameraOffset = true;
		desc.m_shaderUUID = gData.m_defaultShader;
		desc.m_textureUUID = gData.m_scene->GetRenderQuads()[npc->GetUUID(1).GetUUID()].GetTextureUUID();
		gData.m_scene->AddQuad(leftArmQuad, desc);
	}
	{
		Quad rightArmQuad;
		rightArmQuad.SetPos({ 0.0f, 0.0f });
		rightArmQuad.SetRadius({ 0.2f, 0.2f });
		m_rightArm = rightArmQuad.GetUUID().GetUUID();
		RenderQuadInitDesc desc;
		desc.m_depth = 0.25f;
		desc.m_followCameraOffset = true;
		desc.m_shaderUUID = gData.m_defaultShader;
		desc.m_textureUUID = gData.m_scene->GetRenderQuads()[npc->GetUUID(1).GetUUID()].GetTextureUUID();
		gData.m_scene->AddQuad(rightArmQuad, desc);
	}
	gData.m_scene->GetRenderQuads()[m_leftArm].SetFrameIndex(1);
	gData.m_scene->GetRenderQuads()[m_leftArm].SetSideFrames(4);
	gData.m_scene->GetRenderQuads()[m_rightArm].SetFrameIndex(1);
	gData.m_scene->GetRenderQuads()[m_rightArm].SetSideFrames(4);

	m_animNodes.push_back(AnimNode(
		[](NPC* npc) -> bool
		{
			return
				glm::length(npc->GetVelocity()) == 0.0f;
		},
		State::IDLE,
		[](NPCAnimBP* animBP, NPC* npc)
		{
		},
		0.0f,
		4,
		7,
		7
		));

	m_animNodes.push_back(AnimNode(
		[](NPC* npc) -> bool
		{
			return
				glm::length(npc->GetVelocity()) != 0.0f;
		},
		State::WALKING,
		[](NPCAnimBP* animBP, NPC* npc)
		{
		},
		30.0f,
		4,
		8,
		15
		));
}

void NPCAnimBP::Tick(NPC* npc)
{
	uint64_t uuid = npc->GetUUID(1).GetUUID();

	Quad& leftArm = GlobalData::Get().m_scene->GetQuads()[m_leftArm];
	Quad& rightArm = GlobalData::Get().m_scene->GetQuads()[m_rightArm];
	glm::vec2 leftArmPos = leftArm.GetPos() + glm::vec2(0.0f, -1.0f);
	glm::vec2 rightArmPos = rightArm.GetPos() + glm::vec2(0.0f, -1.0f);

	for (AnimNode& node : m_animNodes)
	{
		if (node.m_determineFunc(npc))
		{
			node.m_execFunc(this, npc);
			AnimationPlayer::PlayAnimation(uuid, node.m_framesPerSecond, node.m_sideFrames, node.m_startFrame, node.m_endFrame);
			break;
		}
	}
	UpdateArms(npc, leftArmPos, rightArmPos);
}

void NPCAnimBP::UpdateArms(NPC* npc, glm::vec2 leftLook, glm::vec2 rightLook)
{
	GlobalData& gData = GlobalData::Get();

	float pixelSize = 0.2f / 16.0f;

	Quad& leftArm = gData.m_scene->GetQuads()[m_leftArm];
	Quad& rightArm = gData.m_scene->GetQuads()[m_rightArm];

	leftArm.SetPos(npc->GetPos() + glm::vec2(-5.0f, 0.0f) * glm::vec2(pixelSize));
	rightArm.SetPos(npc->GetPos() + glm::vec2(5.0f, 0.0f) * glm::vec2(pixelSize));

	float leftAngle = GetAngleFromVec(glm::normalize(leftLook - leftArm.GetPos()));
	float rightAngle = GetAngleFromVec(glm::normalize(rightLook - rightArm.GetPos()));

	leftArm.SetRotation(leftAngle);
	rightArm.SetRotation(rightAngle);

	leftArm.ChangePos(14.0f * pixelSize * glm::vec2(glm::sin(glm::radians(leftAngle)), glm::cos(glm::radians(leftAngle))));
	rightArm.ChangePos(14.0f * pixelSize * glm::vec2(glm::sin(glm::radians(rightAngle)), glm::cos(glm::radians(rightAngle))));
}

float NPCAnimBP::GetAngleFromVec(glm::vec2 vec)
{
	glm::vec2 front = glm::vec2(0.0f, 1.0f);

	float degree = glm::degrees(glm::acos(glm::dot(vec, front)));
	return (vec.x > 0.0f ? degree : 360.0f - degree);
}