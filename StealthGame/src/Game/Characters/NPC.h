#pragma once

#include <glm/glm.hpp>

#include "../../input/KeyBoard.h"
#include "../../input/Mouse.h"

#include "../Engine/Entity.h"

#include "../Desc.h"

// first quad in the list acts as the hitbox
// second quad in the list acts as the direction
// third quad in the list acts as the character
// quads that are further down the list will be ignored
class NPC : public Entity
{
	friend class Player;
public:
	NPC() = default;

	void NPCTick(GameTickDesc& desc);

	void SetPos(glm::vec2 newPos);
	void ChangePos(glm::vec2 pos);

	const glm::vec2& GetPos() { return GetQuad(0)->GetPos(); }
private:
	void EliminateMyself();
private:
	// 7 0 1
	// 6 + 2
	// 5 4 3
	int m_dir = 0;

	constexpr static glm::vec2 m_dirOffset[8] = {
		{  0.0f,  1.0f },
		{  1.0f,  1.0f },
		{  1.0f,  0.0f },
		{  1.0f, -1.0f },
		{  0.0f, -1.0f },
		{ -1.0f, -1.0f },
		{ -1.0f,  0.0f },
		{ -1.0f,  1.0f },
	};

	constexpr static glm::vec2 m_normalScale = { 0.2f, 0.2f };

	constexpr static float m_speed = 1.5f;
};