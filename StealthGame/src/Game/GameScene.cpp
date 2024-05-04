#include "GameScene.h"

#include <string>

#include "SceneLoader/SceneLoader.h"

void GameScene::Init(SceneInitDesc& desc)
{
	GlobalData& gData = GlobalData::Get();

	gData.m_gameScene = this;

	m_trespassZone = Zone(*desc.m_trespassingZones);
	m_hostileZone = Zone(*desc.m_hostileZones);
	
	m_visualLayers.Init(desc.m_gameTickDesc, desc.m_backgroundTexID, desc.m_foregroundTexID);
	m_taskbar.Init(desc.m_gameTickDesc);
	m_player.Init(*desc.m_player);
	m_player.BindCamera(desc.m_playerCamera);

	//for (int i = 0; i < MAXPLAYERS; i++)
	//{
	//	std::vector<QuadInitDesc> desc;
	//	glm::vec2 pos = {};
	//	uint64_t shader = gData.m_defaultShader;
	//	uint64_t texture = gData.m_texPlayer;
	//	desc.push_back({ pos, glm::vec2(0.2f, 0.2f), 0.25f, shader, gData.m_texLogo });
	//	desc.push_back({ pos, glm::vec2(0.1f), 1.0f, shader, gData.m_texPlayerCursor });
	//	desc.push_back({ pos, glm::vec2(0.2f), 0.25f, shader, texture });
	//
	//	m_otherPlayers[i].Init(desc);
	//}

	for (int i = 0; i < desc.m_npcs->size(); i++)
	{
		NPCInitDesc& npcDesc = (*desc.m_npcs)[i];

		uint64_t uuid = GameUUID{}.GetUUID();
		m_npcs[uuid] = SceneLoader::Get().MakeNPC(npcDesc.m_type);
		m_npcs[uuid]->SetNPCUUID(uuid);
		
		m_npcs[uuid]->SetType(npcDesc.m_type);
		m_npcs[uuid]->SetIsTarget(npcDesc.m_isTarget);
		m_npcs[uuid]->Init(npcDesc.m_desc, npcDesc.m_name);
		m_npcs[uuid]->BindCollision(desc.m_collision);
		m_npcs[uuid]->BindRoute(npcDesc.m_route);

		if (npcDesc.m_isTarget)
			m_targets.emplace_back(uuid);

	}

	m_map.reserve(desc.m_map->size());
	for (std::vector<QuadInitDesc>& objectDesc : *desc.m_map)
	{
		m_map.push_back(Object());
		m_map.back().Init(objectDesc, false, false);
		desc.m_collision->AddToLayer(3, m_map.back().GetUUID(0).GetUUID());
	}

	for (uint64_t& targetID : m_targets)
	{
		std::string text = "Eliminate: ";
		text.append(m_npcs[targetID]->GetName().c_str());

		m_taskbar.AddTask(TaskBar::Task(TaskBar::TaskType::ELIMINATE, text, GameUUID(targetID)));
	}
	m_taskbar.AddTask({ TaskBar::TaskType::ESCAPE, "Locate the exit", 0 });
}

void GameScene::ClearResources()
{
	m_player.ClearResources();
	*this = {};
}

void GameScene::DeleteTarget(uint64_t target)
{
	int index = -1;
	for (int i = 0; i < m_targets.size(); i++)
		if (m_targets[i] == target)
			index = i;

	if (index != -1)
	{
		m_targets.erase(m_targets.begin() + index);
		m_taskbar.CompleteTask(target);
	}
}

void GameScene::UpdateProjectiles()
{
	for (int i = 0; i < m_projectiles.size(); )
	{
		Projectile* proj = m_projectiles[i].get();
		if (!proj->Tick())
			m_projectiles.erase(m_projectiles.begin() + i);
		else
			i++;
	}
}
