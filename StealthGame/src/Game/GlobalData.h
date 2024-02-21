#pragma once

#include "../Scene.h"

enum class DisguiseState
{
	// NORMAL = normal
	// COMPROMISED = takes time to recognize
	// ALERT = immediantley recognize
	NORMAL, COMPROMISED, ALERT
};

enum class Identities
{
	STANDARD, GUEST, VIPGUEST, GUARD, VIPGUARD
};

class GlobalData
{
public:
	static GlobalData& Get()
	{
		static GlobalData data;
		return data;
	}
public:
	Scene* m_scene = nullptr;
	class GameScene* m_gameScene = nullptr;
	class CollisionDetector* m_collision = nullptr;

	uint64_t m_defaultShader;
	uint64_t m_texLogo;
	uint64_t m_texPlayerCursor;
	uint64_t m_texPlayer;
	uint64_t m_texNPC0;
	uint64_t m_texNPC1;
	uint64_t m_texNPC2;
	uint64_t m_texNPC3;
	uint64_t m_texNPC4;
	uint64_t m_texNPCDead;
	uint64_t m_texNPCDir;
	uint64_t m_texContainer;
	uint64_t m_texDoor;

	int m_bodiesFound = 0;
};