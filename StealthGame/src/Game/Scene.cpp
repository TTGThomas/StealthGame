#include "Scene.h"

void Scene::Init(SceneInitDesc& desc)
{
	m_player.Init(desc.m_renderer, *desc.m_player);
	m_player.BindCamera(desc.m_playerCamera);
	m_player.BindNPCs(&m_npcs);

	m_npcs.reserve(desc.m_npcs->size());
	for (NPCInitDesc& npcDesc : *desc.m_npcs)
	{
		m_npcs.push_back(NPC());
		m_npcs.back().Init(desc.m_renderer, *npcDesc.m_desc);
		m_npcs.back().BindPlayer(&m_player);
		m_npcs.back().BindCollision(desc.m_collision);
		m_npcs.back().BindRoute(*npcDesc.m_route);
	}

	m_map.reserve(desc.m_map->size());
	for (std::vector<QuadInitDesc>& objectDesc : *desc.m_map)
	{
		m_map.push_back(Object());
		m_map.back().Init(desc.m_renderer, objectDesc);
	}

	m_items.BindMap(desc.m_renderer);
}