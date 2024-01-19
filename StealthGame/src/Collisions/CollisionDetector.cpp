#include "CollisionDetector.h"

// be carefull, point is in world space not screen space
CollisionPayload CollisionDetector::Collide(glm::vec2 point)
{
	for (auto& [uuid, quad] : m_parent->GetQuads())
	{
		AABB& aabb = m_parent->GetAABBs()[uuid];
		aabb.SetMinPos(quad.GetPos() - quad.GetRadius());
		aabb.SetMaxPos(quad.GetPos() + quad.GetRadius());
		if (aabb.Collide(point))
			return { true, UUID(uuid) };
	}
	return { false, UUID(0) };
}

CollisionPayload CollisionDetector::Collide(glm::vec2 l0, glm::vec2 l1)
{
	for (auto& [uuid, quad] : m_parent->GetQuads())
	{
		AABB& aabb = m_parent->GetAABBs()[uuid];
		aabb.SetMinPos(quad.GetPos() - quad.GetRadius());
		aabb.SetMaxPos(quad.GetPos() + quad.GetRadius());
		if (aabb.Collide(l0, l1))
			return { true, UUID(uuid) };
	}
	return { false, UUID(0) };
}

CollisionPayload CollisionDetector::Collide(AABB& aabb)
{
	for (auto& [uuid, quad] : m_parent->GetQuads())
	{
		AABB& aabb = m_parent->GetAABBs()[uuid];
		aabb.SetMinPos(quad.GetPos() - quad.GetRadius());
		aabb.SetMaxPos(quad.GetPos() + quad.GetRadius());
		if (aabb.Collide(aabb))
			return { true, UUID(uuid) };
	}
	return { false, UUID(0) };
}

CollisionPayload CollisionDetector::Collide(UUID& uuid)
{
	for (auto& [id, quad] : m_parent->GetQuads())
	{
		if (id == uuid.GetUUID())
			continue;

		AABB& aabb = m_parent->GetAABBs()[id];
		aabb.SetMinPos(quad.GetPos() - quad.GetRadius());
		aabb.SetMaxPos(quad.GetPos() + quad.GetRadius());
		if (aabb.Collide(m_parent->GetAABBs()[id]))
			return { true, UUID(uuid) };
	}
	return { false, UUID(0) };
}