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
		NPC* npc = &m_scene->GetNPCs()[player->GetDraggedNPCID().GetUUID()];

		glm::vec2 pos = obj.GetQuad(0)->GetPos();
		float spacing = npc->GetQuad(1)->GetRadius().x * 0.25f;
		pos.x -= spacing;
		pos.x += (2 - m_spaceLeft) * spacing * 2.0f;
		npc->SetPos(pos);
		npc->SetIsDisposed(true);

		player->SetIsDragging(false);
		player->SetIsCrouching(false);
		m_spaceLeft--;
	}
	else if (m_spaceLeft > 0 && !player->GetIsHidden())
	{
		// hide player
		player->HidePlayer(obj.GetQuad(0)->GetPos());
	}
}

glm::vec2 ContainerInteract::OnTick(bool* show)
{
	Player* player = &m_scene->GetPlayer();
	*show = !player->GetIsHidden() && m_spaceLeft > 0;

	Object& obj = m_scene->GetSpecialBlockManager().GetObjects()[m_objIndex];
	return obj.GetQuad(0)->GetPos();
}