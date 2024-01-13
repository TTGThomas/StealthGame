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
public:
	enum class Type
	{
		STATICGUEST, GUEST, VIPGUEST, GUARD, VIPGUARD
	};

	enum class State
	{
		NORMAL, ALERT, DEAD
	};
public:
	NPC() = default;

	//inline void Init();

	void SetType(Type type) { m_type = type; }
	void SetState(State state) { m_state = state; }

	void NPCTick(GameTickDesc& desc);

	void SetPos(glm::vec2 newPos);
	void ChangePos(glm::vec2 pos);

	const glm::vec2& GetPos() { return GetQuad(0)->GetPos(); }

	void EliminateMyself();
private:
	void TickStaticGuest();
	void TickGuest();
	void TickGuard();
	void TickNonStatic();
private:
	// 7 0 1
	// 6 + 2
	// 5 4 3
	int m_dir = 0;

	Type m_type = Type::GUEST;
	State m_state = State::NORMAL;

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