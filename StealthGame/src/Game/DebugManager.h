#pragma once
#include <vector>

#include "Engine/Object.h"

#include "GlobalData.h"

class DebugManager
{
	struct DebugObjectDesc
	{
		DebugObjectDesc(glm::vec2 pos, glm::vec2 radius, float rotation)
			: m_pos(pos), m_radius(radius), m_rotation(rotation) {}

		glm::vec2 m_pos;
		glm::vec2 m_radius;
		float m_rotation;
	};
public:
	static void AddDebugQuad(glm::vec2 pos, glm::vec2 radius, float rotation);

	static void RenderDebugs();
private:
	static std::vector<DebugObjectDesc> m_objects;
};