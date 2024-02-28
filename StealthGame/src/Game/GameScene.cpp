#include "GameScene.h"

#include <string>

void GameScene::Init(SceneInitDesc& desc)
{
	GlobalData::Get().m_gameScene = this;

	m_trespassZone = Zone(*desc.m_trespassingZones);
	m_hostileZone = Zone(*desc.m_hostileZones);
	
	m_visualLayers.Init(desc.m_gameTickDesc, desc.m_backgroundTexID, desc.m_foregroundTexID);
	m_taskbar.Init(desc.m_gameTickDesc);
	m_player.Init(*desc.m_player);
	m_player.BindCamera(desc.m_playerCamera);

	for (int i = 0; i < desc.m_npcs->size(); i++)
	{
		NPCInitDesc& npcDesc = (*desc.m_npcs)[i];

		NPC npc;
		npc.Init(npcDesc.m_desc, npcDesc.m_name);
		npc.BindCollision(desc.m_collision);
		npc.BindRoute(npcDesc.m_route);
		npc.SetType(npcDesc.m_type);

		if (npcDesc.m_isTarget)
			m_targets.emplace_back(npc.GetNPCUUID().GetUUID());

		m_npcs[npc.GetNPCUUID().GetUUID()] = std::move(npc);
	}

	m_map.reserve(desc.m_map->size());
	for (std::vector<QuadInitDesc>& objectDesc : *desc.m_map)
	{
		m_map.push_back(Object());
		m_map.back().Init(objectDesc);
	}

	for (uint64_t& targetID : m_targets)
	{
		const char* prefix = "Eliminate ";
		const int prefixLen = (int)strlen(prefix);

		const char* name = m_npcs[targetID].GetName();
		const int nameLen = (int)strlen(name);

		char* text = new char[nameLen + prefixLen + 1];
		text[nameLen + prefixLen] = '\0';

		for (int i = 0; i < nameLen + prefixLen; i++)
			text[i] = (i < 10 ? prefix[i] : name[i - 10]);

		m_taskbar.AddTask({ TaskBar::TaskType::ELIMINATE, text, UUID(targetID) });
		delete[] text;
	}
	m_taskbar.AddTask({ TaskBar::TaskType::ESCAPE, "Locate the exit", 0 });
}

void GameScene::ClearResources()
{
	*this = {};
	m_player.ClearResources();
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