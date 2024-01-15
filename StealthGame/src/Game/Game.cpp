#include "Game.h"

#include "Characters/NPCStats.h"

Game::Game()
{
}

Game::~Game()
{
}

void Game::Init(GameTickDesc& desc)
{
	desc.m_collision->BindMap(&desc.m_renderer->GetQuads());

	desc.m_camera->SetZoom(0.5f);

	desc.m_renderer->BindCamera(desc.m_camera);

	desc.m_renderer->AddShader("shaders/default/Vertex.glsl", "shaders/default/Fragment.glsl");
	
	desc.m_renderer->AddTexture("res/logo.png");
	desc.m_renderer->AddTexture("res/Player/player.png");
	desc.m_renderer->AddTexture("res/Player/Cursor.png");
	desc.m_renderer->AddTexture("res/NPC/NPC0.png");
	desc.m_renderer->AddTexture("res/NPC/Dir.png");

	NPCStatInitDesc npcStatDesc;
	npcStatDesc.m_deadBodyTextureIndex = 1;
	npcStatDesc.m_guestTextureIndex = 3;
	NPCStats::SetTextures(npcStatDesc);

	// layer 0 = objLayer
	// layer 1 = UILayer

	std::vector<QuadInitDesc> playerDesc{};
	playerDesc.push_back({ glm::vec2(0.0f, 0.0f), glm::vec2(0.2f), -0.002f, 0, 0});
	playerDesc.push_back({ glm::vec2(0.0f, 0.0f), glm::vec2(0.1f), -1.0f, 0, 2});
	playerDesc.push_back({ glm::vec2(0.0f, 0.0f), glm::vec2(0.2f), -0.002f, 0, 1});

	std::vector<NPCInitDesc> allNpcDesc{};

	std::vector<QuadInitDesc> npcQuad0;
	std::vector<NPCRoutePoint> m_npcRoute0;
	{
		NPCInitDesc npcDesc{};
		glm::vec2 npcPos = glm::vec2(-2.0f, 0.0f);
		npcQuad0.push_back({ npcPos, glm::vec2(0.2f), -0.001f, 0, 0 });
		npcQuad0.push_back({ npcPos, glm::vec2(0.2f), -0.001f, 0, 3 });
		npcQuad0.push_back({ npcPos, glm::vec2(0.5f), -0.01f, 0, 4 });
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
		npcQuad1.push_back({ npcPos, glm::vec2(0.2f), -0.002f, 0, 0 });
		npcQuad1.push_back({ npcPos, glm::vec2(0.2f), -0.002f, 0, 3 });
		npcQuad1.push_back({ npcPos, glm::vec2(0.5f), -0.02f, 0, 4 });
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
	mapDesc.push_back({ glm::vec2(1.2f, 0.0f), glm::vec2(0.5f), 0.0f, 0, 0 });
	mapDesc.push_back({ glm::vec2(1.2f, 0.0f), glm::vec2(0.5f), 0.0f, 0, 0 });
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
			dynamic_cast<Disguise*>(items.back().get())->Init(desc.m_renderer, Disguise::Type::GAURD, glm::vec2(x, y), 0.0f, 0);
		}
	}
	m_scene.GetItems().AddItem(items);
	m_scene.Init(initDesc);
}

void Game::Tick(GameTickDesc& desc)
{
	for (NPC& npc : m_scene.GetNPCs())
		npc.GetQuad(0)->GetAABB().SetEnabled(false);
	m_scene.GetPlayer().PlayerTick(desc);
	for (NPC& npc : m_scene.GetNPCs())
		npc.GetQuad(0)->GetAABB().SetEnabled(true);

	InteractTick(desc);

	if (glm::length(m_scene.GetPlayer().GetVelocity()) != 0.0f || true)
		for (NPC& npc : m_scene.GetNPCs())
			npc.NPCTick(desc);
}

void Game::InteractTick(GameTickDesc& desc)
{
	m_interact.reset();

	// get possible interactive NPC
	desc.m_renderer->GetQuads()[m_scene.GetPlayer().GetIndex(1)].SetVisibility(false);
	for (NPC& npc : m_scene.GetNPCs())
	{
		glm::vec2 diff = npc.GetPos() - m_scene.GetPlayer().GetPos();
		if (glm::length(diff) < 0.5f && !npc.IsPlayerDetected())
		{
			m_interact = std::make_shared<KillInteract>(&m_scene.GetPlayer(), &npc);
			break;
		}
	}

	// get possible interactive items
	Item* item = m_scene.GetItems().GetNearestItem(m_scene.GetPlayer().GetPos()).get();
	if (item != nullptr)
	{
		if (glm::distance(item->GetQuad().GetPos(), m_scene.GetPlayer().GetPos()) < 0.5f)
		{
			m_interact.reset();
			m_interact = std::make_shared<ItemInteract>(&m_scene, item);
		}
	}

	if (m_interact.get() != nullptr)
	{
		desc.m_renderer->GetQuads()[m_scene.GetPlayer().GetIndex(1)].SetPos(m_interact->OnTick());
		desc.m_renderer->GetQuads()[m_scene.GetPlayer().GetIndex(1)].SetVisibility(true);
		if (KeyBoard::IsKeyPressDown(GLFW_KEY_E))
		{
			m_interact->OnInteract();
		}
	}
}