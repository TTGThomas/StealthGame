#include "NPCInteract.h"

void NPCInteract::OnInteract()
{
	if (m_npc->GetIsDisposed())
		return;

	if (m_npc->GetHealth() > 0)
	{
		m_gameScene->GetPlayer().EliminateNPC(*m_npc);
		m_npc->EliminateMyself();
		m_gameScene->DeleteTarget(m_npc->GetNPCUUID().GetUUID());
	}
	else
	{
		m_gameScene->GetPlayer().SetIsCrouching(true);
		m_gameScene->GetPlayer().SetIsDragging(true);
		m_gameScene->GetPlayer().SetDraggedNPCID(m_npc->GetNPCUUID().GetUUID());
		m_npc->SetIsBeingDragged(true);
	}
}

glm::vec2 NPCInteract::OnTick(bool* show)
{
	if (m_npc->GetIsDisposed())
		*show = false;
	return m_npc->GetPos();
}