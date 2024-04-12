#pragma once

#include "../Scene.h"

enum class DisguiseState
{
	// NORMAL = normal
	// COMPROMISED = immediantley recognized
	NORMAL, COMPROMISED
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
	// in seconds
	float m_deltaTime = 0.0f;
	Scene* m_scene = nullptr;
	class GameScene* m_gameScene = nullptr;
	class CollisionDetector* m_collision = nullptr;
	class QuadRenderer* m_renderer = nullptr;

	uint64_t m_defaultShader;
	uint64_t m_texLogo;
	uint64_t m_texPlayerCursor;
	uint64_t m_texPlayer;
	uint64_t m_texNPC0;
	uint64_t m_texNPC1;
	uint64_t m_texNPC2;
	uint64_t m_texNPC3;
	uint64_t m_texNPC4;
	uint64_t m_texDisguise0;
	uint64_t m_texDisguise1;
	uint64_t m_texDisguise2;
	uint64_t m_texDisguise3;
	uint64_t m_texDisguiseSTD;
	uint64_t m_texNPCDead;
	uint64_t m_texNPCDir;
	uint64_t m_texBullet;
	uint64_t m_texContainer;
	uint64_t m_texDoor;

	uint64_t m_audioBR;
	uint64_t m_audioGun1;

	glm::vec2 m_bodyConcentrationPos;
	int m_bodiesFound = 0;
};