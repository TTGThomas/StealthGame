#pragma once

#include "../windows/Window.h"

#include "../quadRenderer/QuadRenderer.h"
#include "../Collisions/CollisionDetector.h"

#include "../Utils/Timer.h"

struct GameTickDesc
{
	QuadRenderer* m_renderer;
	CollisionDetector* m_collision;
	Camera* m_camera;
	Window* m_window;
	Timer* m_tickTimer;
};