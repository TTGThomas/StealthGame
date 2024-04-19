#pragma once

#include <functional>
#include <vector>

#include "../../../quadRenderer/Texture.h"

#include "../../GlobalData.h"

class OtherPlayerAnimBP
{
	enum class State
	{
		IDLE, WALKING, CROUCHIDLE, CROUCHWALKING
	};

	struct AnimNode
	{
		AnimNode(
			std::function<bool(class OtherPlayer*)> determineFunc,
			State representState,
			std::function<void(OtherPlayerAnimBP*, OtherPlayer*)> execFunc,
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

		std::function<bool(OtherPlayer*)> m_determineFunc;
		State m_representState;
		std::function<void(OtherPlayerAnimBP*, OtherPlayer*)> m_execFunc;
		float m_framesPerSecond;
		int m_sideFrames;
		int m_startFrame;
		int m_endFrame;
	};
public:
	void Init();

	void Tick(OtherPlayer* player, float leftRot, float rightRot);

	void ChangeDisguise(OtherPlayer* player, Identities type);

	void UpdateArms(OtherPlayer* player, float leftRot, float rightRot);
private:
	float GetAngleFromVec(glm::vec2 vec);
private:
	std::vector<AnimNode> m_animNodes;
	uint64_t m_leftArm, m_rightArm;
};