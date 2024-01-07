#pragma once

#include <iostream>

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include "../input/KeyBoard.h"
#include "../input/Mouse.h"

void OnResize(GLFWwindow* window, int width, int height);
void OnResize(GLFWwindow* window, int width, int height, int x, int y);

class Window
{
public:
	Window(const char* title, int width, int height, bool vSynch = false, bool fullScreen = false);
	~Window();

	void NewFrame();
	void EndFrame();

	int GetWidth();
	int GetHeight();
	float GetRatio();
	bool GetShouldClose();
	GLFWwindow* GetWindow() { return m_window; }
private:
	bool Init(const char* title, int width, int height, bool vSynch, bool fullScreen);
private:
	GLFWwindow* m_window = nullptr;
};

