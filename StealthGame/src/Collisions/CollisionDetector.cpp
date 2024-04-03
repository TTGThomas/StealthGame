#include "CollisionDetector.h"

// be carefull, point is in world space not screen space
CollisionPayload CollisionDetector::Collide(int layer, glm::vec2 point, bool ignoreDisabled)
{
	for (int i = 0; i < m_layers[layer].size(); i++)
	{
		uint64_t uuid = m_layers[layer][i];
		Quad& quad = m_parent->GetQuads()[uuid];

		AABB& aabb = m_parent->GetAABBs()[uuid];
		aabb.SetMinPos(quad.GetPos() - quad.GetRadius());
		aabb.SetMaxPos(quad.GetPos() + quad.GetRadius());
		if (aabb.Collide(point, ignoreDisabled))
			return { true, UUID(uuid) };
	}
	return { false, UUID(0) };
}

CollisionPayload CollisionDetector::Collide(int layer, glm::vec2 l0, glm::vec2 l1, bool ignoreDisabled)
{
	for (int i = 0; i < m_layers[layer].size(); i++)
	{
		uint64_t uuid = m_layers[layer][i];
		Quad& quad = m_parent->GetQuads()[uuid];

		AABB& aabb = m_parent->GetAABBs()[uuid];
		aabb.SetMinPos(quad.GetPos() - quad.GetRadius());
		aabb.SetMaxPos(quad.GetPos() + quad.GetRadius());
		if (aabb.Collide(l0, l1, ignoreDisabled))
			return { true, UUID(uuid) };
	}
	return { false, UUID(0) };
}

CollisionPayload CollisionDetector::Collide(int layer, AABB& aabb, bool ignoreDisabled)
{
	for (int i = 0; i < m_layers[layer].size(); i++)
	{
		uint64_t uuid = m_layers[layer][i];
		Quad& quad = m_parent->GetQuads()[uuid];

		AABB& aabb = m_parent->GetAABBs()[uuid];
		aabb.SetMinPos(quad.GetPos() - quad.GetRadius());
		aabb.SetMaxPos(quad.GetPos() + quad.GetRadius());
		if (aabb.Collide(aabb, ignoreDisabled))
			return { true, UUID(uuid) };
	}
	return { false, UUID(0) };
}

CollisionPayload CollisionDetector::Collide(int layer, UUID& uuid, bool ignoreDisabled)
{
	{
		Quad& quad = m_parent->GetQuads()[uuid.GetUUID()];
		AABB& aabb = m_parent->GetAABBs()[uuid.GetUUID()];
		aabb.SetMinPos(quad.GetPos() - quad.GetRadius());
		aabb.SetMaxPos(quad.GetPos() + quad.GetRadius());
	}

	for (int i = 0; i < m_layers[layer].size(); i++)
	{
		uint64_t id = m_layers[layer][i];
		Quad& quad = m_parent->GetQuads()[id];

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

void CollisionDetector::SetLayers(int numLayers)
{
	m_layers.resize(numLayers);
	for (int i = 0; i < numLayers; i++)
		m_layers[i] = {};
}

void CollisionDetector::AddToLayer(int layer, uint64_t aabb)
{
	m_layers[layer].emplace_back(aabb);
}
