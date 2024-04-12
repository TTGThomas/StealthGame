#include "NPCInteract.h"

#include "../Items/Disguise.h"

void NPCInteract::OnInteract()
{
	Player& player = m_gameScene->GetPlayer();
	if (m_npc->GetIsDisposed())
		return;

	if (m_npc->GetHealth() > 0)
	{
		std::shared_ptr<Disguise> disguise = std::make_shared<Disguise>();
		disguise->Init(m_npc->GetType(), player.GetPos(), 0.0f, GlobalData::Get().m_defaultShader);
		m_gameScene->GetItems().AddItem(disguise);
		player.EliminateNPC(*m_npc);
		m_npc->EliminateMyself();
	}
	else
	{
		player.SetIsCrouching(true);
		player.SetIsDragging(true);
		player.SetDraggedNPCID(m_npc->GetNPCUUID().GetUUID());
		m_npc->SetIsBeingDragged(true);
	}
}

glm::vec2 NPCInteract::OnTick(bool* show)
{
	if (m_npc->GetIsDisposed() || (m_gameScene->GetPlayer().GetInventory().GetEquippiedType() != Inventory::Type::FIBERWIRE && m_npc->GetHealth() > 0))
		*show = false;
	return m_npc->GetPos();
}