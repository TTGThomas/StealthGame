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

	Texture npcDead("res/NPC/NPCDead.png");
	GlobalData::Get().m_texNPCDead = npcDead.GetUUID().GetUUID();
	desc.m_renderer->AddTexture(npcDead);

	Texture npc0("res/NPC/NPC0.png");
	GlobalData::Get().m_texNPC0 = npc0.GetUUID().GetUUID();
	desc.m_renderer->AddTexture(npc0);

	Texture npc2("res/NPC/NPC2.png");
	GlobalData::Get().m_texNPC2 = npc2.GetUUID().GetUUID();
	desc.m_renderer->AddTexture(npc2);

	Texture npc4("res/NPC/NPC4.png");
	GlobalData::Get().m_texNPC4 = npc4.GetUUID().GetUUID();
	desc.m_renderer->AddTexture(npc4);

	Texture npcDir("res/NPC/Dir.png");
	GlobalData::Get().m_texNPCDir = npcDir.GetUUID().GetUUID();
	desc.m_renderer->AddTexture(npcDir);


	//Texture foreground("res/NPC/Dir.png");
	//uint64_t foregroundID = foreground.GetUUID().GetUUID();
	uint64_t foregroundID = 0;
	//desc.m_renderer->AddTexture(foreground);

	Texture background("res/Levels/DebugLevel.png");
	uint64_t backgroundID = background.GetUUID().GetUUID();
	desc.m_renderer->AddTexture(background);

	// UILayer > 0.9
	// objLayer < 0.9

	// map background = -0.1
	// map foreground = 0.99

	GlobalData& gData = GlobalData::Get();

	// Player
	std::vector<QuadInitDesc> playerDesc{};
	SetPlayer(&playerDesc, { 0.0f, 0.0f }, gData.m_defaultShader, gData.m_texPlayer);
	
	// NPC
	std::vector<NPCInitDesc> allNpcDesc{};

	{
		std::vector<NPCRoutePoint> npcRoute;
		npcRoute.push_back({ {-2.0f,  0.0f} });
		npcRoute.push_back({ {-2.0f,  4.0f} });
		npcRoute.push_back({ { 2.0f,  4.0f} });
		npcRoute.push_back({ { 2.0f, -1.0f} });

		LoadNPC(&allNpcDesc, "Robert Knox", true, {-2.0f, 0.0f}, Identities::GUARD, gData.m_defaultShader, gData.m_texNPC4, npcRoute);
	}
	
	{
		std::vector<NPCRoutePoint> npcRoute;
		npcRoute.push_back({ { 2.0f, -1.0f} });
		npcRoute.push_back({ { 2.0f,  4.0f} });
		npcRoute.push_back({ {-2.0f,  4.0f} });
		npcRoute.push_back({ {-2.0f,  0.0f} });

		LoadNPC(&allNpcDesc, "Sierra Knox", true, {-2.0f, -1.0f}, Identities::GUARD, gData.m_defaultShader, gData.m_texNPC4, npcRoute);
	}

	{
		std::vector<NPCRoutePoint> npcRoute;
		LoadNPC(&allNpcDesc, "Chad Bingham", true, { 0.0f, 2.0f }, Identities::GUARD, gData.m_defaultShader, gData.m_texNPC4, npcRoute);
	}

	// Map
	std::vector<std::vector<QuadInitDesc>> allMapDesc{};
	
	std::vector<QuadInitDesc> mapDesc{};

	LoadMap(&allMapDesc, { 0.9f, 2.0f }, { 0.1f, 1.0f }, gData.m_defaultShader, gData.m_texLogo);
	LoadMap(&allMapDesc, { -0.9f, 2.0f }, { 0.1f, 1.0f }, gData.m_defaultShader, gData.m_texLogo);
	LoadMap(&allMapDesc, { 0.0f, 3.0f }, { 1.0f, 0.1f }, gData.m_defaultShader, gData.m_texLogo);
	LoadMap(&allMapDesc, { 0.65f, 1.0f }, { 0.35f, 0.1f }, gData.m_defaultShader, gData.m_texLogo);
	LoadMap(&allMapDesc, { -0.65f, 1.0f }, { 0.35f, 0.1f }, gData.m_defaultShader, gData.m_texLogo);

	// trespassing zones
	std::vector<AABB> trespassingZones = {};
	trespassingZones.emplace_back(AABB({ -0.8f, 1.0f }, { 0.0f, 3.0f }, UUID()));

	// hostile zones
	std::vector<AABB> hostileZones = {};
	hostileZones.emplace_back(AABB({ 0.0f, 1.0f }, { 0.8f, 3.0f }, UUID()));

	// special objects
	int specialBlockIndex = 0;
	{
		Object object;
		std::shared_ptr<ContainerInteract> event = std::make_shared<ContainerInteract>(scene, specialBlockIndex);
		std::vector<QuadInitDesc> objectDesc;
		float index = (0.6f + (allMapDesc.size() * 0.000001f));
		objectDesc.push_back({ {3.0f, 0.0f}, {0.2f, 0.2f}, index, gData.m_defaultShader, gData.m_texLogo });
		objectDesc.push_back({ {3.0f, 0.0f}, {0.2f, 0.2f}, index, gData.m_defaultShader, gData.m_texLogo });
		object.Init(objectDesc);
		scene->GetSpecialBlockManager().AddSpecialBlock(object, event);
		specialBlockIndex++;
	}


	// Init
	SceneInitDesc initDesc;
	initDesc.m_renderer = desc.m_renderer;
	initDesc.m_collision = desc.m_collision;
	initDesc.m_player = &playerDesc;
	initDesc.m_playerCamera = desc.m_camera;
	initDesc.m_npcs = &allNpcDesc;
	initDesc.m_map = &allMapDesc;
	initDesc.m_gameTickDesc = desc;
	initDesc.m_trespassingZones = &trespassingZones;
	initDesc.m_hostileZones = &hostileZones;
	initDesc.m_foregroundTexID = foregroundID;
	initDesc.m_backgroundTexID = backgroundID;

	scene->Init(initDesc);

	// Items
	std::vector<std::shared_ptr<Item>> items;
