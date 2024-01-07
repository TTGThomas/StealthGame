#include "AABB.h"

AABB::AABB(glm::vec2 minPos, glm::vec2 maxPos)
{
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