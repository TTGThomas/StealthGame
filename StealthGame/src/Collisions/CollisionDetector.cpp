#include "CollisionDetector.h"

// be carefull, point is in world space not screen space
CollisionPayload CollisionDetector::Collide(glm::vec2 point, bool ignoreDisabled)
{
	for (auto& [uuid, quad] : m_parent->GetQuads())
	{
		AABB& aabb = m_parent->GetAABBs()[uuid];
		aabb.SetMinPos(quad.GetPos() - quad.GetRadius());
		aabb.SetMaxPos(quad.GetPos() + quad.GetRadius());
		if (aabb.Collide(point, ignoreDisabled))
			return { true, UUID(uuid) };
	}
	return { false, UUID(0) };
}

CollisionPayload CollisionDetector::Collide(glm::vec2 l0, glm::vec2 l1, bool ignoreDisabled)
{
	for (auto& [uuid, quad] : m_parent->GetQuads())
	{
		AABB& aabb = m_parent->GetAABBs()[uuid];
		aabb.SetMinPos(quad.GetPos() - quad.GetRadius());
		aabb.SetMaxPos(quad.GetPos() + quad.GetRadius());
		if (aabb.Collide(l0, l1, ignoreDisabled))
			return { true, UUID(uuid) };
	}
	return { false, UUID(0) };
}

CollisionPayload CollisionDetector::Collide(AABB& aabb, bool ignoreDisabled)
{
	for (auto& [uuid, quad] : m_parent->GetQuads())
	{
		AABB& aabb = m_parent->GetAABBs()[uuid];
		aabb.SetMinPos(quad.GetPos() - quad.GetRadius());
		aabb.SetMaxPos(quad.GetPos() + quad.GetRadius());
		if (aabb.Collide(aabb, ignoreDisabled))
			return { true, UUID(uuid) };
	}
	return { false, UUID(0) };
}

CollisionPayload CollisionDetector::Collide(UUID& uuid, bool ignoreDisabled)
{
	{
		Quad& quad = m_parent->GetQuads()[uuid.GetUUID()];
		AABB& aabb = m_parent->GetAABBs()[uuid.GetUUID()];
		aabb.SetMinPos(quad.GetPos() - quad.GetRadius());
		aabb.SetMaxPos(quad.GetPos() + quad.GetRadius());
	}

	for (auto& [id, quad] : m_parent->GetQuads())
	{
		if (id == uuid.GetUUID())
			continue;

		AABB& aabb = m_parent->GetAABBs()[id];
		aabb.SetMinPos(quad.GetPos() - quad.GetRadius());
		aabb.SetMaxPos(quad.GetPos() + quad.GetRadius());
		if (aabb.Collide(m_parent->GetAABBs()[uuid.GetUUID()], ignoreDisabled))
			return { true, UUID(id) };
	}
	return { false, UUID(0) };
}