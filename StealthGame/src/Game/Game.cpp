#include "Game.h"

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

	std::vector<QuadInitDesc> playerDesc{};
	playerDesc.push_back({ glm::vec2(0.0f, 0.0f), glm::vec2(0.2f), 0, 0});
	playerDesc.push_back({ glm::vec2(0.0f, 0.0f), glm::vec2(0.1f), 0, 2});
	playerDesc.push_back({ glm::vec2(0.0f, 0.0f), glm::vec2(0.2f), 0, 1});

	std::vector<std::vector<QuadInitDesc>> allNpcDesc{};

	std::vector<QuadInitDesc> npcDesc{};
	npcDesc.push_back({ glm::vec2(-1.2f, 0.0f), glm::vec2(0.2f), 0, 0 });
	npcDesc.push_back({ glm::vec2(-1.2f, 0.0f), glm::vec2(0.05f), 0, 4 });
	npcDesc.push_back({ glm::vec2(-1.2f, 0.0f), glm::vec2(0.2f), 0, 3 });
	allNpcDesc.push_back(npcDesc);

	std::vector<std::vector<QuadInitDesc>> allMapDesc{};

	std::vector<QuadInitDesc> mapDesc{};
	mapDesc.push_back({ glm::vec2(1.2f, 0.0f), glm::vec2(0.5f), 0, 0 });
	mapDesc.push_back({ glm::vec2(1.2f, 0.0f), glm::vec2(0.5f), 0, 0 });
	allMapDesc.push_back(mapDesc);

	SceneInitDesc initDesc;
	initDesc.m_renderer = desc.m_renderer;
	initDesc.m_player = &playerDesc;
	initDesc.m_playerCamera = desc.m_camera;
	initDesc.m_npcs = &allNpcDesc;
	initDesc.m_map = &allMapDesc;
	m_scene.Init(initDesc);

	std::vector<std::shared_ptr<Item>> items;
	for (float y = 1.0f; y < 3.0f; y += 0.1f)
	{
		for (float x = -1.0f; x < 1.0f; x += 0.1f)
		{
			items.emplace_back(std::make_shared<Disguise>());
			dynamic_cast<Disguise*>(items.back().get())->Init(desc.m_renderer, Disguise::Type::GAURD, glm::vec2(x, y), 0);
		}
	}
	m_scene.GetItems().AddItem(items);
}

void Game::Tick(GameTickDesc& desc)
{
	for (NPC& npc : m_scene.GetNPCs())
		npc.GetQuad(0)->GetAABB().SetEnabled(false);
	m_scene.GetPlayer().PlayerTick(desc);
	for (NPC& npc : m_scene.GetNPCs())
		npc.GetQuad(0)->GetAABB().SetEnabled(true);

	m_scene.InteractTick(desc);

	for (NPC& npc : m_scene.GetNPCs())
		npc.NPCTick(desc);
}