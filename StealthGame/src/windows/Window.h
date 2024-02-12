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
void OnContentScaled(GLFWwindow* window, float width, float height);

class Window
{
public:
	Window(const char* title, int width, int height, bool vSynch = false, bool fullScreen = false, bool maximize = false);
	~Window();

	void NewFrame();
	void EndFrame();

	void SetWindowPos(int x, int y);

	int GetWidth();
	int GetHeight();
	bool GetIsResized();
	float GetRatio();
	bool GetShouldClose();
	GLFWwindow* GetWindow() { return m_window; }
private:
	bool Init(const char* title, int width, int height, bool vSynch, bool fullScreen, bool maximized);
private:
	GLFWwindow* m_window = nullptr;
};

