#pragma once

#include <queue>
#include <unordered_set>
#include <vector>

#include <glm/glm.hpp>

#include "../../input/KeyBoard.h"
#include "../../input/Mouse.h"

#include "../Engine/Entity.h"

#include "../Desc.h"

#include "../Items/Item.h"

#include "../GlobalData.h"

class Player;
class CollisionDetector;

struct NPCRoutePoint
{
	glm::vec2 m_pos;
};

// first quad in the list acts as the hitbox
// second quad in the list acts as the character
// third quad in the list acts as the direction
// quads that are further down the list will be ignored
class NPC : public Entity
{
public:
	enum class Type
	{
		GUEST, VIPGUEST, GUARD, VIPGUARD
	};

	enum class State
	{
		NORMAL, SUSPICIOUS, WITNESS
	};
public:
	NPC() = default;

	//inline void Init();

	void BindCollision(CollisionDetector* collision) { m_collision = collision; }

	void BindRoute(std::vector<NPCRoutePoint>& route) { m_route = route; }

	void SetType(Type type) { m_type = type; }
	void SetState(State state) { m_state = state; }

	void NPCTick(GameTickDesc& desc);

	void SetPos(glm::vec2 newPos);
	void ChangePos(glm::vec2 pos);

	const glm::vec2& GetPos() { return GetQuad(0)->GetPos(); }

	void EliminateMyself();

	bool IsPlayerDetected();

	float GetSuspiciousMeter() { return m_suspiciousMeter; }
	State GetState() { return m_state; }
	UUID& GetNPCUUID() { return m_uuid; }
	int GetHealth() { return m_health; }
	Type GetType() { return m_type; }
private:
	void SetDirPos(glm::vec2 pos);
private:
	bool IsPlayerInSight();
	void DetectPlayer();
	void DetectItems();
	void DetectNPCs();

	// O(n^2 + n)
	void DetectEverything();
private:
	void UpdateFrontVec();
private:
	void TickGuest(GameTickDesc& desc);
	void TickGuard(GameTickDesc& desc);
	void TickNonStatic(GameTickDesc& desc);
	void TickDead(GameTickDesc& desc);
private:
	// returns if it is at point or not
	bool MoveToTarget(float dt, glm::vec2 point, bool snapp = true);
	void PointAtPoint(glm::vec2 point);
	glm::vec2 GetAddFromTarget(glm::vec2 target);
private:
	UUID m_uuid;
	CollisionDetector* m_collision;

	bool m_isPlayerDetected = false;

	float m_dir = 0.0f;
	float m_targetDir = 0.0f;

	Type m_type = Type::GUEST;
	State m_state = State::NORMAL;
	float m_suspiciousMeter = 0;
	int m_health = 100;

	std::vector<NPCRoutePoint> m_route;
	int m_targetRouteIndex = 0;

	glm::vec2 m_frontVec{};
	
	std::vector<uint64_t> m_detectedItems;
	std::vector<uint64_t> m_detectedNPCs;

	constexpr static glm::vec2 m_normalScale = { 0.2f, 0.2f };

	constexpr static float m_speed = 0.8f;
};