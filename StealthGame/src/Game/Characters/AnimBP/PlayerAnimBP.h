#pragma once

#include <functional>
#include <vector>

#include "../../../quadRenderer/Texture.h"

#include "../../GlobalData.h"

class PlayerAnimBP
{
	enum class State
	{
		IDLE, WALKING, CROUCHIDLE, CROUCHWALKING
	};

	struct AnimNode
	{
		AnimNode(
			std::function<bool(class Player*)> determineFunc,
			State representState,
			std::function<void(PlayerAnimBP*, Player*)> execFunc,
			float framesPerSecond,
			int sideFrames,
			int startFrame,
			int endFrame
		)
			:
			m_determineFunc(determineFunc),
			m_representState(representState),
			m_execFunc(execFunc),
			m_framesPerSecond(framesPerSecond),
			m_sideFrames(sideFrames),
			m_startFrame(startFrame),
			m_endFrame(endFrame) {}

		std::function<bool(Player*)> m_determineFunc;
		State m_representState;
		std::function<void(PlayerAnimBP*, Player*)> m_execFunc;
		float m_framesPerSecond;
		int m_sideFrames;
		int m_startFrame;
		int m_endFrame;
	};
public:
	void Init();

	void Tick(Player* player);

	void UpdateArms(Player* player, glm::vec2 leftLook, glm::vec2 rightLook);
private:
	float GetAngleFromVec(glm::vec2 vec);
private:
	std::vector<AnimNode> m_animNodes;
	uint64_t m_leftArm, m_rightArm;
};