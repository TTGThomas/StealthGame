#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "windows/Window.h"

#include "input/KeyBoard.h"
#include "input/Mouse.h"

#include "quadRenderer/QuadRenderer.h"

class App
{
public:
	App();
	~App();

	int Exec();
private:
	void Tick();
	void UpdateCamera();
private:
	Window m_window;
	Camera m_camera;
	QuadRenderer m_renderer;
};

