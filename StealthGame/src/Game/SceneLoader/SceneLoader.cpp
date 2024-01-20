#include "SceneLoader.h"

void SceneLoader::LoadDebugScene(GameTickDesc& desc, GameScene* scene)
{
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
	playerDesc.push_back({ glm::vec2(0.0f, 0.0f), glm::vec2(0.2f), -0.02f, globalData.m_defaultShader, globalData.m_texLogo });
	playerDesc.push_back({ glm::vec2(0.0f, 0.0f), glm::vec2(0.1f), -1.0f, globalData.m_defaultShader, globalData.m_texPlayerCursor });
	playerDesc.push_back({ glm::vec2(0.0f, 0.0f), glm::vec2(0.2f), -0.02f, globalData.m_defaultShader, globalData.m_texPlayer });

	std::vector<NPCInitDesc> allNpcDesc{};

	std::vector<QuadInitDesc> npcQuad0;
	std::vector<NPCRoutePoint> m_npcRoute0;
	{
		NPCInitDesc npcDesc{};
		glm::vec2 npcPos = glm::vec2(-2.0f, 0.0f);
		npcQuad0.push_back({ npcPos, glm::vec2(0.2f), -0.0101f, globalData.m_defaultShader, globalData.m_texLogo });
		npcQuad0.push_back({ npcPos, glm::vec2(0.2f), -0.0101f, globalData.m_defaultShader, globalData.m_texNPC0 });
		npcQuad0.push_back({ npcPos, glm::vec2(0.5f), -0.0201f, globalData.m_defaultShader, globalData.m_texNPCDir });
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
		npcQuad1.push_back({ npcPos, glm::vec2(0.2f), -0.0102f, globalData.m_defaultShader, globalData.m_texLogo });
		npcQuad1.push_back({ npcPos, glm::vec2(0.2f), -0.0102f, globalData.m_defaultShader, globalData.m_texNPC0 });
		npcQuad1.push_back({ npcPos, glm::vec2(0.5f), -0.0202f, globalData.m_defaultShader, globalData.m_texNPCDir });
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

	scene->Init(initDesc);

	std::vector<std::shared_ptr<Item>> items;
	for (float y = 1.0f; y < 3.0f; y += 0.1f)
	{
		for (float x = -1.0f; x < 1.0f; x += 0.1f)
		{
			items.emplace_back(std::make_shared<Disguise>());
			dynamic_cast<Disguise*>(items.back().get())->Init(Disguise::Type::GAURD, glm::vec2(x, y), -0.0f, globalData.m_defaultShader);
		}
	}
	scene->GetItems().AddItem(items);
}