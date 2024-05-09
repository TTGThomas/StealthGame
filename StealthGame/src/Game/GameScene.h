#pragma once

#include <vector>
#include <unordered_map>
#include <set>
#include <memory>

#include "Items/ItemsManager.h"

#include "Characters/Player.h"
#include "Characters/OtherPlayer.h"
#include "Characters/NPC.h"

#include "UI/TaskBar.h"

#include "Engine/Object.h"
#include "Engine/Projectile.h"

#include "../quadRenderer/Camera.h"

#include "Interact/Interaction.h"
#include "Interact/NPCInteract.h"
#include "Interact/ItemInteract.h"

#include "SpecialBlock/SpecialBlockManager.h"

#include "UI/PopUps/PopUpManager.h"

#include "Zones/Zone.h"

#include "VisualLayers.h"
#include "Desc.h"
#include "PassData.h"
#include "GlobalData.h"

struct NPCInitDesc
{
	std::vector<QuadInitDesc> m_desc;
	std::vector<NPCRoutePoint> m_route;
	Identities m_type;
	std::string m_name;
	bool m_isTarget = false;
};

struct SceneInitDesc
{
	QuadRenderer* m_renderer;
	CollisionDetector* m_collision;

	std::vector<QuadInitDesc>* m_player;
	Camera* m_playerCamera;

	std::vector<NPCInitDesc>* m_npcs;
	std::vector<std::vector<QuadInitDesc>>* m_map;

	std::vector<AABB>* m_trespassingZones;
	std::vector<AABB>* m_hostileZones;

	GameTickDesc m_gameTickDesc;
};

class GameScene
{
public:
	void Init(SceneInitDesc& desc);

	void ClearResources();

	void DeleteTarget(uint64_t target);

	void UpdateProjectiles();

	std::vector<Object>& GetMap() { return m_map; }
	const std::vector<uint64_t>& GetTargets() { return m_targets; }
	std::unordered_map<uint64_t, std::unique_ptr<NPC>>& GetNPCs() { return m_npcs; }
	ItemsManager& GetItems() { return m_items; }
	Player& GetPlayer() { return m_player; }
	std::array<OtherPlayer, MAXPLAYERS>& GetOtherPlayers() { return m_otherPlayers; }
	TaskBar& GetTaskbar() { return m_taskbar; }
	Zone& GetTrespassZone() { return m_trespassZone; }
	Zone& GetHostileZone() { return m_hostileZone; }
	SpecialBlockManager& GetSpecialBlockManager() { return m_specialBlockManager; }
	std::vector<std::unique_ptr<Projectile>>& GetProjectiles() { return m_projectiles; }
private:
	VisualLayers m_visualLayers;
	std::vector<Object> m_map;
	std::vector<std::unique_ptr<Projectile>> m_projectiles;
	std::unordered_map<uint64_t, std::unique_ptr<NPC>> m_npcs;
	std::vector<uint64_t> m_targets;
	ItemsManager m_items;
	SpecialBlockManager m_specialBlockManager;
	Player m_player;
	std::array<OtherPlayer, MAXPLAYERS> m_otherPlayers;
	TaskBar m_taskbar;

	Zone m_trespassZone;
	Zone m_hostileZone;
};