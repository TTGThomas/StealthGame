#pragma once

#include <fstream>
#include <string>

#include <vector>
#include <map>
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
#include "../Interact/FoodInteract.h"

#include "SceneConfig.h"

#include "../GameScene.h"

#include "../GlobalData.h"

#include "../Desc.h"

#undef LoadMenu

class SceneLoader
{

	struct RowLine
	{
		RowLine(int row, int line)
			: m_row(row), m_line(line) {}

		int Index() const { return m_row * 10000 + m_line; }

		bool operator < (const RowLine& other) const
		{
			return Index() < other.Index();
		}

		int m_row;
		int m_line;
	};

	struct LoadNPCDesc
	{
		LoadNPCDesc() = default;

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
	uint64_t NPCTex(Identities type, bool isTarget);
	// input ranges 0 - 1
	static float Depth(float depth);
private:
	void LoadConstants(GameTickDesc& desc, GameScene* scene, Game* game);
	void LoadTextures(GameTickDesc& desc);
	void LoadAudio(GameTickDesc& desc);
private:
	void LoadZones(const char* path);
private:
	void LoadNPC(LoadNPCDesc& desc);
	void SetPlayer(glm::vec2 pos, uint64_t shader, uint64_t texture);
	void LoadMap(glm::vec2 pos, glm::vec2 radius, uint64_t shader, uint64_t texture);
private:
	std::vector<AABB> m_trespass;
	std::vector<AABB> m_hostile;

	// row, line objIndex
	std::map<RowLine, int> m_specialMap;
	std::vector<std::string> m_npcNames;
	std::vector<NPCInitDesc> m_npcMap;
	std::vector<QuadInitDesc> m_playerDesc;
	std::vector<std::vector<QuadInitDesc>> m_allMapDesc;
};