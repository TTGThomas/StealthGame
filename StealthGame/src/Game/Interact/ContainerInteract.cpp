#include "ContainerInteract.h"

#include "../GameScene.h"

void ContainerInteract::OnInteract()
{
	if (m_spaceLeft == 0) 
		return;

	Object& obj = m_scene->GetSpecialBlockManager().GetObjects()[m_objIndex];

	Player* player = &m_scene->GetPlayer();
	if (player->GetIsDragging())
	{
		// hide body
		m_spaceLeft--;
		NPC* npc = &m_scene->GetNPCs()[player->GetDraggedNPCID().GetUUID()];

		npc->SetPos(obj.GetQuad(0)->GetPos());
		npc->SetIsDisposed(true);

		player->SetIsDragging(false);
	}
	else if (m_spaceLeft > 0)
	{
		// hide player
	}
}

glm::vec2 ContainerInteract::OnTick()
{
	Object& obj = m_scene->GetSpecialBlockManager().GetObjects()[m_objIndex];
	return obj.GetQuad(0)->GetPos();
}