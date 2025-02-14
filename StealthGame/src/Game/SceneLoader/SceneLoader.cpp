#include "SceneLoader.h"

#include "../Game.h"

void SceneLoader::LoadMap(GameTickDesc& desc, GameScene* scene, class Game* game, int index)
{
	switch (index)
	{
	case 0:
		LoadMenu(desc, scene, game);
		break;
	case 1:
		LoadFromFile(desc, scene, game, "res/Levels/DebugLevel/");
		break;
	}
}

void SceneLoader::LoadMenu(GameTickDesc& desc, GameScene* scene, Game* game)
{
	desc.m_camera->SetZoom(0.5f);
	game->InitZonePopUp(desc);

	LoadConstants(desc, scene, game);

	//Texture foreground("res/NPC/Dir.png");
	//uint64_t foregroundID = foreground.GetUUID().GetUUID();
	uint64_t foregroundID = 0;
	//desc.m_renderer->AddTexture(foreground);

	//Texture background("res/Levels/DebugLevel.png");
	//uint64_t backgroundID = background.GetUUID().GetUUID();
	uint64_t backgroundID = 0;
	//desc.m_renderer->AddTexture(background);

	// UILayer > 0.9
	// objLayer < 0.9

	// map background = -0.1
	// map foreground = 0.99

	GlobalData& gData = GlobalData::Get();

	m_playerDesc.clear();
	m_npcMap.clear();
	m_npcNames.clear();
	m_allMapDesc.clear();
	m_trespass.clear();
	m_hostile.clear();

	// Player
	SetPlayer({ 0.0f, 0.0f }, gData.m_defaultShader, gData.m_texPlayer);

	// NPC

	// Map
	LoadMap({ 0.0f, 2.1f }, { 3.0f, 0.1f }, gData.m_defaultShader, gData.m_texLogo);
	LoadMap({ -3.0f, 0.5f }, { 0.1f, 1.7f }, gData.m_defaultShader, gData.m_texLogo);
	LoadMap({ 3.0f, 0.5f }, { 0.1f, 1.7f }, gData.m_defaultShader, gData.m_texLogo);
	LoadMap({ 0.0f, -1.1f }, { 3.1f, 0.1f }, gData.m_defaultShader, gData.m_texLogo);

	// trespassing zones
	std::vector<AABB> trespassingZones = {};

	// hostile zones
	std::vector<AABB> hostileZones = {};

	// special objects
	int specialBlockIndex = 0;

	// the exits
	{
		Object object;
		glm::vec2 pos = { 0.0f, 1.5f };
		glm::vec2 radius = { 0.3f, 0.3f };
		std::shared_ptr<ExitInteract> event = std::make_shared<ExitInteract>(scene, game, specialBlockIndex, 1);
		std::vector<QuadInitDesc> objectDesc;
		float index = (0.6f + ((m_allMapDesc.size() + specialBlockIndex) * 0.000001f));
		objectDesc.push_back({ pos, radius, Depth(index), gData.m_defaultShader, gData.m_texLogo });
		objectDesc.push_back({ pos, radius, Depth(index), gData.m_defaultShader, gData.m_texDoor });
		object.Init(objectDesc, true, false);
		scene->GetSpecialBlockManager().AddSpecialBlock(object, event);
		specialBlockIndex++;
	}

	// Init
	SceneInitDesc initDesc;
	initDesc.m_renderer = desc.m_renderer;
	initDesc.m_collision = desc.m_collision;
	initDesc.m_player = &m_playerDesc;
	initDesc.m_playerCamera = desc.m_camera;
	initDesc.m_npcs = &m_npcMap;
	initDesc.m_map = &m_allMapDesc;
	initDesc.m_gameTickDesc = desc;
	initDesc.m_trespassingZones = &trespassingZones;
	initDesc.m_hostileZones = &hostileZones;
	initDesc.m_foregroundTexID = foregroundID;
	initDesc.m_backgroundTexID = backgroundID;

	scene->Init(initDesc);
}

