#pragma once

#include <fstream>
#include <string>

#include <vector>
#include <unordered_map>

#include "../Characters/Player.h"
#include "../Characters/NPC.h"

#include "../Items/Item.h"
#include "../Items/Disguise.h"

#include "../Interact/ContainerInteract.h"
#include "../Interact/DoorInteract.h"
#include "../Interact/ExitInteract.h"

#include "SceneConfig.h"

#include "../GameScene.h"

#include "../GlobalData.h"

#include "../Desc.h"

class SceneLoader
{
public:
	static SceneLoader& Get()
	{
		static SceneLoader sceneLoader;
		return sceneLoader;
	}

	void LoadMap(GameTickDesc& desc, GameScene* scene, class Game* game, int index);

	void LoadDebugScene(GameTickDesc& desc, GameScene* scene, class Game* game);
	void LoadTestLevel(GameTickDesc& desc, GameScene* scene, class Game* game);

	void LoadMenu(GameTickDesc& desc, GameScene* scene, Game* game);

	void LoadFromFile(GameTickDesc& desc, GameScene* scene, Game* game, const char* path, const char* name);
private:
	void LoadConstants(GameTickDesc& desc, GameScene* scene, Game* game);
	void LoadTextures(GameTickDesc& desc);
private:
	void GetNPCDataFromFile(std::vector<std::string>* names, std::vector<NPCRoutePoint>* route, const char* path, int row, int line);
	void LoadNPC(std::vector<NPCInitDesc>* npcMap, const char* name, bool isTarget, glm::vec2 pos, Identities type, uint64_t shader, uint64_t texture, std::vector<NPCRoutePoint>& route);
	void SetPlayer(std::vector<QuadInitDesc>* playerDesc, glm::vec2 pos, uint64_t shader, uint64_t texture);
	void LoadMap(std::vector<std::vector<QuadInitDesc>>* allMapDesc, glm::vec2 pos, glm::vec2 radius, uint64_t shader, uint64_t texture);
};