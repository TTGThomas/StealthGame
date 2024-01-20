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

	Shader shader("shaders/default/Vertex.glsl", "shaders/default/Fragment.glsl");
	GlobalData::Get().m_defaultShader = shader.GetUUID().GetUUID();
	desc.m_renderer->AddShader(shader);
	
	Texture logo("res/logo.png");
	GlobalData::Get().m_texLogo = logo.GetUUID().GetUUID();
	desc.m_renderer->AddTexture(logo);

	Texture cursor("res/Player/Cursor.png");
	GlobalData::Get().m_texPlayerCursor = cursor.GetUUID().GetUUID();
	desc.m_renderer->AddTexture(cursor);

	Texture player("res/Player/Player.png");
	GlobalData::Get().m_texPlayer = player.GetUUID().GetUUID();
	desc.m_renderer->AddTexture(player);

	Texture npc0("res/NPC/NPC0.png");
	GlobalData::Get().m_texNPC0 = npc0.GetUUID().GetUUID();
	desc.m_renderer->AddTexture(npc0);

	Texture npcDir("res/NPC/Dir.png");
	GlobalData::Get().m_texNPCDir = npcDir.GetUUID().GetUUID();
	desc.m_renderer->AddTexture(npcDir);

	// layer 0 = objLayer
	// layer 1 = UILayer

	GlobalData& globalData = GlobalData::Get();

	std::vector<QuadInitDesc> playerDesc{};
	playerDesc.push_back({ glm::vec2(0.0f, 0.0f), glm::vec2(0.2f), -0.002f, globalData.m_defaultShader, globalData.m_texLogo});
	playerDesc.push_back({ glm::vec2(0.0f, 0.0f), glm::vec2(0.1f), -1.0f, globalData.m_defaultShader, globalData.m_texPlayerCursor});
	playerDesc.push_back({ glm::vec2(0.0f, 0.0f), glm::vec2(0.2f), -0.002f, globalData.m_defaultShader, globalData.m_texPlayer});

	std::vector<NPCInitDesc> allNpcDesc{};

	std::vector<QuadInitDesc> npcQuad0;
	std::vector<NPCRoutePoint> m_npcRoute0;
	{
		NPCInitDesc npcDesc{};
		glm::vec2 npcPos = glm::vec2(-2.0f, 0.0f);
		npcQuad0.push_back({ npcPos, glm::vec2(0.2f), -0.001f, globalData.m_defaultShader, globalData.m_texLogo });
		npcQuad0.push_back({ npcPos, glm::vec2(0.2f), -0.001f, globalData.m_defaultShader, globalData.m_texNPC0 });
		npcQuad0.push_back({ npcPos, glm::vec2(0.5f), -0.01f, globalData.m_defaultShader, globalData.m_texNPCDir });
		npcDesc.m_desc = &npcQuad0;
		m_npcRoute0.push_back({ {-2.0f,  0.0f} });
		m_npcRoute0.push_back({ {-2.0f,  4.0f} });
		m_npcRoute0.push_back({ { 2.0f,  4.0f} });
		m_npcRoute0.push_back({ { 2.0f, -1.0f} });
		npcDesc.m_route = &m_npcRoute0;
		allNpcDesc.push_back(npcDesc);
	}

	std::vector<QuadInitDesc> npcQuad1;
	std::vector<NPCRoutePoint> m_npcRoute1;
	{
		NPCInitDesc npcDesc{};
		glm::vec2 npcPos = glm::vec2(-2.0f, -1.0f);
		npcQuad1.push_back({ npcPos, glm::vec2(0.2f), -0.002f, globalData.m_defaultShader, globalData.m_texLogo });
		npcQuad1.push_back({ npcPos, glm::vec2(0.2f), -0.002f, globalData.m_defaultShader, globalData.m_texNPC0 });
		npcQuad1.push_back({ npcPos, glm::vec2(0.5f), -0.02f, globalData.m_defaultShader, globalData.m_texNPCDir });
		npcDesc.m_desc = &npcQuad1;
		m_npcRoute1.push_back({ { 2.0f, -1.0f} });
		m_npcRoute1.push_back({ { 2.0f,  4.0f} });
		m_npcRoute1.push_back({ {-2.0f,  4.0f} });
		m_npcRoute1.push_back({ {-2.0f,  0.0f} });
		npcDesc.m_route = &m_npcRoute1;
		allNpcDesc.push_back(npcDesc);
	}

	std::vector<std::vector<QuadInitDesc>> allMapDesc{};

	std::vector<QuadInitDesc> mapDesc{};
	mapDesc.push_back({ glm::vec2(1.2f, 0.0f), glm::vec2(0.5f), -0.3f, globalData.m_defaultShader, globalData.m_texLogo });
	mapDesc.push_back({ glm::vec2(1.2f, 0.0f), glm::vec2(0.5f), -0.3f, globalData.m_defaultShader, globalData.m_texLogo });
	allMapDesc.push_back(mapDesc);

	SceneInitDesc initDesc;
	initDesc.m_renderer = desc.m_renderer;
	initDesc.m_collision = desc.m_collision;
	initDesc.m_player = &playerDesc;
	initDesc.m_playerCamera = desc.m_camera;
	initDesc.m_npcs = &allNpcDesc;
	initDesc.m_map = &allMapDesc;

	std::vector<std::shared_ptr<Item>> items;
	for (float y = 1.0f; y < 3.0f; y += 0.5f)
	{
		for (float x = -1.0f; x < 1.0f; x += 0.5f)
		{
			items.emplace_back(std::make_shared<Disguise>());
			dynamic_cast<Disguise*>(items.back().get())->Init(Disguise::Type::GAURD, glm::vec2(x, y), 0.0f, globalData.m_defaultShader);
		}
	}
	m_gameScene.GetItems().AddItem(items);
	m_gameScene.Init(initDesc);
}

void Game::Tick(GameTickDesc& desc)
{
	Scene* scene = GlobalData::Get().m_scene;

	for (NPC& npc : m_gameScene.GetNPCs())
		scene->GetAABBs()[npc.GetUUID(0).GetUUID()].SetEnabled(false);
	m_gameScene.GetPlayer().PlayerTick(desc);
	for (NPC& npc : m_gameScene.GetNPCs())
		scene->GetAABBs()[npc.GetUUID(0).GetUUID()].SetEnabled(true);

	InteractTick(desc);

	if (glm::length(m_gameScene.GetPlayer().GetVelocity()) != 0.0f || true)
		for (NPC& npc : m_gameScene.GetNPCs())
			npc.NPCTick(desc);
}

void Game::InteractTick(GameTickDesc& desc)
{
	Scene* scene = GlobalData::Get().m_scene;

	m_interact.reset();

	// get possible interactive NPC
	scene->GetRenderQuads()[m_gameScene.GetPlayer().GetUUID(1).GetUUID()].SetVisibility(false);
	for (NPC& npc : m_gameScene.GetNPCs())
	{
		glm::vec2 diff = npc.GetPos() - m_gameScene.GetPlayer().GetPos();
		if (glm::length(diff) < 0.5f && !npc.IsPlayerDetected())
		{
			m_interact = std::make_shared<KillInteract>(&m_gameScene.GetPlayer(), &npc);
			break;
		}
	}

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