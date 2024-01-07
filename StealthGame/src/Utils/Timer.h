#pragma once

#include <GLFW/glfw3.h>

class Timer
{
public:
	Timer() = default;

	void Start()
	{
		m_start = (float)glfwGetTime();
	}

	void Stop()
	{
		if (m_start == -1.0f)
			return;
		m_duration = (float)glfwGetTime() - m_start;
		m_start = -1.0f;
	}

	float Ms()
	{
		return m_duration * 1000.0f;
	}

	float Second()
	{
		return m_duration;
	}
private:
	float m_start = -1.0f, m_duration = 0.0f;
};