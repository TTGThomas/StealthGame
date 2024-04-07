#include "AABB.h"

AABB::AABB(glm::vec2 minPos, glm::vec2 maxPos, GameUUID uuid)
{
	m_uuid = uuid;
	m_minPos = minPos;
	m_maxPos = maxPos;
}

bool AABB::Collide(glm::vec2 point, bool ignoreDisabled)
{
	if (!m_enabled && ignoreDisabled)
		return false;

	if (point.x > m_minPos.x)
		if (point.x < m_maxPos.x)
			if (point.y > m_minPos.y)
				if (point.y < m_maxPos.y)
					return true;
	return false;
}

bool AABB::Collide(glm::vec2 l0, glm::vec2 l1, bool ignoreDisabled)
{
	if (!m_enabled && ignoreDisabled)
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

bool AABB::Collide(AABB& other, bool ignoreDisabled)
{
	if (!m_enabled && ignoreDisabled)
		return false;

	if (!other.GetEnabled() && ignoreDisabled)
		return false;

	glm::vec2 diff = glm::abs(GetCenterPoint() - other.GetCenterPoint());
	if (diff.x + diff.y > MAP_RADIUS * 4.1f)
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
	float b1ymb0y = (b1.y - b0.y);
	float a1xma0x = (a1.x - a0.x);
	float b1xmb0x = (b1.x - b0.x);
	float a1yma0y = (a1.y - a0.y);
	float a0ymb0y = (a0.y - b0.y);
	float a0xmb0x = (a0.x - b0.x);
	float inv = 1.0f / (b1ymb0y * a1xma0x - b1xmb0x * a1yma0y);
	float uA = (b1xmb0x * a0ymb0y - b1ymb0y * a0xmb0x) * inv;
	float uB = (a1xma0x * a0ymb0y - a1yma0y * a0xmb0x) * inv;

	return uA >= 0.0f && uA <= 1.0f && uB >= 0.0f && uB <= 1.0f;
}

float AABB::GetDistFromPos(glm::vec2 otherPos)
{
	return glm::distance(GetCenterPoint(), otherPos);
}
