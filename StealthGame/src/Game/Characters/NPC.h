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
	enum class State
	{
		NORMAL, SUSPICIOUS, SEARCHING, PANIC
	};

	enum class SearchType
	{
		DEADBODY, ILLEGALWEAPON, GUNSHOT
	};
public:
	NPC() = default;

	void Init(std::vector<QuadInitDesc> desc, const char* name);

	void BindCollision(CollisionDetector* collision) { m_collision = collision; }

	void BindRoute(std::vector<NPCRoutePoint>& route) { m_route = route; }

	void SetType(Identities type) { m_type = type; }
	void SetState(State state) { m_state = state; }

	void NPCTick(GameTickDesc& desc);

	void SetPos(glm::vec2 newPos);
	void ChangePos(glm::vec2 pos);

	const glm::vec2& GetPos() { return GetQuad(0)->GetPos(); }

	void EliminateMyself();
	void WhenHitBullet() { m_health -= 40; }

	bool IsPlayerDetected();

	void SetIsBeingDragged(bool isBeingDragged) { m_isBeingDragged = isBeingDragged; }
	void SetIsDisposed(bool val) { m_isDisposed = val; }

	void SetSpeed(float speed) { m_speed = speed; }

	void SetSearchType(SearchType type) { m_searchType = type; }

	float GetSuspiciousMeter() { return m_suspiciousMeter; }
	State GetState() { return m_state; }
	UUID& GetNPCUUID() { return m_uuid; }
	int GetHealth() { return m_health; }
	Identities GetType() { return m_type; }
	const char* GetName() { return m_name; }
	bool GetIsBeingDragged() { return m_isBeingDragged; }
	bool GetIsDisposed() { return m_isDisposed; }
	float GetSpeed() { return m_speed; }
	SearchType GetSearchType() { return m_searchType; }
private:
	void SetDirPos(glm::vec2 pos);
private:
	void ApplyDamage();
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
	void TickGuardSearchBody(GameTickDesc& desc);
	void TickGuardSearchGunShot(GameTickDesc& desc);
	void TickGuardSearchILLEGALWEAPON(GameTickDesc& desc);
private:
	inline bool IsThetaInView(float cosTheta);
private:
	void Search(glm::vec2 where, float time, SearchType type)
	{
		m_state = State::SEARCHING;
		m_searchingMeter = time;
		m_searchPos = where;
		m_miniSearchPos = m_searchPos;
		m_searchType = type;
	}
private:
	// returns if it is at point or not
	bool MoveToTarget(float dt, glm::vec2 point, bool snapp = true);
	void PointAtPoint(glm::vec2 point);
	float AngleFromPoint(glm::vec2 point);
	glm::vec2 GetAddFromTarget(glm::vec2 target);
	void NPCMove(glm::vec2 vec);
private:
	UUID m_uuid;
	CollisionDetector* m_collision;

	const char* m_name;

	bool m_isPlayerDetected = false;

	bool m_isBeingDragged = false;
	bool m_isDisposed = false;

	bool m_isWitness = false;

	float m_dir = 0.0f;
	float m_targetDir = 0.0f;

	Identities m_type = Identities::GUEST;
	State m_state = State::NORMAL;
	SearchType m_searchType = SearchType::DEADBODY;
	glm::vec2 m_searchPos = {};
	glm::vec2 m_miniSearchPos = {};
	DisguiseState m_disguiseStates[5];
	float m_suspiciousMeter = 0.0f;
	float m_searchingMeter = 0.0f;
	int m_health = 100;

	std::vector<NPCRoutePoint> m_route;
	int m_targetRouteIndex = 0;

	glm::vec2 m_frontVec{};
	
	std::vector<uint64_t> m_detectedItems;
	std::vector<uint64_t> m_detectedNPCs;

	static std::unordered_set<uint64_t> m_detectedDeadNPCs;

	float m_speed = 0.8f;

	constexpr static float m_normalSpeed = 0.8f;
	constexpr static float m_runningSpeed = 1.2f;

	constexpr static glm::vec2 m_normalScale = { 0.2f, 0.2f };
};