void SceneLoader::LoadFromFile(GameTickDesc& desc, GameScene* scene, Game* game, const char* path)
{
	desc.m_camera->SetZoom(0.5f);
	game->InitZonePopUp(desc);

	std::ifstream mainFile;
	
	std::string mainFilePath = path;
	mainFilePath.append("Level.txt");
	mainFile.open(mainFilePath);
	
	if (!mainFile.is_open())
	{
		printf("fail to open file path: %s\n", mainFilePath.c_str());
		return;
	}

	LoadConstants(desc, scene, game);

	std::string backgroundPath = path;
	backgroundPath.append("Back.png");
	uint64_t backgroundID = desc.m_renderer->AddTexture(backgroundPath.c_str());

	std::string foregroundPath = path;
	foregroundPath.append("Fore.png");
	uint64_t foregroundID = desc.m_renderer->AddTexture(foregroundPath.c_str());

	GlobalData& gData = GlobalData::Get();

	m_playerDesc.clear();
	m_npcMap.clear();
	m_npcNames.clear();
	m_allMapDesc.clear();
	m_trespass.clear();
	m_hostile.clear();

	std::vector<std::shared_ptr<Item>> items;
	
	int specialBlockIndex = 0;

	std::string line = "";
	int noLine = 1;
	while (std::getline(mainFile, line))
	{
		int noChar = 1;
		for (char c : line)
		{
			glm::vec2 pos = { (float)(noChar - 1) * MAP_SCALE, (float)(noLine - 1) * -MAP_SCALE };
			pos += glm::vec2(MAP_RADIUS, -MAP_RADIUS);

			char lowC = std::tolower(c);

			if (c == 'p')
				SetPlayer(pos, gData.m_defaultShader, gData.m_texPlayer);
			else if (c == '#')
				LoadMap(pos, { MAP_RADIUS, MAP_RADIUS }, gData.m_defaultShader, gData.m_texLogo);
			else if (c == 'e')
			{
				Object object;
				glm::vec2 radius = { MAP_RADIUS, MAP_RADIUS };
				std::shared_ptr<ExitInteract> event = std::make_shared<ExitInteract>(scene, game, specialBlockIndex, 0);
				std::vector<QuadInitDesc> objectDesc;
				float index = (0.6f + ((m_allMapDesc.size() + specialBlockIndex) * 0.000001f));
				objectDesc.push_back({ pos, radius, Depth(index), gData.m_defaultShader, gData.m_texDoor });
				object.Init(objectDesc, true, false);
				scene->GetSpecialBlockManager().AddSpecialBlock(object, event);
				m_specialMap[RowLine(noChar, noLine)] = specialBlockIndex;
				specialBlockIndex++;
			}
			else if (c == 'd')
			{
				Object object;
				glm::vec2 radius = { MAP_RADIUS - 0.000001f, MAP_RADIUS - 0.000001f };
				std::shared_ptr<DoorInteract> event = std::make_shared<DoorInteract>(scene, specialBlockIndex, radius);
				std::vector<QuadInitDesc> objectDesc;
				float index = (0.6f + ((m_allMapDesc.size() + specialBlockIndex) * 0.000001f));
				objectDesc.push_back({ pos, radius, Depth(index), gData.m_defaultShader, gData.m_texDoor });
				object.Init(objectDesc, true, true);
				scene->GetSpecialBlockManager().AddSpecialBlock(object, event);
				m_specialMap[RowLine(noChar, noLine)] = specialBlockIndex;
				specialBlockIndex++;
			}
			else if (c == 'c')
			{
				Object object;
				std::shared_ptr<ContainerInteract> event = std::make_shared<ContainerInteract>(scene, specialBlockIndex);
				std::vector<QuadInitDesc> objectDesc;
				glm::vec2 radius = { MAP_RADIUS, MAP_RADIUS };
				float index = (0.6f + ((m_allMapDesc.size() + specialBlockIndex) * 0.000001f));
				objectDesc.push_back({ pos, radius, Depth(index), gData.m_defaultShader, gData.m_texContainer });
				object.Init(objectDesc, true, false);
				scene->GetSpecialBlockManager().AddSpecialBlock(object, event);
				m_specialMap[RowLine(noChar, noLine)] = specialBlockIndex;
				specialBlockIndex++;
			}
			else if (c == 'f')
			{
				Object object;
				std::shared_ptr<FoodInteract> event = std::make_shared<FoodInteract>(scene, specialBlockIndex);
				std::vector<QuadInitDesc> objectDesc;
				glm::vec2 radius = { MAP_RADIUS, MAP_RADIUS };
				float index = (0.6f + ((m_allMapDesc.size() + specialBlockIndex) * 0.000001f));
				objectDesc.push_back({ pos, radius, Depth(index), gData.m_defaultShader, gData.m_texFood });
				object.Init(objectDesc, true, false);
				scene->GetSpecialBlockManager().AddSpecialBlock(object, event);
				m_specialMap[RowLine(noChar, noLine)] = specialBlockIndex;
				specialBlockIndex++;
			}
			// npcs
			else if (lowC == 'n')
			{
				LoadNPCDesc desc;
				desc.m_isTarget = std::isupper(c);
				desc.m_line = noLine;
				desc.m_path = path;
				desc.m_pos = pos;
				desc.m_row = noChar;
				desc.m_shader = gData.m_defaultShader;
				m_npcLoad.emplace_back(desc);
			}
			// body concentration
			else if (c == 'b')
				gData.m_bodyConcentrationPos = pos;
			// disguises
			else if (c >= '0' && c <= '9')
			{
				items.emplace_back(std::make_shared<Disguise>());
				((Disguise*)items.back().get())->Init(Identities::GUEST, pos, Depth(0.0f), gData.m_defaultShader);
			}

			noChar++;
		}
		noLine++;
	}
	
	for (LoadNPCDesc& desc : m_npcLoad)
		LoadNPC(desc);

	LoadZones(path);

	// Init
	SceneInitDesc initDesc;
	initDesc.m_renderer = desc.m_renderer;
	initDesc.m_collision = desc.m_collision;
	initDesc.m_player = &m_playerDesc;
	initDesc.m_playerCamera = desc.m_camera;
	initDesc.m_npcs = &m_npcMap;
	initDesc.m_map = &m_allMapDesc;
	initDesc.m_gameTickDesc = desc;
	initDesc.m_trespassingZones = &m_trespass;
	initDesc.m_hostileZones = &m_hostile;
#if 0
	initDesc.m_foregroundTexID = foregroundID;
	initDesc.m_backgroundTexID = backgroundID;
#else
	initDesc.m_foregroundTexID = 0;
	initDesc.m_backgroundTexID = 0;
#endif

	scene->Init(initDesc);

	scene->GetItems().AddItem(items);

	mainFile.close();
}

