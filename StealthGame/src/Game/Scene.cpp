#include "Scene.h"

void Scene::Init(SceneInitDesc& desc)
{
	m_player.Init(desc.m_renderer, *desc.m_player);
	m_player.BindCamera(desc.m_playerCamera);
	m_player.BindNPCs(&m_npcs);

	m_npcs.reserve(desc.m_npcs->size());
	for (std::vector<QuadInitDesc>& npcDesc : *desc.m_npcs)
	{
		m_npcs.push_back(NPC());
		m_npcs.back().Init(desc.m_renderer, npcDesc);
	}

	m_map.reserve(desc.m_map->size());
	for (std::vector<QuadInitDesc>& objectDesc : *desc.m_map)
	{
		m_map.push_back(Object());
		m_map.back().Init(desc.m_renderer, objectDesc);
	}

	m_items.BindMap(desc.m_renderer);
}

void Scene::InteractTick(GameTickDesc& desc)
{
	m_interact.reset();

	// get possible interactive NPC
	desc.m_renderer->GetQuads()[m_player.GetIndex(1)].SetVisibility(false);
	for (NPC& npc : m_npcs)
	{
		glm::vec2 diff = npc.GetPos() - m_player.GetPos();
		if (glm::length(diff) < 0.5f)
		{
			m_interact = std::make_shared<KillInteract>(&m_player, &npc);
			break;
		}
	}

	// get possible interactive items
	Item* item = m_items.GetNearestItem(m_player.GetPos()).get();
	if (item != nullptr)
	{
		if (glm::distance(item->GetQuad().GetPos(), m_player.GetPos()) < 0.5f)
		{
			m_interact.reset();
			m_interact = std::make_shared<ItemInteract>(this, item);
		}
	}
	
	if (m_interact.get() != nullptr)
	{
		desc.m_renderer->GetQuads()[m_player.GetIndex(1)].SetPos(m_interact->OnTick());
		desc.m_renderer->GetQuads()[m_player.GetIndex(1)].SetVisibility(true);
		if (KeyBoard::IsKeyPressDown(GLFW_KEY_E))
		{
			m_interact->OnInteract();
		}
	}
}