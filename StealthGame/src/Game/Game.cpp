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
	GlobalData::Get().m_renderer = desc.m_renderer;

	desc.m_camera->SetZoom(0.5f);

	SceneLoader::Get().LoadMenu(desc, &m_gameScene, this);
}

void Game::Tick(GameTickDesc& desc)
{
	Scene* scene = GlobalData::Get().m_scene;
	GlobalData& gData = GlobalData::Get();

	gData.m_deltaTime = desc.m_tickTimer->Second();

	m_gameScene.GetTaskbar().UpdateTaskbar(desc);
	m_popUpManager.UpdatePopUps(desc);
	m_gameScene.UpdateProjectiles();

	ShowStatsWindow();
	m_gameScene.GetPlayer().ShowWindow();
	m_gameScene.GetTaskbar().ShowStatsWindow();

	m_gameScene.GetPlayer().PlayerTick(desc);
	if (m_gameScene.GetPlayer().GetHealth() <= 0)
	{
		m_gameScene.GetPlayer().SetHealth(1000);
		OnExit(0);
	}

	if (m_gameScene.GetTrespassZone().IsPointInZone(m_gameScene.GetPlayer().GetPos()))
	{
		if (m_gameScene.GetPlayer().OnTrespassZone())
			m_zonePopUp.OnTrespass();
		else
			m_zonePopUp.OnExit(desc);
	}
	else if (m_gameScene.GetHostileZone().IsPointInZone(m_gameScene.GetPlayer().GetPos()))
	{
		if (m_gameScene.GetPlayer().OnHostileZone())
			m_zonePopUp.OnHostile();
		else
			m_zonePopUp.OnExit(desc);
	}
	else
	{
		m_zonePopUp.OnExit(desc);
	}

	InteractTick(desc);

	for (auto& [uuid, npc] : m_gameScene.GetNPCs())
		npc.NPCTick(desc);

	if (m_exiting)
	{
		float time = (float)glfwGetTime() - m_exitStartTime;

		m_exitPopUp.StartEnd(desc, &m_popUpManager);

		if (time > 0.5f)
		{
			m_exiting = false;
			ClearCurrentScene(desc);
			SceneLoader::Get().LoadMap(desc, &m_gameScene, this, m_exitMap);
			m_gameScene.GetPlayer().SetInputEnabled(true);
			m_exitPopUp.StartStart(desc, &m_popUpManager);
		}
	}

	NetworkTick(desc);

	DebugManager::RenderDebugs();
}

void Game::OnResize(int width, int height)
{
	float ratio = (float)height / (float)width;
	// x * ratio = 1
	// x = 1 / ratio
	m_gameScene.GetTaskbar().SetStartPos({ -1.0f / ratio, 1.0f });
	m_zonePopUp.SetStartPos({ -1.0f / ratio, -1.0f + m_zonePopUp.GetFontSize() * 2.0f });
	m_gameScene.GetPlayer().GetInventory().OnResize(width, height);
}

void Game::OnExit(int level)
{
	m_gameScene.GetTaskbar().CompleteTask(0);
	m_gameScene.GetPlayer().SetInputEnabled(false);
	m_exiting = true;
	m_exitStartTime = (float)glfwGetTime();
	m_exitMap = level;
}

void Game::ClearCurrentScene(GameTickDesc& desc)
{
	desc.m_renderer->ClearResources();
	desc.m_scene->ClearResources();
	desc.m_collision->ClearResources();
	m_gameScene.ClearResources();
	m_zonePopUp.ClearResources();
	m_exitPopUp.ClearResources();
	m_popUpManager.ClearResources();
	AnimationPlayer::ClearResource();
}

void Game::InteractTick(GameTickDesc& desc)
{
	Scene* scene = GlobalData::Get().m_scene;

	scene->GetRenderQuads()[m_gameScene.GetPlayer().GetUUID(1).GetUUID()].SetVisibility(false);

	m_interact.reset();

	m_interactDist = FLT_MAX;


	InteractNPC();
	InteractItems();
	InteractSpecialBlocks();

	if (m_interact.get() != nullptr)
	{
		bool show = true;
		scene->GetQuads()[m_gameScene.GetPlayer().GetUUID(1).GetUUID()].SetPos(m_interact->OnTick(&show));
		if (show)
		{
			scene->GetRenderQuads()[m_gameScene.GetPlayer().GetUUID(1).GetUUID()].SetVisibility(true);
			
			if (KeyBoard::IsKeyPressDown(GLFW_KEY_E))
			{
				m_interact->OnInteract();
			}
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
			if (!GlobalData::Get().m_collision->Collide(0, m_gameScene.GetPlayer().GetPos(), npc.GetPos()).m_hasHit)
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
			if (!GlobalData::Get().m_collision->Collide(0, m_gameScene.GetPlayer().GetPos(), item->GetQuad().GetPos()).m_hasHit)
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

void Game::NetworkTick(GameTickDesc& desc)
{
	GlobalData& gData = GlobalData::Get();
	if (desc.m_network->GetRole() == NetworkManager::Role::SERVER)
	{
		std::array<std::array<unsigned char, PATCHSIZE>, MAXPLAYERS> data;

		auto& recvData = desc.m_network->GetRecvData();
		m_gameScene.GetOtherPlayers()[0].Tick(recvData[0]);

		PassData passData = m_gameScene.GetPlayer().GetPassData();
		data[1] = *(std::array<unsigned char, PATCHSIZE>*)&passData;
		data[0] = *(std::array<unsigned char, PATCHSIZE>*)&recvData[0];
		desc.m_network->ServerUpdateSendData(data);
	}
	else if (desc.m_network->GetRole() == NetworkManager::Role::CLIENT)
	{
		std::array<unsigned char, PATCHSIZE> data;

		auto& recvData = desc.m_network->GetRecvData();
		m_gameScene.GetOtherPlayers()[0].Tick(recvData[1]);

		PassData passData = m_gameScene.GetPlayer().GetPassData();
		data = *(std::array<unsigned char, PATCHSIZE>*)&passData;
		desc.m_network->ClientUpdateSendData(data);
	}
}