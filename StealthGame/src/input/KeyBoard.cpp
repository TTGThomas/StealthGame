#include "KeyBoard.h"

std::bitset<GLFW_KEY_LAST> KeyBoard::m_keyDown{};
std::bitset<GLFW_KEY_LAST> KeyBoard::m_KeyPress{};

void KeyBoard::KeyCallback(GLFWwindow* window, int key, int scanCode, int action, int mods)
{
	bool last = m_keyDown[key];
	bool curr = action != GLFW_RELEASE;
	m_keyDown[key] = curr;
	m_KeyPress[key] = last ^ curr;
}

bool KeyBoard::IsKeyPressDown(int key)
{
	return m_KeyPress[key] && m_keyDown[key];
}

bool KeyBoard::IsKeyPressUp(int key)
{
	return m_KeyPress[key] && !m_keyDown[key];
}

bool KeyBoard::IsKeyDown(int key)
{
	return m_keyDown[key];
}

void KeyBoard::Flush()
{
	m_KeyPress.reset();
}
