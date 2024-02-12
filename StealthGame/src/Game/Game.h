#pragma once

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>

#include "../windows/Window.h"

#include "../input/KeyBoard.h"
#include "../input/Mouse.h"

#include "../quadRenderer/QuadRenderer.h"
#include "../Collisions/CollisionDetector.h"

#include "../Utils/Timer.h"

#include "Engine/Object.h"
#include "Engine/Entity.h"

#include "Characters/Player.h"
#include "Characters/NPC.h"

#include "Items/Item.h"
#include "Items/Disguise.h"

#include "GameScene.h"
#include "SceneLoader/SceneLoader.h"

#include "GlobalData.h"

#include "Desc.h"

class Game
{
public:
	Game();
	~Game();

	void Init(GameTickDesc& desc);

	void Tick(GameTickDesc& desc);

	void OnResize(int width, int height);
private:
	void InteractTick(GameTickDesc& desc);
	void InteractNPC();
	void InteracatItems();
private:
	void ShowStatsWindow();
private:
	GameScene m_gameScene;
	std::shared_ptr<Interaction> m_interact;
};