#if 0
	for (float y = 1.0f; y < 3.0f; y += 0.5f)
	{
		for (float x = -1.0f; x < 1.0f; x += 0.5f)
		{
			items.emplace_back(std::make_shared<Disguise>());
			dynamic_cast<Disguise*>(items.back().get())->Init(Disguise::Type::GAURD, glm::vec2(x, y), -0.0f, globalData.m_defaultShader);
		}
	}
#endif

	scene->GetItems().AddItem(items);
}

void SceneLoader::LoadNPC(std::vector<NPCInitDesc>* npcMap, const char* name, bool isTarget, glm::vec2 pos, Identities type, uint64_t shader, uint64_t texture, std::vector<NPCRoutePoint>& route)
{
	// npc are from 0.0 -- 0.2
	// npcdir are from 0.3 -- 0.5
	GlobalData& globalData = GlobalData::Get();
	NPCInitDesc npcDesc{};
	std::vector<QuadInitDesc> npcQuad;
	float index = npcMap->size() * 0.000001f;
	npcQuad.push_back({ pos, glm::vec2(0.2f), index, shader, texture});
	npcQuad.push_back({ pos, glm::vec2(0.2f), index, shader, texture });
	npcQuad.push_back({ pos, glm::vec2(0.7f, 0.2f), (0.3f + index), shader, globalData.m_texNPCDir });
	npcDesc.m_desc = npcQuad;
	npcDesc.m_route = route;
	npcDesc.m_type = type;
	npcDesc.m_name = name;
	npcDesc.m_isTarget = isTarget;

	npcMap->push_back(npcDesc);
}

void SceneLoader::SetPlayer(std::vector<QuadInitDesc>* playerDesc, glm::vec2 pos, uint64_t shader, uint64_t texture)
{
	GlobalData& globalData = GlobalData::Get();

	playerDesc->push_back({ pos, glm::vec2(0.2f), 0.25f, shader, globalData.m_texLogo });
	playerDesc->push_back({ pos, glm::vec2(0.1f), 1.0f, shader, globalData.m_texPlayerCursor });
	playerDesc->push_back({ pos, glm::vec2(0.2f), 0.25f, shader, texture });
}

// actually the map hitbox
void SceneLoader::LoadMap(std::vector<std::vector<QuadInitDesc>>* allMapDesc, glm::vec2 pos, glm::vec2 radius, uint64_t shader, uint64_t texture)
{
	// map is from 0.6 -- 0.9

	std::vector<QuadInitDesc> mapDesc{};
	float index = (0.6f + (allMapDesc->size() * 0.000001f));
	mapDesc.push_back({ pos, radius, index, shader, texture });
	mapDesc.push_back({ pos, radius, index, shader, texture });
	allMapDesc->push_back(mapDesc);
}