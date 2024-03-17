#pragma once

#include <glm/glm.hpp>

#include "../UUID.h"

#include "../Game/SceneLoader/SceneConfig.h"

class AABB
{
public:
	AABB() = default;
	AABB(glm::vec2 minPos, glm::vec2 maxPos, UUID uuid);

	bool Collide(glm::vec2 point);
	bool Collide(glm::vec2 l0, glm::vec2 l1);
	bool Collide(AABB& other);

	void SetMinPos(glm::vec2 newPos) { m_minPos = newPos; }
	void SetMaxPos(glm::vec2 newPos) { m_maxPos = newPos; }
	void SetEnabled(bool enabled) { m_enabled = enabled; }

	glm::vec2 GetCenterPoint()
	{
		return (m_minPos + m_maxPos) / 2.0f;
	}

	const glm::vec2& GetMinPos() { return m_minPos; }
	const glm::vec2& GetMaxPos() { return m_maxPos; }
	bool GetEnabled() { return m_enabled; }
private:
	bool CollideLines(glm::vec2 a0, glm::vec2 a1, glm::vec2 b0, glm::vec2 b1);
	float GetDistFromPos(glm::vec2 otherPos);
private:
	UUID m_uuid;
	glm::vec2 m_minPos{};
	glm::vec2 m_maxPos{};
	bool m_enabled = true;
};