#include "GameScene.h"

void GameScene::Init(SceneInitDesc& desc)
{
	GlobalData::Get().m_gameScene = this;
	m_player.Init(*desc.m_player);
	m_player.BindCamera(desc.m_playerCamera);

	for (NPCInitDesc& npcDesc : *desc.m_npcs)
	{
		NPC npc;
		npc.Init(npcDesc.m_desc);
		npc.BindCollision(desc.m_collision);
		npc.BindRoute(npcDesc.m_route);
		npc.SetType(npcDesc.m_type);
		m_npcs[npc.GetNPCUUID().GetUUID()] = std::move(npc);
	}

	m_map.reserve(desc.m_map->size());
	for (std::vector<QuadInitDesc>& objectDesc : *desc.m_map)
	{
		m_map.push_back(Object());
		m_map.back().Init(objectDesc);
	}
}