#pragma once

#include <fstream>
#include <string>

#include <vector>
#include <unordered_map>

#include "../../WildException.h"

#include "../Characters/Player.h"
#include "../Characters/NPC.h"

#include "../Characters/NPCs/Guest.h"
#include "../Characters/NPCs/Guard.h"

#include "../Items/Item.h"
#include "../Items/Disguise.h"

#include "../Interact/ContainerInteract.h"
#include "../Interact/DoorInteract.h"
#include "../Interact/ExitInteract.h"

#include "SceneConfig.h"

#include "../GameScene.h"

#include "../GlobalData.h"

#include "../Desc.h"

#undef LoadMenu

class SceneLoader
{
	struct LoadNPCDesc
	{
		LoadNPCDesc() = default;

		std::vector<NPCInitDesc>* m_npcMap;
		std::vector<std::string>* m_names;
		const char* m_path;
		int m_line;
		int m_row;
		bool m_isTarget;
		glm::vec2 m_pos;
		uint64_t m_shader;
	};
public:
	static SceneLoader& Get()
	{
		static SceneLoader sceneLoader;
		return sceneLoader;
	}

	void LoadMap(GameTickDesc& desc, GameScene* scene, class Game* game, int index);
	void LoadMenu(GameTickDesc& desc, GameScene* scene, Game* game);
	void LoadFromFile(GameTickDesc& desc, GameScene* scene, Game* game, const char* path);

	std::unique_ptr<NPC> MakeNPC(Identities type);
	uint64_t NPCTex(Identities type);
	// input ranges 0 - 1
	static float Depth(float depth);
private:
	void LoadConstants(GameTickDesc& desc, GameScene* scene, Game* game);
	void LoadTextures(GameTickDesc& desc);
	void LoadAudio(GameTickDesc& desc);
private:
	void LoadZones(const char* path, std::vector<AABB>* hostile, std::vector<AABB>* trespass);
private:
	void LoadNPC(LoadNPCDesc& desc);
	void SetPlayer(std::vector<QuadInitDesc>* playerDesc, glm::vec2 pos, uint64_t shader, uint64_t texture);
	void LoadMap(std::vector<std::vector<QuadInitDesc>>* allMapDesc, glm::vec2 pos, glm::vec2 radius, uint64_t shader, uint64_t texture);

};