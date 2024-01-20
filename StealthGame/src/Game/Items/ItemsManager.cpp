#include "ItemsManager.h"

void ItemsManager::AddItem(std::shared_ptr<Item> item)
{
	m_items[item->GetUUID().GetUUID()] = item;
}

void ItemsManager::AddItem(std::vector<std::shared_ptr<Item>>& items)
{
	for (std::shared_ptr<Item>& item : items)
	{
		AddItem(item);
	}
}

void ItemsManager::DeleteItem(uint64_t uuid)
{
	Scene* scene = GlobalData::Get().m_scene;

	scene->DeleteQuad(uuid);
	m_items.erase(uuid);
}

UUID ItemsManager::GetNearestUUID(glm::vec2 pos)
{
	uint64_t ret = 0;
	float retDist = -1.0f;
	for (auto [uuid, item] : m_items)
	{
		if (GetItem(uuid).get() == nullptr)
			continue;

		float dist = glm::distance(pos, GetItem(uuid)->GetQuad().GetPos());
		if (retDist == -1.0f || dist < retDist)
		{
			retDist = dist;
			ret = uuid;
		}
	}
	return UUID(ret);
}

std::shared_ptr<Item> ItemsManager::GetNearestItem(glm::vec2 pos)
{
	uint64_t index = GetNearestUUID(pos).GetUUID();
	return m_items[index];
}