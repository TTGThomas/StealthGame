#include "ItemInteract.h"

void ItemInteract::OnInteract()
{
	m_item->OnCollect(m_scene);
}

glm::vec2 ItemInteract::OnTick(bool* show)
{
	return m_item->GetQuad().GetPos();
}