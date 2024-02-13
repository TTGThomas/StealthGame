#include "SpecialBlockManager.h"

void SpecialBlockManager::AddSpecialBlock(Object& object, std::shared_ptr<Interaction> event)
{
	m_objects.emplace_back(std::move(object));
	m_interacts.emplace_back(event);
}

std::shared_ptr<Interaction> SpecialBlockManager::GetClosestEventWithinRange(glm::vec2 pos, float range)
{
	int retIndex = -1;
	float retDist = range;
	for (int i = 0; i < m_objects.size(); i++)
	{
		Object& obj = m_objects[i];

		glm::vec2 objPos = obj.GetQuad(0)->GetPos();
		float dist = glm::distance(pos, objPos);
		if (dist < retDist)
		{
			retDist = dist;
			retIndex = i;
		}
	}
	if (retIndex == -1)
		return nullptr;

	return m_interacts[retIndex];
}
