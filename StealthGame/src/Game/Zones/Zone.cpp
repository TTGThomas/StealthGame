#include "Zone.h"

bool Zone::IsPointInZone(glm::vec2 point)
{
	for (AABB& aabb : m_zones)
		if (aabb.Collide(point))
			return true;
	return false;
}