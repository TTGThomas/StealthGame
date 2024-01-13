#include "ItemsManager.h"

void ItemsManager::AddItem(std::shared_ptr<Item> item)
{
	m_items.emplace_back(item);
	m_items.back()->SetItemIndex((int)m_items.size() - 1);
}

void ItemsManager::AddItem(std::vector<std::shared_ptr<Item>>& items)
{
	for (std::shared_ptr<Item>& item : items)
	{
		AddItem(item);
	}
}

void ItemsManager::DeleteItem(int index)
{
	Item* item = m_items[index].get();
	m_map->GetQuads()[item->GetIndex()].Cleanup();
	m_map->GetQuads()[item->GetIndex()].SetVisibility(false);
	//m_map->GetQuads().erase(m_map->GetQuads().begin() + item->GetIndex());
	m_items[index].reset();
	//m_items.erase(m_items.begin() + index);
}

int ItemsManager::GetNearestIndex(glm::vec2 pos)
{
	int ret = -1;
	float retDist = 0.0f;
	for (int i = 0; i < m_items.size(); i++)
	{
		if (GetItem(i).get() == nullptr)
			continue;

		float dist = glm::distance(pos, GetItem(i)->GetQuad().GetPos());
		if (ret == -1 || dist < retDist)
		{
			retDist = dist;
			ret = i;
		}
	}
	return ret;
}

std::shared_ptr<Item> ItemsManager::GetNearestItem(glm::vec2 pos)
{
	int index = GetNearestIndex(pos);
	return index == -1 ? nullptr : m_items[index];
}