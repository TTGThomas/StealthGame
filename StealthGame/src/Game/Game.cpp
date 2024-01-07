#include "Game.h"

Game::Game()
{
}

Game::~Game()
{
}

void Game::Init(GameTickDesc& desc)
{
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
	m_player.Init(desc.m_renderer, playerDesc);
	m_player.BindCamera(desc.m_camera);
	m_player.BindNPCs(&m_npcs);

	m_npcs.resize(1);

	std::vector<QuadInitDesc> npcDesc{};
	npcDesc.push_back({ glm::vec2(-1.2f, 0.0f), glm::vec2(0.2f), 0, 0 });
	npcDesc.push_back({ glm::vec2(-1.2f, 0.0f), glm::vec2(0.05f), 0, 4 });
	npcDesc.push_back({ glm::vec2(-1.2f, 0.0f), glm::vec2(0.2f), 0, 3 });
	m_npcs[0].Init(desc.m_renderer, npcDesc);

	std::vector<QuadInitDesc> mapDesc{};
	mapDesc.push_back({ glm::vec2(1.2f, 0.0f), glm::vec2(0.5f), 0, 0 });
	mapDesc.push_back({ glm::vec2(1.2f, 0.0f), glm::vec2(0.5f), 0, 0 });
	m_map0.Init(desc.m_renderer, mapDesc);

	desc.m_collision->BindMap(&desc.m_renderer->GetQuads());
}

void Game::Tick(GameTickDesc& desc)
{
	for (NPC& npc : m_npcs)
		npc.GetQuad(0)->GetAABB().SetEnabled(false);
	m_player.PlayerTick(desc);
	for (NPC& npc : m_npcs)
		npc.GetQuad(0)->GetAABB().SetEnabled(true);

	m_npcs[0].NPCTick(desc);
}