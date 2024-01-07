#include "CollisionDetector.h"

void CollisionDetector::BindMap(std::vector<Quad>* map)
{
	m_map = map;
}

void CollisionDetector::DisableAABBs(std::vector<int> indices)
{
	for (int& index : indices)
	{
		(*m_map)[index].GetAABB().SetEnabled(false);
	}
}

// be carefull, point is in world space not screen space
CollisionPayload CollisionDetector::Collide(glm::vec2 point)
{
	for (int i = 0; i < m_map->size(); i++)
	{
		Quad& quad = (*m_map)[i];
		quad.UpdateAABB();
		if (quad.GetAABB().Collide(point))
			return { true, i };
	}
	return { false, -1 };
}

CollisionPayload CollisionDetector::Collide(AABB& aabb)
{
	for (int i = 0; i < m_map->size(); i++)
	{
		Quad& quad = (*m_map)[i];
		quad.UpdateAABB();
		if (quad.GetAABB().Collide(aabb))
			return { true, i };
	}
	return { false, -1 };
}

CollisionPayload CollisionDetector::Collide(int index)
{
	(*m_map)[index].UpdateAABB();
	AABB& aabb = (*m_map)[index].GetAABB();

	for (int i = 0; i < m_map->size(); i++)
	{
		if (i == index)
			continue;

		Quad& quad = (*m_map)[i];
		quad.UpdateAABB();
		if (quad.GetAABB().Collide(aabb))
		{
			return { true, i };
		}
	}

	return { false, -1 };
}