#pragma once

#include <vector>
#include <unordered_map>

#include <glm/glm.hpp>

#include "quadRenderer/RenderQuad.h"
#include "UUID.h"

#include "Collisions/AABB.h"

class Quad
{
public:
	Quad() = default;

	Quad(uint64_t uuid, glm::vec2 pos, glm::vec2 radius)
		: m_uuid(uuid), m_pos(pos), m_radius(radius) {}

	Quad(glm::vec2 pos, glm::vec2 radius)
		: m_pos(pos), m_radius(radius) {}

	void SetPos(glm::vec2 pos) { m_pos = pos; }
	void SetRadius(glm::vec2 radius) { m_radius = radius; }
	void SetRotation(float rot) { m_rotation = rot; }
	void ChangePos(glm::vec2 pos) { m_pos += pos; }
	void ChangeRadius(glm::vec2 radius) { m_radius += radius; }
	void ChangeRotation(float rot) { m_rotation += rot; }

	const glm::vec2& GetPos() { return m_pos; }
	const glm::vec2& GetRadius() { return m_radius; }
	float GetRotation() { return m_rotation; }
	UUID& GetUUID() { return m_uuid; }
private:
	glm::vec2 m_pos{};
	glm::vec2 m_radius{};
	float m_rotation = 0.0f;
	UUID m_uuid;
};

class Scene
{
public:
	~Scene();

	void AddQuad(Quad& quad, RenderQuadInitDesc& renderQuadDesc);

	std::unordered_map<uint64_t, Quad>& GetQuads() { return m_quads; }
	std::unordered_map<uint64_t, RenderQuad>& GetRenderQuads() { return m_renderQuads; }
	std::unordered_map<uint64_t, AABB>& GetAABBs() { return m_aabb; }

	// from the largest to the smallest
	std::vector<uint64_t>& GetDepthOrder() { return m_depthOrder; }
private:
	std::unordered_map<uint64_t, Quad> m_quads;
	std::unordered_map<uint64_t, RenderQuad> m_renderQuads;
	std::unordered_map<uint64_t, AABB> m_aabb;
	std::vector<uint64_t> m_depthOrder{};
};