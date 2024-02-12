#pragma once

#include <vector>
#include <map>
#include <memory>

#include "Items/ItemsManager.h"

#include "Characters/Player.h"
#include "Characters/NPC.h"

#include "UI/TaskBar.h"

#include "Engine/Object.h"

#include "../quadRenderer/Camera.h"

#include "Interact/Interaction.h"
#include "Interact/KillInteract.h"
#include "Interact/ItemInteract.h"

#include "Desc.h"

#include "GlobalData.h"

struct NPCInitDesc
{
	std::vector<QuadInitDesc> m_desc;
	std::vector<NPCRoutePoint> m_route;
	Identities m_type;
};

struct SceneInitDesc
{
	QuadRenderer* m_renderer;
	CollisionDetector* m_collision;

	std::vector<QuadInitDesc>* m_player;
	Camera* m_playerCamera;

	std::vector<NPCInitDesc>* m_npcs;
	std::vector<std::vector<QuadInitDesc>>* m_map;

	GameTickDesc m_gameTickDesc;
};

class GameScene
{
public:
	void Init(SceneInitDesc& desc);

	std::vector<Object>& GetMap() { return m_map; }
	std::unordered_map<uint64_t, NPC>& GetNPCs() { return m_npcs; }
	ItemsManager& GetItems() { return m_items; }
	Player& GetPlayer() { return m_player; }
	TaskBar& GetTaskbar() { return m_taskbar; }
private:
	std::vector<Object> m_map;
	//std::vector<NPC> m_npcs;
	std::unordered_map<uint64_t, NPC> m_npcs;
	ItemsManager m_items;
	Player m_player;
	TaskBar m_taskbar;
};