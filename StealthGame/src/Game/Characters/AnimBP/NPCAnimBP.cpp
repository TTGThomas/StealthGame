#include "NPCAnimBP.h"

#include "../NPC.h"
#include "../../GameScene.h"

void NPCAnimBP::Init(Identities identity)
{
	GlobalData& gData = GlobalData::Get();

	memcpy(m_idlePath, "res/NPC/NPC*/idle.png", 22 * sizeof(char));
	m_idlePath[11] = ((int)identity - 1) + '0';

	memcpy(m_walkPath, "res/NPC/NPC*/walk.png", 22 * sizeof(char));
	m_walkPath[11] = ((int)identity - 1) + '0';

	printf("idl: %s;\nwalk: %s;\n", m_idlePath, m_walkPath);

	m_animNodes.push_back(AnimNode(
		[](NPC* npc) -> bool
		{
			return
				glm::length(npc->GetVelocity()) == 0.0f;
		},
		State::IDLE,
		[](NPCAnimBP* animBP, NPC* player)
		{
		},
		m_idlePath,
		0.0f,
		1,
		1
		));

	m_animNodes.push_back(AnimNode(
		[](NPC* npc) -> bool
		{
			return
				glm::length(npc->GetVelocity()) != 0.0f;
		},
		State::WALKING,
		[](NPCAnimBP* animBP, NPC* player)
		{
		},
		m_walkPath,
		10.0f,
		3,
		8
		));
}

void NPCAnimBP::Tick(NPC* npc)
{
	uint64_t uuid = npc->GetUUID(1).GetUUID();

	for (AnimNode& node : m_animNodes)
	{
		if (node.m_determineFunc(npc))
		{
			if (m_state != node.m_representState)
				AnimationPlayer::SetAnimationAtlas(uuid, node.m_filePath);
			m_state = node.m_representState;
			node.m_execFunc(this, npc);
			AnimationPlayer::PlayAnimation(uuid, node.m_framesPerSecond, node.m_sideFrames, node.m_validFrames);
			break;
		}
	}
}