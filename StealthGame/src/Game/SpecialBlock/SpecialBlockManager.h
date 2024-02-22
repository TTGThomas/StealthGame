#pragma once

#include <vector>
#include <memory>

#include <glm/glm.hpp>

#include "../Engine/Object.h"

#include "../Interact/Interaction.h"

#include "../GlobalData.h"

class SpecialBlockManager
{
public:
	void AddSpecialBlock(Object& object, std::shared_ptr<Interaction> event);

	std::shared_ptr<Interaction> GetClosestEventWithinRange(glm::vec2 pos, float range, float* dist);

	std::vector<Object>& GetObjects() { return m_objects; }
	std::vector<std::shared_ptr<Interaction>>& GetInteracts() { return m_interacts; }
private:
	std::vector<Object> m_objects;
	std::vector<std::shared_ptr<Interaction>> m_interacts;
};