#pragma once

#include <vector>
#include <unordered_map>

#include "../Characters/Player.h"
#include "../Characters/NPC.h"

#include "../Items/Item.h"
#include "../Items/Disguise.h"

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

	void LoadDebugScene(GameTickDesc& desc, GameScene* scene);
private:
	void LoadNPC(std::vector<NPCInitDesc>* npcMap, glm::vec2 pos, NPC::Type type, uint64_t shader, uint64_t texture, std::vector<NPCRoutePoint>& route);
	void SetPlayer(std::vector<QuadInitDesc>* playerDesc, glm::vec2 pos, uint64_t shader, uint64_t texture);
	void LoadMap(std::vector<std::vector<QuadInitDesc>>* allMapDesc, glm::vec2 pos, glm::vec2 radius, uint64_t shader, uint64_t texture);
};