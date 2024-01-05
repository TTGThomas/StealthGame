#include "Camera.h"

void Camera::SetZoom(float zoom)
{
	m_zoom = zoom;
	if (m_zoom < m_minZoom)
		m_zoom = m_minZoom;
}

void Camera::ChangeZoom(float zoom)
{
	m_zoom += zoom;
	if (m_zoom < m_minZoom)
		m_zoom = m_minZoom;
}

void Camera::ShowStatsWindow()
{
	ImGui::Begin("Camera");
	ImGui::DragFloat2("Position", glm::value_ptr(m_pos), 0.0001f);
	ImGui::DragFloat("Zoom", &m_zoom, 0.01f);
	ImGui::End();
}