std::unique_ptr<NPC> SceneLoader::MakeNPC(Identities type)
{
	switch (type)
	{
	case Identities::STANDARD:
		throw WildException("MakeNPC arg cannot be standard");
		break;
	case Identities::GUEST:
	case Identities::VIPGUEST:
		return std::make_unique<Guest>();
		break;
	case Identities::GUARD:
	case Identities::VIPGUARD:
		return std::make_unique<Guard>();
		break;
	default:
		break;
	}
	return {};
}

uint64_t SceneLoader::NPCTex(Identities type, bool isTarget)
{
	GlobalData& gData = GlobalData::Get();

	if (isTarget)
		return gData.m_texNPC4;

	switch (type)
	{
	case Identities::STANDARD:
		throw WildException("NPCTex arg cannot be standard");
		break;
	case Identities::GUEST:
		return gData.m_texNPC0;
		break;
	case Identities::VIPGUEST:
		return gData.m_texNPC1;
		break;
	case Identities::GUARD:
		return gData.m_texNPC2;
		break;
	case Identities::VIPGUARD:
		return gData.m_texNPC3;
		break;
	default:
		break;
	}
	return 0;
}

void SceneLoader::LoadConstants(GameTickDesc& desc, GameScene* scene, Game* game)
{
	LoadTextures(desc);
	LoadAudio(desc);

	game->OnResize(desc.m_window->GetWidth(), desc.m_window->GetHeight());
	scene->GetPlayer().GetInventory().GiveEverything();
	desc.m_collision->SetLayers(6);

	GlobalData::Get().m_bodiesFound = 0;
}

