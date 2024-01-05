#include "Mouse.h"

bool Mouse::m_mouseIsFirst = true;
double Mouse::m_mousePosX;
double Mouse::m_mousePosY;
double Mouse::m_mouseLastPosX;
double Mouse::m_mouseLastPosY;
double Mouse::m_mouseDX;
double Mouse::m_mouseDY;

double Mouse::m_mouseScrollDX;
double Mouse::m_mouseScrollDY;

std::bitset<GLFW_MOUSE_BUTTON_LAST> Mouse::m_buttonDown;
std::bitset<GLFW_MOUSE_BUTTON_LAST> Mouse::m_buttonPress;

void Mouse::MousePosCallback(GLFWwindow* window, double x, double y)
{
	m_mousePosX = x;
	m_mousePosY = y;

	if (m_mouseIsFirst)
	{
		m_mouseIsFirst = false;

		m_mouseLastPosX = m_mousePosX;
		m_mouseLastPosX = m_mousePosY;
	}

	m_mouseDX = m_mousePosX - m_mouseLastPosX;
	m_mouseDY = m_mousePosY - m_mouseLastPosY;

	m_mouseLastPosX = m_mousePosX;
	m_mouseLastPosY = m_mousePosY;
}

double Mouse::GetMouseX()
{
	return m_mousePosX;
}

double Mouse::GetMouseY()
{
	return m_mousePosY;
}

double Mouse::GetMouseDX()
{
	return m_mouseDX;
}

double Mouse::GetMouseDY()
{
	return m_mouseDY;
}

void Mouse::MouseScrollCallback(GLFWwindow* window, double dx, double dy)
{
	m_mouseScrollDX = dx;
	m_mouseScrollDY = dy;
}

double Mouse::GetMouseScrollDX()
{
	return m_mouseScrollDX;
}

double Mouse::GetMouseScrollDY()
{
	return m_mouseScrollDY;
}

void Mouse::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	bool last = m_buttonDown[button];
	bool curr = action != GLFW_RELEASE;
	m_buttonDown[button] = curr;
	m_buttonPress[button] = last ^ curr;
}

bool Mouse::IsMouseDown(int button)
{
	return m_buttonDown[button];
}

bool Mouse::IsMousePressDown(int button)
{
	return m_buttonPress[button] && m_buttonDown[button];
}

bool Mouse::IsMousePressUp(int button)
{
	return m_buttonPress[button] && !m_buttonDown[button];
}

void Mouse::Flush()
{
	m_mouseDX = 0.0;
	m_mouseDY = 0.0;

	m_mouseScrollDX = 0.0;
	m_mouseScrollDY = 0.0;

	m_buttonPress.reset();
}