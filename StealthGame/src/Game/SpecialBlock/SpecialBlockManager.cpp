#include "SpecialBlockManager.h"

void SpecialBlockManager::AddSpecialBlock(Object& object, std::shared_ptr<Interaction> event)
{
	m_objects.emplace_back(std::move(object));
	m_interacts.emplace_back(event);
}

std::shared_ptr<Interaction> SpecialBlockManager::GetClosestEventWithinRange(glm::vec2 pos, float range, float* dist)
{
	int retIndex = -1;
	float retDist = range;
	for (int i = 0; i < m_objects.size(); i++)
	{
		Object& obj = m_objects[i];

		glm::vec2 objPos = obj.GetQuad(0)->GetPos();
		glm::vec2 diff = pos - objPos;
		float dist = glm::length(diff); // pythagoras
		//float dist = glm::abs(diff.x) + glm::abs(diff.y); // manhattem
		if (dist < retDist)
		{
			bool last = GlobalData::Get().m_scene->GetAABBs()[obj.GetUUID(0).GetUUID()].GetEnabled();
			GlobalData::Get().m_scene->GetAABBs()[obj.GetUUID(0).GetUUID()].SetEnabled(false);
			if (!GlobalData::Get().m_collision->Collide(0, pos, objPos).m_hasHit)
			{
				retDist = dist;
				retIndex = i;
			}
			GlobalData::Get().m_scene->GetAABBs()[obj.GetUUID(0).GetUUID()].SetEnabled(last);
		}
	}
	if (retIndex == -1)
		return nullptr;

	*dist = retDist;
	return m_interacts[retIndex];
}
