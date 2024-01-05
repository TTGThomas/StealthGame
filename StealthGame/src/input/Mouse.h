#pragma once

#include <bitset>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui.h>

class Mouse
{
public:
	static void MousePosCallback(GLFWwindow* window, double x, double y);

	static double GetMouseX();
	static double GetMouseY();
	static double GetMouseDX();
	static double GetMouseDY();

	static void MouseScrollCallback(GLFWwindow* window, double dx, double dy);

	static double GetMouseScrollDX();
	static double GetMouseScrollDY();

	static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

	static bool IsMouseDown(int button);
	static bool IsMousePressDown(int button);
	static bool IsMousePressUp(int button);

	static void Flush();
private:
	static bool m_mouseIsFirst;
	static double m_mousePosX;
	static double m_mousePosY;
	static double m_mouseLastPosX;
	static double m_mouseLastPosY;
	static double m_mouseDX;
	static double m_mouseDY;

	static double m_mouseScrollDX;
	static double m_mouseScrollDY;

	static std::bitset<GLFW_MOUSE_BUTTON_LAST> m_buttonDown;
	static std::bitset<GLFW_MOUSE_BUTTON_LAST> m_buttonPress;
};