void SceneLoader::LoadTextures(GameTickDesc& desc)
{
	Shader shader("shaders/default/Vertex.glsl", "shaders/default/Fragment.glsl");
	GlobalData::Get().m_defaultShader = shader.GetUUID().GetUUID();
	desc.m_renderer->AddShader(shader);

	GlobalData::Get().m_texLogo = desc.m_renderer->AddTexture("res/logo.png");
	GlobalData::Get().m_texPlayerCursor = desc.m_renderer->AddTexture("res/Player/Cursor.png");
	GlobalData::Get().m_texPlayer = desc.m_renderer->AddTexture("res/Player/masterAtlas.png");
	GlobalData::Get().m_texNPCDead = desc.m_renderer->AddTexture("res/NPC/NPCDead.png");
	GlobalData::Get().m_texNPC0 = desc.m_renderer->AddTexture("res/NPC/NPC0/masterAtlas.png");
	GlobalData::Get().m_texNPC1 = desc.m_renderer->AddTexture("res/NPC/NPC1/masterAtlas.png");
	GlobalData::Get().m_texNPC2 = desc.m_renderer->AddTexture("res/NPC/NPC2/masterAtlas.png");
	GlobalData::Get().m_texNPC3 = desc.m_renderer->AddTexture("res/NPC/NPC3/masterAtlas.png");
	GlobalData::Get().m_texNPC4 = desc.m_renderer->AddTexture("res/NPC/NPC4/masterAtlas.png");
	GlobalData::Get().m_texNPCDir = desc.m_renderer->AddTexture("res/NPC/Dir.png");
	GlobalData::Get().m_texBullet = desc.m_renderer->AddTexture("res/Inventories/Bullet.png");
	GlobalData::Get().m_texContainer = desc.m_renderer->AddTexture("res/SpecialObjects/Container.png");
	GlobalData::Get().m_texDoor = desc.m_renderer->AddTexture("res/SpecialObjects/Door.png");
	GlobalData::Get().m_texFood = desc.m_renderer->AddTexture("res/SpecialObjects/Food.png");
	GlobalData::Get().m_texFoodPoisoned = desc.m_renderer->AddTexture("res/SpecialObjects/FoodPoisoned.png");
}

void SceneLoader::LoadAudio(GameTickDesc& desc)
{
	GlobalData& gData = GlobalData::Get();

	AudioManager& audio = desc.m_scene->GetAudio();
	audio.Init();
	
	gData.m_audioBR = audio.AddSound("res/Audio/BohemianRapsody.mp3",
		{},
		1.0f, 5.0f,
		false,
		true,
		true
	).GetUUID();

	gData.m_audioGun1 = audio.AddSound("res/Audio/Gun/gunshot1.mp3",
		{},
		10.0f, 50.0f,
		false,
		false,
		false
	).GetUUID();

	gData.m_audioGun2 = audio.AddSound("res/Audio/Gun/gunshot1.mp3",
		{},
		10.0f, 50.0f,
		false,
		false,
		false
	).GetUUID();

	gData.m_audioCoin = audio.AddSound("res/Audio/coinDrop.mp3",
		{},
		10.0f, 50.0f,
		false,
		false,
		false
	).GetUUID();

	audio.StartSound(gData.m_audioBR);
	audio.SetSoundVolume(gData.m_audioBR, 0.2f);
}

void SceneLoader::LoadZones(const char* path)
{
	std::string zonesFilePath = path;
	zonesFilePath.append("Zones.txt");

	std::ifstream zonesFile;
	zonesFile.open(zonesFilePath);

	int noLine = 1;
	std::string line = "";
	while (std::getline(zonesFile, line))
	{
		int noChar = 1;
		for (char c : line)
		{
			glm::vec2 pos = { (float)(noChar - 1) * MAP_SCALE, (float)(noLine - 1) * -MAP_SCALE };
			pos += glm::vec2(MAP_RADIUS, -MAP_RADIUS);

			glm::vec2 minPos = pos - glm::vec2(MAP_RADIUS, MAP_RADIUS);
			glm::vec2 maxPos = pos + glm::vec2(MAP_RADIUS, MAP_RADIUS);

			if (c == 'H')
			{
				m_hostile.emplace_back(AABB(minPos, maxPos, GameUUID()));
			}
			else if (c == 'T')
			{
				m_trespass.emplace_back(AABB(minPos, maxPos, GameUUID()));
			}
			noChar++;
		}
		noLine++;
	}
	zonesFile.close();
}

