#include "DoorInteract.h"

#include "../GameScene.h"

void DoorInteract::OnInteract()
{
	Scene* scene = GlobalData::Get().m_scene;
	CollisionDetector* collision = GlobalData::Get().m_collision;
	Object& obj = m_scene->GetSpecialBlockManager().GetObjects()[m_objIndex];

	m_isOpen = !m_isOpen;

	if (m_isOpen)
	{
		scene->GetAABBs()[obj.GetUUID(0).GetUUID()].SetEnabled(false);
		obj.GetQuad(0)->SetRadius({});
	}
	else
	{
		scene->GetAABBs()[obj.GetUUID(0).GetUUID()].SetEnabled(true);
		obj.GetQuad(0)->SetRadius(m_radius);
		
		CollisionPayload payload = collision->Collide(1, obj.GetUUID(0));
		if (payload.m_hasHit)
		{
			m_isOpen = true;
			scene->GetAABBs()[obj.GetUUID(0).GetUUID()].SetEnabled(false);
			obj.GetQuad(0)->SetRadius({});
		}
	}
}

glm::vec2 DoorInteract::OnTick(bool* show)
{
	Object& obj = m_scene->GetSpecialBlockManager().GetObjects()[m_objIndex];
	return obj.GetQuad(0)->GetPos();
}