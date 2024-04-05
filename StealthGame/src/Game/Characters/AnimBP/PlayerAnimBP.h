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
			const char* filePath, float framesPerSecond,
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

		std::function<bool(Player*)> m_determineFunc;
		State m_representState;
		std::function<void(PlayerAnimBP*, Player*)> m_execFunc;
		const char* m_filePath;
		float m_framesPerSecond;
		int m_sideFrames;
		int m_validFrames;
	};
public:
	void Init();

	void Tick(Player* player);

	void UpdateArms(Player* player, glm::vec2 leftLook, glm::vec2 rightLook);

	void SetState(State state) { m_state = state; }
private:
	float GetAngleFromVec(glm::vec2 vec);
private:
	std::vector<AnimNode> m_animNodes;
	State m_state;

	uint64_t m_playerArmTexture;
	uint64_t m_leftArm, m_rightArm;
};