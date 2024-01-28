#include "Game.h"

Game::Game()
{
}

Game::~Game()
{
}

void Game::Init(GameTickDesc& desc)
{
	GlobalData::Get().m_scene = desc.m_scene;

	desc.m_camera->SetZoom(0.5f);

	SceneLoader::Get().LoadDebugScene(desc, &m_gameScene);
}

void Game::Tick(GameTickDesc& desc)
{
	Scene* scene = GlobalData::Get().m_scene;

	for (auto& [uuid, npc] : m_gameScene.GetNPCs())
		scene->GetAABBs()[npc.GetUUID(0).GetUUID()].SetEnabled(false);
	m_gameScene.GetPlayer().PlayerTick(desc);

	InteractTick(desc);

	if (glm::length(m_gameScene.GetPlayer().GetVelocity()) != 0.0f || true)
		for (auto& [uuid, npc] : m_gameScene.GetNPCs())
			npc.NPCTick(desc);
}

void Game::InteractTick(GameTickDesc& desc)
{
	Scene* scene = GlobalData::Get().m_scene;

	m_interact.reset();
	InteractNPC();
	InteracatItems();

	if (m_interact.get() != nullptr)
	{
		scene->GetRenderQuads()[m_gameScene.GetPlayer().GetUUID(1).GetUUID()].SetVisibility(true);
		scene->GetQuads()[m_gameScene.GetPlayer().GetUUID(1).GetUUID()].SetPos(m_interact->OnTick());

		if (KeyBoard::IsKeyPressDown(GLFW_KEY_E))
		{
			m_interact->OnInteract();
		}
	}
}

void Game::InteractNPC()
{
	Scene* scene = GlobalData::Get().m_scene;

	// get possible interactive NPC
	scene->GetRenderQuads()[m_gameScene.GetPlayer().GetUUID(1).GetUUID()].SetVisibility(false);
	NPC* victim;
	float victimDst = -1.0f;
	for (auto& [uuid, npc] : m_gameScene.GetNPCs())
	{
		glm::vec2 diff = npc.GetPos() - m_gameScene.GetPlayer().GetPos();
		float dst = glm::length(diff);
		if (dst < 0.5f && !npc.IsPlayerDetected() && (dst < victimDst || victimDst == -1.0f))
		{
			victim = &npc;
			victimDst = glm::length(diff);
		}
	}

	if (victimDst != -1.0f)
	{
		m_interact.reset();
		m_interact = std::make_shared<KillInteract>(&m_gameScene.GetPlayer(), victim);
	}
}

void Game::InteracatItems()
{
	// get possible interactive items
	Item* item = m_gameScene.GetItems().GetNearestItem(m_gameScene.GetPlayer().GetPos()).get();
	if (item != nullptr)
	{
		if (glm::distance(item->GetQuad().GetPos(), m_gameScene.GetPlayer().GetPos()) < 0.5f)
		{
			m_interact.reset();
			m_interact = std::make_shared<ItemInteract>(&m_gameScene, item);
		}
	}
}