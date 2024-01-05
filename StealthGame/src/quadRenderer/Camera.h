#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>

class Camera
{
public:
	Camera() = default;
	Camera(glm::vec2 pos, float zoom)
		: m_pos(pos), m_zoom(zoom) {}

	void SetPos(glm::vec2 pos) { m_pos = pos; }
	void SetPos(float x, float y) { m_pos = {x, y}; }

	void SetZoom(float zoom);

	void ChangePos(glm::vec2 pos) { m_pos += pos; }
	void ChangePos(float x, float y) { m_pos += glm::vec2(x, y); }

	void ChangeZoom(float zoom);

	void ShowStatsWindow();

	const glm::vec2& GetPos() { return m_pos; }
	float GetPosX() { return m_pos.x; }
	float GetPosY() { return m_pos.y; }
	float GetZoom() { return m_zoom; }
private:
	glm::vec2 m_pos;
	float m_zoom = 1.0f;

	constexpr static float m_minZoom = 0.1f;
};

