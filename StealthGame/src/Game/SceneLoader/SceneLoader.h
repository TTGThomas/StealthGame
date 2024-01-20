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
};