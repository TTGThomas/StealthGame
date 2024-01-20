#pragma once

#include "../Scene.h"

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

	uint64_t m_defaultShader;
	uint64_t m_texLogo;
	uint64_t m_texPlayerCursor;
	uint64_t m_texPlayer;
	uint64_t m_texNPC0;
	uint64_t m_texNPCDir;
};

