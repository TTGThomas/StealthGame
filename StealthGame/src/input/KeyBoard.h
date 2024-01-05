#pragma once

#include <bitset>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class KeyBoard
{
public:
	static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

	static bool IsKeyPressDown(int key);
	static bool IsKeyPressUp(int key);
	static bool IsKeyDown(int key);

	static void Flush();
private:
	static std::bitset<GLFW_KEY_LAST> m_keyDown;
	static std::bitset<GLFW_KEY_LAST> m_KeyPress;
};