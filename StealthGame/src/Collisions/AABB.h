#pragma once

#include <glm/glm.hpp>

class AABB
{
public:
	AABB() = default;
	AABB(glm::vec2 minPos, glm::vec2 maxPos);

	bool Collide(glm::vec2 point);
	bool Collide(AABB& other);

	void SetMinPos(glm::vec2 newPos) { m_minPos = newPos; }
	void SetMaxPos(glm::vec2 newPos) { m_maxPos = newPos; }
	void SetEnabled(bool enabled) { m_enabled = enabled; }

	const glm::vec2& GetMinPos() { return m_minPos; }
	const glm::vec2& GetMaxPos() { return m_maxPos; }
	bool GetEnabled() { return m_enabled; }
private:
	glm::vec2 m_minPos{};
	glm::vec2 m_maxPos{};
	bool m_enabled = true;
};