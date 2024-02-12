#include "AABB.h"

AABB::AABB(glm::vec2 minPos, glm::vec2 maxPos, UUID uuid)
{
	m_uuid = uuid;
	m_minPos = minPos;
	m_maxPos = maxPos;
}

bool AABB::Collide(glm::vec2 point)
{
	if (!m_enabled)
		return false;

	if (point.x > m_minPos.x)
		if (point.x < m_maxPos.x)
			if (point.y > m_minPos.y)
				if (point.y < m_maxPos.y)
					return true;
	return false;
}

bool AABB::Collide(glm::vec2 l0, glm::vec2 l1)
{
	if (!m_enabled)
		return false;

	if (CollideLines(l0, l1, m_minPos, { m_minPos.x, m_maxPos.y }))
		return true;
	if (CollideLines(l0, l1, m_minPos, { m_maxPos.x, m_minPos.y }))
		return true;
	if (CollideLines(l0, l1, m_maxPos, { m_minPos.x, m_maxPos.y }))
		return true;
	if (CollideLines(l0, l1, m_maxPos, { m_maxPos.x, m_minPos.y }))
		return true;
	return false;
}

bool AABB::Collide(AABB& other)
{
	if (!m_enabled)
		return false;

	if (other.GetMinPos().x < m_maxPos.x)
		if (other.GetMaxPos().x > m_minPos.x)
			if (other.GetMinPos().y < m_maxPos.y)
				if (other.GetMaxPos().y > m_minPos.y)
					return true;
	return false;
}

bool AABB::CollideLines(glm::vec2 a0, glm::vec2 a1, glm::vec2 b0, glm::vec2 b1)
{
	// from https://www.jeffreythompson.org/collision-detection/line-line.php
	float inv = 1.0f / ((b1.y - b0.y) * (a1.x - a0.x) - (b1.x - b0.x) * (a1.y - a0.y));
	float uA = ((b1.x - b0.x) * (a0.y - b0.y) - (b1.y - b0.y) * (a0.x - b0.x)) * inv;
	float uB = ((a1.x - a0.x) * (a0.y - b0.y) - (a1.y - a0.y) * (a0.x - b0.x)) * inv;

	return uA >= 0.0f && uA <= 1.0f && uB >= 0.0f && uB <= 1.0f;
}