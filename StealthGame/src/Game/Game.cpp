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
	GlobalData::Get().m_collision = desc.m_collision;

	desc.m_camera->SetZoom(0.5f);

	SceneLoader::Get().LoadDebugScene(desc, &m_gameScene);

	m_zonePopUp.Init(desc, &m_popUpManager);
}

void Game::Tick(GameTickDesc& desc)
{
	Scene* scene = GlobalData::Get().m_scene;
	GlobalData& gData = GlobalData::Get();

	m_gameScene.GetTaskbar().UpdateTaskbar(desc);
	m_popUpManager.UpdatePopUps(desc);

	ShowStatsWindow();
	m_gameScene.GetTaskbar().ShowStatsWindow();

	for (auto& [uuid, npc] : m_gameScene.GetNPCs())
		scene->GetAABBs()[npc.GetUUID(0).GetUUID()].SetEnabled(false);

	m_gameScene.GetPlayer().PlayerTick(desc);

	if (m_gameScene.GetTrespassZone().IsPointInZone(m_gameScene.GetPlayer().GetPos()))
	{
		m_gameScene.GetPlayer().OnTrespassZone();
		m_zonePopUp.OnTrespass();
	}
	else if (m_gameScene.GetHostileZone().IsPointInZone(m_gameScene.GetPlayer().GetPos()))
	{
		m_gameScene.GetPlayer().OnHostileZone();
		m_zonePopUp.OnHostile();
	}
	else
	{
		m_zonePopUp.OnExit(desc);
	}

	InteractTick(desc);

	if (glm::length(m_gameScene.GetPlayer().GetVelocity()) != 0.0f || true)
		for (auto& [uuid, npc] : m_gameScene.GetNPCs())
			npc.NPCTick(desc);
}

void Game::OnResize(int width, int height)
{
	float ratio = (float)height / (float)width;
	// x * ratio = 1
	// x = 1 / ratio
	m_gameScene.GetTaskbar().SetStartPos({ -1.0f / ratio, 1.0f });
	m_zonePopUp.SetStartPos({ -1.0f / ratio, -1.0f + m_zonePopUp.GetFontSize() * 2.0f });
}

void Game::InteractTick(GameTickDesc& desc)
{
	Scene* scene = GlobalData::Get().m_scene;

	scene->GetRenderQuads()[m_gameScene.GetPlayer().GetUUID(1).GetUUID()].SetVisibility(false);

	m_interact.reset();

	GlobalData::Get().m_scene->GetAABBs()[m_gameScene.GetPlayer().GetUUID(0).GetUUID()].SetEnabled(false);

	m_interactDist = FLT_MAX;

	InteractNPC();
	InteractItems();
	InteractSpecialBlocks();

	GlobalData::Get().m_scene->GetAABBs()[m_gameScene.GetPlayer().GetUUID(0).GetUUID()].SetEnabled(true);

	if (m_interact.get() != nullptr)
	{
		bool showCursor = true;
		scene->GetQuads()[m_gameScene.GetPlayer().GetUUID(1).GetUUID()].SetPos(m_interact->OnTick(&showCursor));
		if (showCursor)
			scene->GetRenderQuads()[m_gameScene.GetPlayer().GetUUID(1).GetUUID()].SetVisibility(true);

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
	if (m_gameScene.GetPlayer().GetIsDragging())
		return;

	NPC* victim;
	float victimDst = -1.0f;
	for (auto& [uuid, npc] : m_gameScene.GetNPCs())
	{
		glm::vec2 diff = npc.GetPos() - m_gameScene.GetPlayer().GetPos();
		float dst = glm::length(diff); // pythagoras
		//float dst = glm::abs(diff.x) + glm::abs(diff.y); // manhattem
		if (dst < 0.5f && !npc.IsPlayerDetected() && (dst < victimDst || victimDst == -1.0f) && !npc.GetIsBeingDragged())
		{
			if (!GlobalData::Get().m_collision->Collide(m_gameScene.GetPlayer().GetPos(), npc.GetPos()).m_hasHit)
			{
				victim = &npc;
				victimDst = glm::length(diff);
			}
		}
	}

	if (victimDst != -1.0f && victimDst < m_interactDist)
	{
		m_interact.reset();
		m_interact = std::make_shared<NPCInteract>(&m_gameScene, victim);
		m_interactDist = victimDst;
	}
}

void Game::InteractItems()
{
	// get possible interactive items
	Item* item = m_gameScene.GetItems().GetNearestItem(m_gameScene.GetPlayer().GetPos()).get();
	if (item != nullptr)
	{
		glm::vec2 diff = item->GetQuad().GetPos() - m_gameScene.GetPlayer().GetPos();
		float dist = glm::length(diff); // pythagoras
		//float dist = glm::abs(diff.x) + glm::abs(diff.y); // manhattem
		if (dist < 0.5f)
		{
			if (!GlobalData::Get().m_collision->Collide(m_gameScene.GetPlayer().GetPos(), item->GetQuad().GetPos()).m_hasHit)
			{
				if (dist < m_interactDist)
				{
					m_interact.reset();
					m_interact = std::make_shared<ItemInteract>(&m_gameScene, item);
					m_interactDist = dist;
				}
			}
		}
	}
}

void Game::InteractSpecialBlocks()
{
	float dist = 0.0f;
	std::shared_ptr<Interaction> interact = m_gameScene.GetSpecialBlockManager().GetClosestEventWithinRange(m_gameScene.GetPlayer().GetPos(), 0.6f, &dist);
	if (interact != nullptr)
	{
		if (dist < m_interactDist)
		{
			m_interact.reset();
			m_interact = interact;
			m_interactDist = dist;
		}
	}
}

void Game::ShowStatsWindow()
{
	GlobalData& gData = GlobalData::Get();

	ImGui::Begin("Game");
	ImGui::Text("Bodies found: %i", gData.m_bodiesFound);
	ImGui::End();
}