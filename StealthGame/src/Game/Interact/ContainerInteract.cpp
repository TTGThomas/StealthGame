#include "ContainerInteract.h"

#include "../GameScene.h"

void ContainerInteract::OnInteract()
{
	Player* player = &m_scene->GetPlayer();
}

glm::vec2 ContainerInteract::OnTick()
{
	return m_obj->GetQuad(0)->GetPos();
}