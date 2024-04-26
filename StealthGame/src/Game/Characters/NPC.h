#pragma once

#include <queue>
#include <set>
#include <unordered_set>
#include <functional>
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

#undef min

class Player;
class CollisionDetector;

struct NPCCalcDynamicIn
{
	void* m_route;
	void* m_isRouteCalculated;
	void* m_shouldTerminate;
	glm::vec2 m_start;
	glm::vec2 m_location;
};

void CalculateDynamicRoute(NPCCalcDynamicIn in);

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
		NORMAL, SEARCHING, ATTACK
	};

	enum class ReportSearch
	{
		NONE, DEADBODY, ILLEGALWEAPON, GUNSHOT, PLAYER
	};

	enum class SearchType
	{
		DEADBODY, ILLEGALWEAPON, GUNSHOT
	};

	struct Bridge
	{
		std::vector<int> m_originIndexes;
		int m_destIndex;
		// npc, timeFromEnter, frameFromEnter
		std::function<bool(float, int)> m_determineFunc;
	};

	struct Node
	{
		// desc, timeFromEnter, frameFromEnter
		std::function<void(GameTickDesc&, float, int)> m_func;
		std::vector<int> m_relatedBridges;
	};
public:
	NPC() = default;
	~NPC();

	void Init(std::vector<QuadInitDesc> desc, std::string name);

	void BindCollision(CollisionDetector* collision) { m_collision = collision; }

	void BindRoute(std::vector<NPCRoutePoint>& route) { m_route = route; }

	void SetType(Identities type) { m_type = type; }

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
	void SetIsTarget(bool isTarget) { m_isTarget = isTarget; }

	static glm::vec2 GetGridPos(glm::vec2 location);

	State GetState() { return m_stateOverview; }
	GameUUID& GetNPCUUID() { return m_uuid; }
	int GetHealth() { return m_health; }
	Identities GetType() { return m_type; }
	std::string GetName() { return m_name; }
	bool GetIsBeingDragged() { return m_isBeingDragged; }
	bool GetIsDisposed() { return m_isDisposed; }
	bool GetIsTarget() { return m_isTarget; }
	float GetSpeed() { return m_speed; }
	glm::vec2 GetVelocity() { return m_velocity; }
	bool GetIsAttacking() { return m_isAttacking; }
protected:
	void SetDirPos(glm::vec2 pos);
protected:
	void ApplyDamage();
protected:
	bool IsPlayerInSight();
	void DetectPlayer();
	void DetectItems();
	void DetectNPCs();

	// O(n^2 + n)
	void DetectEverything();
protected:
	void UpdateFrontVec();
protected:
	inline bool IsThetaInView(float cosTheta);
protected:
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
protected:
	void ResetNodeGraph()
	{
		m_nodePos = 0;
		m_nodes.clear();
		m_bridges.clear();
	}

	void CompileNodeGraph();
	//void NodeGraphGuard();
	//void NodeGraphGuest();

	virtual void InitNodeGraph() {}
	void NodeGraphDead();

	void ResetTimer() { m_timeWhenEnter = 0.0f; }
protected:
	State m_stateOverview = State::NORMAL;
	GameUUID m_uuid;
	CollisionDetector* m_collision;

	std::vector<Node> m_nodes;
	std::vector<Bridge> m_bridges;
	int m_nodePos = 0;
	int m_frameFromEnter = 0;
	float m_timeWhenEnter = 0.0f;

	NPCAnimBP m_animBP;

	std::string m_name;

	bool m_isPlayerDetected = false;
	bool m_isBeingDragged = false;
	bool m_isDisposed = false;
	bool m_isAttacking = false;
	bool m_isTarget = false;

	float m_dir = 0.0f;
	float m_targetDir = 0.0f;

	bool m_isAtTarget = false;
	float m_timeAtTarget = 0.0f;

	Identities m_type = Identities::GUEST;
	DisguiseState m_disguiseStates[5];

	int m_health = 100;

	std::vector<NPCRoutePoint> m_route;
	std::vector<NPCRoutePoint> m_dynamicRoute;
	int m_dynamicTargetRouteIndex = 0;
	int m_targetRouteIndex = 0;
	bool m_isDynamicRouteCalculated = true;
	bool m_shouldTerminateCalc = false;
	std::thread m_routeThread;

	glm::vec2 m_frontVec{};
	glm::vec2 m_velocity{};
	
	std::vector<uint64_t> m_detectedItems;
	std::vector<uint64_t> m_detectedNPCs;

	static std::unordered_set<uint64_t> m_detectedDeadNPCs;

	float m_speed = 0.8f;

	constexpr static float m_normalSpeed = 0.8f;
	constexpr static float m_runningSpeed = 1.2f;

	constexpr static glm::vec2 m_normalScale = { 0.1f, 0.2f };
};