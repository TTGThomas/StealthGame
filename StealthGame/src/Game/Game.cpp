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
	GlobalData::Get().m_camera = desc.m_camera;

	int count = 0;
	std::for_each(m_gameStates.begin(), m_gameStates.end(), [&](GameStateNode& x) { x.m_state = (GameState)count++; });

	m_gameStates[0].m_bridges = { 1 };
	m_gameStates[1].m_bridges = { 2 };
	m_gameStates[2].m_bridges = { 3, 1 };
	m_gameStates[3].m_bridges = { 2, 1 };

	desc.m_camera->SetZoom(0.5f);

	LoadStart(desc);
}

void Game::Tick(GameTickDesc& desc)
{
	if (m_gameStates[m_gameState].m_state == GameState::START)
		StartTick(desc);
	else if (m_gameStates[m_gameState].m_state == GameState::MENU)
		MenuTick(desc);
	else if (m_gameStates[m_gameState].m_state == GameState::GAME)
		GameTick(desc);
	else if (m_gameStates[m_gameState].m_state == GameState::PAUSE)
		PauseTick(desc);
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

void Game::OnExit()
{
	m_gameScene.GetTaskbar().CompleteTask(0);
	m_gameScene.GetPlayer().SetInputEnabled(false);
	m_exiting = true;
	m_exitStartTime = (float)glfwGetTime();
}

void Game::ClearCurrentScene(GameTickDesc& desc)
{
	desc.m_renderer->ClearResources();
	desc.m_scene->ClearResources();
	desc.m_collision->ClearResources();
	m_gameScene.ClearResources();
	m_zonePopUp.ClearResources();
	//m_exitPopUp.ClearResources();
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
		glm::vec2 diff = npc->GetPos() - m_gameScene.GetPlayer().GetPos();
		float dst = glm::length(diff); // pythagoras
		//float dst = glm::abs(diff.x) + glm::abs(diff.y); // manhattem
		if (dst < 0.5f && !npc->IsPlayerDetected() && (dst < victimDst || victimDst == -1.0f) && !npc->GetIsBeingDragged())
		{
			if (!GlobalData::Get().m_collision->Collide(0, m_gameScene.GetPlayer().GetPos(), npc->GetPos()).m_hasHit)
			{
				victim = npc.get();
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

#ifndef IMGUI_DISABLE
	ImGui::Begin("Game");
	ImGui::Text("Bodies found: %i", gData.m_bodiesFound);
	ImGui::End();
#endif
}

void Game::NetworkTick(GameTickDesc& desc)
{
	if (!m_isOnline)
		return;

	GlobalData& gData = GlobalData::Get();
	if (desc.m_network->GetRole() == NetworkManager::Role::SERVER)
	{
		std::array<std::array<unsigned char, PATCHSIZE>, MAXPLAYERS> data;

		auto& recvData = desc.m_network->GetRecvData();
		for (int i = 0; i < MAXPLAYERS; i++)
			if (i != MAXPLAYERS - 1)
				m_gameScene.GetOtherPlayers()[i].Tick(recvData[i]);

		PassData passData = m_gameScene.GetPlayer().GetPassData();
		for (int i = 0; i < MAXPLAYERS - 1; i++)
			data[i] = *(std::array<unsigned char, PATCHSIZE>*)&recvData[i];
		data[MAXPLAYERS - 1] = *(std::array<unsigned char, PATCHSIZE>*)&passData;
		desc.m_network->ServerUpdateSendData(data);
	}
	else if (desc.m_network->GetRole() == NetworkManager::Role::CLIENT)
	{
		std::array<unsigned char, PATCHSIZE> data;

		auto& recvData = desc.m_network->GetRecvData();
		for (int i = 0; i < MAXPLAYERS; i++)
			if (i != desc.m_network->ClientGetIndex())
				m_gameScene.GetOtherPlayers()[i].Tick(recvData[i]);

		PassData passData = m_gameScene.GetPlayer().GetPassData();
		data = *(std::array<unsigned char, PATCHSIZE>*)&passData;
		desc.m_network->ClientUpdateSendData(data);
	}
}

void Game::GameTick(GameTickDesc& desc)
{
	Scene* scene = GlobalData::Get().m_scene;
	GlobalData& gData = GlobalData::Get();
	
	if (m_onEnter)
	{
		float alpha = desc.m_scene->GetRenderQuads()[m_menuUUIDs[0]].GetAlpha();
		alpha -= desc.m_tickTimer->Second();
		if (alpha < 0.0f)
			m_onEnter = false;
		desc.m_scene->GetRenderQuads()[m_menuUUIDs[0]].SetAlpha(alpha);
	}

	gData.m_deltaTime = desc.m_tickTimer->Second();

	m_gameScene.GetTaskbar().UpdateTaskbar(desc);
	m_popUpManager.UpdatePopUps(desc);

	ShowStatsWindow();
	m_gameScene.GetPlayer().ShowWindow();
	m_gameScene.GetTaskbar().ShowStatsWindow();

	m_gameScene.GetPlayer().PlayerTick(desc);
	if (m_gameScene.GetPlayer().GetHealth() <= 0)
	{
		m_gameScene.GetPlayer().SetHealth(1000);
		OnExit();
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

	if (m_gameScene.GetPlayer().GetVelocity() != glm::vec2(0.0f) || true)
	{
		for (auto& [uuid, npc] : m_gameScene.GetNPCs())
			npc->NPCTick(desc);
		m_gameScene.UpdateProjectiles();
	}

	if (m_exiting)
	{
		m_onEnter = false;
		m_exitState = true;
		m_exiting = false;
	}

	if (m_exitState)
	{
		float alpha = desc.m_scene->GetRenderQuads()[m_menuUUIDs[0]].GetAlpha();
		alpha += desc.m_tickTimer->Second();
		if (alpha > 1.0f)
		{
			m_exitState = false;
			SwitchState(desc, 1);
			return;
		}
		desc.m_scene->GetRenderQuads()[m_menuUUIDs[0]].SetAlpha(alpha);
	}

	if (KeyBoard::IsKeyPressDown(GLFW_KEY_ESCAPE))
		RawSwitchState(desc, 0);

	NetworkTick(desc);

	DebugManager::RenderDebugs();
}

void Game::SwitchState(GameTickDesc& desc, int bridgeIndex)
{
	m_menuUUIDs = {};
	m_textures = {};
	ClearCurrentScene(desc);
	m_gameState = m_gameStates[m_gameState].m_bridges[bridgeIndex];
	m_onEnter = true;
	if (m_gameState == 0)
		LoadStart(desc);
	else if (m_gameState == 1)
		LoadMenu(desc);
	else if (m_gameState == 2)
		LoadGame(desc);
	else if (m_gameState == 3)
		LoadPause(desc);
}

void Game::RawSwitchState(GameTickDesc& desc, int bridgeIndex)
{
	m_gameState = m_gameStates[m_gameState].m_bridges[bridgeIndex];
	m_onEnter = true;
	if (m_gameState == 0)
		LoadStart(desc);
	else if (m_gameState == 1)
		LoadMenu(desc);
	else if (m_gameState == 2)
		LoadGame(desc);
	else if (m_gameState == 3)
		LoadPause(desc);
}

void Game::LoadStart(GameTickDesc& desc)
{
	GlobalData& gData = GlobalData::Get();
	SceneLoader::Get().LoadRawConstants(desc);

	desc.m_renderer->SetClear({ 0.0f, 0.0f, 0.0f, 1.0f });
	m_textures.emplace_back(desc.m_renderer->AddTexture("res/start.png"));
	
	{
		Quad quad;
		quad.SetPos({});
		quad.SetRadius({ 1.0f, 1.0f });
		m_menuUUIDs.emplace_back(quad.GetUUID().GetUUID());
		RenderQuadInitDesc renderDesc;
		renderDesc.m_depth = 0.0f;
		renderDesc.m_followCameraOffset = false;
		renderDesc.m_shaderUUID = gData.m_defaultShader;
		renderDesc.m_textureUUID = m_textures[0];
		desc.m_scene->AddQuad(quad, renderDesc);
	}
	{
		Quad quad;
		quad.SetPos({});
		quad.SetRadius({ 1000.0f, 1.0f });
		m_menuUUIDs.emplace_back(quad.GetUUID().GetUUID());
		RenderQuadInitDesc renderDesc;
		renderDesc.m_depth = 1.0f;
		renderDesc.m_followCameraOffset = false;
		renderDesc.m_shaderUUID = gData.m_defaultShader;
		renderDesc.m_textureUUID = gData.m_texBlack;
		desc.m_scene->AddQuad(quad, renderDesc);
		desc.m_scene->GetRenderQuads()[m_menuUUIDs.back()].SetAlpha(1.0f);
	}

}

void Game::LoadMenu(GameTickDesc& desc)
{
	GlobalData& gData = GlobalData::Get();
	SceneLoader::Get().LoadRawConstants(desc);

	{
		Quad quad;
		quad.SetPos({});
		quad.SetRadius({ 0.5f, 0.5f });
		m_menuUUIDs.emplace_back(quad.GetUUID().GetUUID());
		RenderQuadInitDesc renderDesc;
		renderDesc.m_depth = 0.0f;
		renderDesc.m_followCameraOffset = false;
		renderDesc.m_shaderUUID = gData.m_defaultShader;
		renderDesc.m_textureUUID = gData.m_texLogo;
		desc.m_scene->AddQuad(quad, renderDesc);
	}
	{
		Quad quad;
		quad.SetPos({});
		quad.SetRadius({ 1000.0f, 1.0f });
		m_menuUUIDs.emplace_back(quad.GetUUID().GetUUID());
		RenderQuadInitDesc renderDesc;
		renderDesc.m_depth = 1.0f;
		renderDesc.m_followCameraOffset = false;
		renderDesc.m_shaderUUID = gData.m_defaultShader;
		renderDesc.m_textureUUID = gData.m_texBlack;
		desc.m_scene->AddQuad(quad, renderDesc);
		desc.m_scene->GetRenderQuads()[m_menuUUIDs.back()].SetAlpha(1.0f);
	}
}

void Game::LoadGame(GameTickDesc& desc)
{
	GlobalData& gData = GlobalData::Get();
	SceneLoader::Get().LoadMap(desc, &m_gameScene, this, m_enterMap);
	{
		Quad quad;
		quad.SetPos({});
		quad.SetRadius({ 1000.0f, 1.0f });
		m_menuUUIDs.emplace_back(quad.GetUUID().GetUUID());
		RenderQuadInitDesc renderDesc;
		renderDesc.m_depth = 1.0f;
		renderDesc.m_followCameraOffset = false;
		renderDesc.m_shaderUUID = gData.m_defaultShader;
		renderDesc.m_textureUUID = gData.m_texBlack;
		desc.m_scene->AddQuad(quad, renderDesc);
		desc.m_scene->GetRenderQuads()[m_menuUUIDs.back()].SetAlpha(1.0f);
	}
}

void Game::LoadPause(GameTickDesc& desc)
{
	GlobalData& gData = GlobalData::Get();

	{
		Quad quad;
		quad.SetPos({});
		quad.SetRadius({ 1.0f, 1.0f });
		m_menuUUIDs.emplace_back(quad.GetUUID().GetUUID());
		RenderQuadInitDesc renderDesc;
		renderDesc.m_depth = 1.0f;
		renderDesc.m_followCameraOffset = false;
		renderDesc.m_shaderUUID = gData.m_defaultShader;
		renderDesc.m_textureUUID = gData.m_texDoor;
		desc.m_scene->AddQuad(quad, renderDesc);
		desc.m_scene->GetRenderQuads()[m_menuUUIDs.back()].SetAlpha(0.1f);
	}
}


void Game::StartTick(GameTickDesc& desc)
{
	if (m_onEnter)
	{
		float alpha = desc.m_scene->GetRenderQuads()[m_menuUUIDs[1]].GetAlpha();
		alpha -= desc.m_tickTimer->Second();
		if (alpha < 0.0f)
			m_onEnter = false;
		desc.m_scene->GetRenderQuads()[m_menuUUIDs[1]].SetAlpha(alpha);
	}

	if (KeyBoard::IsKeyPressDown(GLFW_KEY_ENTER))
	{
		m_onEnter = false;
		m_exitState = true;
	}

	if (m_exitState)
	{
		float alpha = desc.m_scene->GetRenderQuads()[m_menuUUIDs[1]].GetAlpha();
		alpha += desc.m_tickTimer->Second();
		if (alpha > 1.0f)
		{
			m_exitState = false;
			SwitchState(desc, 0);
		}
		desc.m_scene->GetRenderQuads()[m_menuUUIDs[1]].SetAlpha(alpha);
	}
}

void Game::MenuTick(GameTickDesc& desc)
{
	if (m_onEnter)
	{
		float alpha = desc.m_scene->GetRenderQuads()[m_menuUUIDs.back()].GetAlpha();
		alpha -= desc.m_tickTimer->Second();
		if (alpha < 0.0f)
			m_onEnter = false;
		desc.m_scene->GetRenderQuads()[m_menuUUIDs.back()].SetAlpha(alpha);
	}

	if (QuadClicked(desc, m_menuUUIDs[0]))
	{
		m_enterMap = 0;
		m_onEnter = false;
		m_exitState = true;
	}

	if (m_exitState)
	{
		float alpha = desc.m_scene->GetRenderQuads()[m_menuUUIDs.back()].GetAlpha();
		alpha += desc.m_tickTimer->Second();
		if (alpha > 1.0f)
		{
			m_exitState = false;
			SwitchState(desc, 0);
			return;
		}
		desc.m_scene->GetRenderQuads()[m_menuUUIDs.back()].SetAlpha(alpha);
	}
}

void Game::PauseTick(GameTickDesc& desc)
{
	if (KeyBoard::IsKeyPressDown(GLFW_KEY_ESCAPE))
	{
		// Manual clear objects
		desc.m_scene->DeleteQuad(m_menuUUIDs.back());

		m_menuUUIDs.erase(m_menuUUIDs.end() - 1);
		m_gameState = m_gameStates[m_gameState].m_bridges[0];
		m_onEnter = false;
	}
}

bool Game::QuadClicked(GameTickDesc& desc, uint64_t uuid)
{
	Quad& quad = desc.m_scene->GetQuads()[uuid];
	AABB& aabb = desc.m_scene->GetAABBs()[uuid];
	aabb.SetMinPos(quad.GetPos() - quad.GetRadius());
	aabb.SetMaxPos(quad.GetPos() + quad.GetRadius());

	glm::vec2 windowScale = { desc.m_window->GetWidth() , desc.m_window->GetHeight() };
	glm::vec2 mousePos = { Mouse::GetMouseX(), Mouse::GetMouseY() };
	mousePos = mousePos / windowScale * 2.0f - 1.0f;
	mousePos.x *= 1.0f / (windowScale.y / windowScale.x);
	mousePos.y = -mousePos.y;

	if (!Mouse::IsMousePressDown(GLFW_MOUSE_BUTTON_LEFT))
		return false;

	if (mousePos.x > aabb.GetMinPos().x && mousePos.x < aabb.GetMaxPos().x)
		if (mousePos.y > aabb.GetMinPos().y && mousePos.y < aabb.GetMaxPos().y)
			return true;
	return false;
}
