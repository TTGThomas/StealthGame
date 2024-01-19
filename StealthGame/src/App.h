#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "windows/Window.h"

#include "input/KeyBoard.h"
#include "input/Mouse.h"

#include "quadRenderer/QuadRenderer.h"
#include "Collisions/CollisionDetector.h"

#include "Scene.h"

#include "Utils/Timer.h"

#include "Game/Game.h"

class App
{
public:
	App();
	~App();

	int Exec();
private:
	void Tick();
private:
	void UpdateCamera();
	void UpdateSelection();
	void UpdateGame();
private:
	glm::vec2 GetMouseGLPos();
private:
	Window m_window;
	Camera m_camera;

	Scene m_scene;
	QuadRenderer m_renderer;
	CollisionDetector m_collision;

	// timers here
	Timer m_gpuTimer;
	Timer m_tickTimer;

	int m_hoveredIndex = -1;
	int m_selectedIndex = -1;

	Game m_game;
};