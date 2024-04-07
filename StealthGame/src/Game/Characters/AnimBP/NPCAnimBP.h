#pragma once

#include <functional>
#include <vector>

#include "../../../quadRenderer/Texture.h"

#include "../../GlobalData.h"

class NPCAnimBP
{
	enum class State
	{
		IDLE, WALKING
	};

	struct AnimNode
	{
		AnimNode(
			std::function<bool(class NPC*)> determineFunc,
			State representState,
			std::function<void(NPCAnimBP*, NPC*)> execFunc,
			char* filePath,
			float framesPerSecond,
			int sideFrames,
			int validFrames
		)
			:
			m_determineFunc(determineFunc),
			m_representState(representState),
			m_execFunc(execFunc),
			m_filePath(filePath),
			m_framesPerSecond(framesPerSecond),
			m_sideFrames(sideFrames),
			m_validFrames(validFrames) {}

		std::function<bool(NPC*)> m_determineFunc;
		State m_representState;
		std::function<void(NPCAnimBP*, NPC*)> m_execFunc;
		char* m_filePath;
		float m_framesPerSecond;
		int m_sideFrames;
		int m_validFrames;
	};
public:
	NPCAnimBP() = default;

	void Init(Identities identity);

	void Tick(NPC* npc);

	void SetState(State state) { m_state = state; }
private:
	std::vector<AnimNode> m_animNodes;
	State m_state;

	char m_idlePath[22];
	char m_walkPath[22];
};