void SceneLoader::LoadNPC(LoadNPCDesc& desc)
{
	// npc are from 0.0 -- 0.2
	// npcdir are from 0.3 -- 0.5

	std::vector<NPCRoutePoint> npcRoute;

	std::string filePath = desc.m_path;
	filePath.append(std::to_string(desc.m_row));
	filePath.push_back('#');
	filePath.append(std::to_string(desc.m_line));
	filePath.append(".txt");

	std::ifstream file;
	file.open(filePath);

	Identities type = Identities::STANDARD;

	std::string lineStr = "";
	int lineIndex = 0;
	while (std::getline(file, lineStr))
	{
		if (lineStr[0] == '#')
			continue;

		if (lineIndex == 0)
			type = (Identities)std::stoi(lineStr);
		else if (lineIndex == 1)
			m_npcNames.push_back(lineStr);
		else
		{
			if (lineStr[0] == 'e')
			{
				int row = 0, line = 0;
				int commaIdx = (int)lineStr.find_first_of(',');
				row = std::stoi(lineStr.substr(2, commaIdx));
				line = std::stoi(lineStr.substr(commaIdx + 1));

				npcRoute.push_back(m_specialMap.at(RowLine(row, line)));
				continue;
			}
			
			int x = 0, y = 0, waitMs = 0;
			int commaIdx = (int)lineStr.find_first_of(',');
			int spaceIdx = (int)lineStr.find_last_of(' ');
			x = std::stoi(lineStr.substr(0, commaIdx));
			y = std::stoi(lineStr.substr(commaIdx + 1, spaceIdx - commaIdx));
			waitMs = std::stoi(lineStr.substr(spaceIdx + 1));

			glm::vec2 pos = { (float)(x - 1) * MAP_SCALE, (float)(y - 1) * -MAP_SCALE };
			pos += glm::vec2(MAP_RADIUS, -MAP_RADIUS);
			npcRoute.push_back(NPCRoutePoint(pos, waitMs));
		}
		lineIndex++;
	}

	GlobalData& globalData = GlobalData::Get();
	NPCInitDesc npcDesc{};
	std::vector<QuadInitDesc> npcQuad;
	float index = m_npcMap.size() * 0.000001f;
	uint64_t texture = NPCTex(type, desc.m_isTarget);
	npcQuad.push_back({ desc.m_pos, glm::vec2(0.2f), Depth(index), desc.m_shader, texture});
	npcQuad.push_back({ desc.m_pos, glm::vec2(MAP_RADIUS), Depth(index), desc.m_shader, texture });
	npcQuad.push_back({ desc.m_pos, glm::vec2(0.7f, 0.2f), Depth(0.3f + index), desc.m_shader, globalData.m_texNPCDir });
	npcDesc.m_desc = npcQuad;
	npcDesc.m_route = npcRoute;
	npcDesc.m_type = type;
	npcDesc.m_name = m_npcNames.back().c_str();
	npcDesc.m_isTarget = desc.m_isTarget;

	m_npcMap.push_back(npcDesc);
	file.close();
}

void SceneLoader::SetPlayer(glm::vec2 pos, uint64_t shader, uint64_t texture)
{
	GlobalData& globalData = GlobalData::Get();

	m_playerDesc.push_back({ pos, glm::vec2(0.2f, 0.2f), Depth(0.25f), shader, globalData.m_texLogo });
	m_playerDesc.push_back({ pos, glm::vec2(0.1f), Depth(1.0f), shader, globalData.m_texPlayerCursor });
	m_playerDesc.push_back({ pos, glm::vec2(MAP_RADIUS), Depth(0.25f), shader, texture });
}

// actually the map hitbox
void SceneLoader::LoadMap(glm::vec2 pos, glm::vec2 radius, uint64_t shader, uint64_t texture)
{
	// map is from 0.6 -- 0.9

	std::vector<QuadInitDesc> mapDesc{};
	float index = (0.6f + (m_allMapDesc.size() * 0.000001f));
	mapDesc.push_back({ pos, radius, Depth(index), shader, texture });
	m_allMapDesc.push_back(mapDesc);
}

float SceneLoader::Depth(float depth)
{
	return depth;
}
