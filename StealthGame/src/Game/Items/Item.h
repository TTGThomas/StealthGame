#pragma once

#include "../../quadRenderer/QuadRenderer.h"

// items should only have one quad
class Item
{
public:
	void SetItemIndex(int index) { m_itemIndex = index; }

	virtual void OnCollect(class Scene* scene) {}

	Quad& GetQuad() { return m_map->GetQuads()[m_index]; }
	int GetIndex() { return m_index; }
protected:
	QuadRenderer* m_map;
	int m_index;
	int m_itemIndex;
};