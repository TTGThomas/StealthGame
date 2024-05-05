#pragma once

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>

#include "../windows/Window.h"
#include "../WildException.h"

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

#include "UI/ZonePopUp.h"
#include "UI/ExitCurtainPopUp.h"

#include "GameScene.h"
#include "SceneLoader/SceneLoader.h"

#include "Animation/AnimationPlayer.h"

#include "GlobalData.h"

#include "Desc.h"

#include "DebugManager.h"

enum class GameState
{
	START, MENU, GAME, PAUSE
};

struct GameStateNode
{
	GameState m_state;
	std::vector<int> m_bridges;
};

class Game
{
public:
	Game();
	~Game();

	void Init(GameTickDesc& desc);

	void Tick(GameTickDesc& desc);

	void OnResize(int width, int height);

	void OnExit(int level);

	void ClearCurrentScene(GameTickDesc& desc);

	void InitZonePopUp(GameTickDesc& desc) { m_zonePopUp.Init(desc, &m_popUpManager); }
private:
	void InteractTick(GameTickDesc& desc);
	void InteractNPC();
	void InteractItems();
	void InteractSpecialBlocks();
private:
	void ShowStatsWindow();
private:
	void NetworkTick(GameTickDesc& desc);
	void GameTick(GameTickDesc& desc);
private:
	void SwitchState(GameTickDesc& desc, int bridgeIndex);

	void LoadStart(GameTickDesc& desc);
	void LoadMenu(GameTickDesc& desc);
	void LoadPause(GameTickDesc& desc);

	void StartTick(GameTickDesc& desc);
	void MenuTick(GameTickDesc& desc);
	void PauseTick(GameTickDesc& desc);
private:
	std::array<GameStateNode, 4> m_gameStates;
	int m_gameState = 0;

	bool m_isOnline = false;
	GameScene m_gameScene;
	std::shared_ptr<Interaction> m_interact;
	float m_interactDist = 0.0f;

	bool m_exiting = false;
	float m_exitStartTime = -1.0f;
	int m_exitMap = 0;

	PopUpManager m_popUpManager;
	ZonePopUp m_zonePopUp;
	ExitCurtainPopUp m_exitPopUp;
};