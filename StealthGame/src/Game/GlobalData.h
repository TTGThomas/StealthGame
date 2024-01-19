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
};

