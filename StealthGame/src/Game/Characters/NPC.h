#pragma once

#include <queue>
#include <set>
#include <unordered_set>
#include <vector>
#include <string>
#include <thread>

#include <glm/glm.hpp>

#include "../../input/KeyBoard.h"
#include "../../input/Mouse.h"

#include "../Engine/Entity.h"

#include "../Animation/AnimationPlayer.h"
#include "AnimBP/NPCAnimBP.h"

#include "../Desc.h"

#include "../Items/Item.h"

#include "../GlobalData.h"

#include "../DebugManager.h"

class Player;
class CollisionDetector;

void CalculateDynamicRoute(GlobalData* gData, void* route, void* isRouteCalculated, glm::vec2 start, glm::vec2 location);

struct NPCRoutePoint
{
	NPCRoutePoint(glm::vec2 pos, int waitMs = 0)
		: m_pos(pos), m_waitMs(waitMs) {}

	glm::vec2 m_pos;
	int m_waitMs = 0;
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

	void Init(std::vector<QuadInitDesc> desc, std::string name);

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
	void SetNPCUUID(GameUUID uuid) { m_uuid = uuid; }

	void SetSearchType(SearchType type) { m_searchType = type; }

	static glm::vec2 GetGridPos(glm::vec2 location);

	float GetSuspiciousMeter() { return m_suspiciousMeter; }
	State GetState() { return m_state; }
	GameUUID& GetNPCUUID() { return m_uuid; }
	int GetHealth() { return m_health; }
	Identities GetType() { return m_type; }
	std::string GetName() { return m_name; }
	bool GetIsBeingDragged() { return m_isBeingDragged; }
	bool GetIsDisposed() { return m_isDisposed; }
	float GetSpeed() { return m_speed; }
	SearchType GetSearchType() { return m_searchType; }
	glm::vec2 GetVelocity() { return m_velocity; }
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
	void Search(void* param, glm::vec2 where, float time, SearchType type)
	{
		m_state = State::SEARCHING;
		m_searchingMeter = time;
		m_searchPos = where;
		m_miniSearchPos = m_searchPos;
		m_searchType = type;
		m_seachParam = param;
		m_onSeachEnter = true;
		m_isLocationNew = true;
	}
private:
	// returns if it is at point or not
	// this does not applies BFS
	bool MoveToTarget(float dt, glm::vec2 point, bool snapp = true);
	// returns if it is at location or not
	// this doesnt calculate the route
	bool MoveToLocation(float dt);
	void StartMoveToLocation(glm::vec2 location);
	void PointAtPoint(glm::vec2 point);
	float AngleFromPoint(glm::vec2 point);
	void NPCMove(glm::vec2 vec);
private:
	GameUUID m_uuid;
	CollisionDetector* m_collision;

	NPCAnimBP m_animBP;

	std::string m_name;

	bool m_isPlayerDetected = false;

	bool m_isBeingDragged = false;
	bool m_isDisposed = false;

	bool m_isWitness = false;

	float m_dir = 0.0f;
	float m_targetDir = 0.0f;

	bool m_isAtTarget = false;
	float m_timeAtTarget = 0.0f;

	Identities m_type = Identities::GUEST;
	State m_state = State::NORMAL;
	SearchType m_searchType = SearchType::DEADBODY;
	glm::vec2 m_searchPos = {};
	glm::vec2 m_miniSearchPos = {};
	void* m_seachParam = nullptr;// can be anything
	float m_miniSearchingMeter = 2.0f;
	bool m_isLocationNew = true;
	bool m_onSeachEnter = true;

	DisguiseState m_disguiseStates[5];
	float m_suspiciousMeter = 0.0f;
	float m_searchingMeter = 0.0f;
	int m_health = 100;

	std::vector<NPCRoutePoint> m_route;
	std::vector<NPCRoutePoint> m_dynamicRoute;
	int m_dynamicTargetRouteIndex = 0;
	int m_targetRouteIndex = 0;
	bool m_isDynamicRouteCalculated = false;

	glm::vec2 m_frontVec{};
	glm::vec2 m_velocity{};
	
	std::vector<uint64_t> m_detectedItems;
	std::vector<uint64_t> m_detectedNPCs;

	static std::unordered_set<uint64_t> m_detectedDeadNPCs;

	float m_speed = 0.8f;

	constexpr static float m_normalSpeed = 0.8f;
	constexpr static float m_runningSpeed = 1.2f;

	constexpr static glm::vec2 m_normalScale = { 0.2f, 0.2f };
};