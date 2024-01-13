#pragma once

#include <vector>
#include <memory>

#include "Items/ItemsManager.h"

#include "Characters/Player.h"
#include "Characters/NPC.h"

#include "Engine/Object.h"

#include "../quadRenderer/Camera.h"

#include "Interact/Interaction.h"
#include "Interact/KillInteract.h"
#include "Interact/ItemInteract.h"

#include "Desc.h"

struct SceneInitDesc
{
	QuadRenderer* m_renderer;
	
	std::vector<QuadInitDesc>* m_player;
	Camera* m_playerCamera;

	std::vector<std::vector<QuadInitDesc>>* m_npcs;
	std::vector<std::vector<QuadInitDesc>>* m_map;
};

class Scene
{
public:
	void Init(SceneInitDesc& desc);

	void InteractTick(GameTickDesc& desc);

	std::vector<Object>& GetMap() { return m_map; }
	std::vector<NPC>& GetNPCs() { return m_npcs; }
	ItemsManager& GetItems() { return m_items; }
	Player& GetPlayer() { return m_player; }
private:
	std::vector<Object> m_map;
	std::vector<NPC> m_npcs;
	ItemsManager m_items;
	Player m_player;
	std::shared_ptr<Interaction> m_interact;
};