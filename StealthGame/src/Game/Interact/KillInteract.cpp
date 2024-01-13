#include "KillInteract.h"

void KillInteract::OnInteract()
{
	m_player->EliminateNPC(*m_npc);
	m_npc->EliminateMyself();
}

glm::vec2 KillInteract::OnTick()
{
	return m_npc->GetPos();
}
