#pragma once

#include "../windows/Window.h"

#include "../quadRenderer/QuadRenderer.h"
#include "../Collisions/CollisionDetector.h"
#include "../Scene.h"

#include "../Utils/Timer.h"

struct GameTickDesc
{
	QuadRenderer* m_renderer;
	CollisionDetector* m_collision;
	Scene* m_scene;
	Camera* m_camera;
	Window* m_window;
	Timer* m_tickTimer;
};