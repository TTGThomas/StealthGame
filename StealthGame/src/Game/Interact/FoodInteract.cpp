#include "FoodInteract.h"

#include "../GameScene.h"

void FoodInteract::OnInteract()
{
	GlobalData& gData = GlobalData::Get();
	Scene* scene = GlobalData::Get().m_scene;
	CollisionDetector* collision = GlobalData::Get().m_collision;
	Object& obj = m_scene->GetSpecialBlockManager().GetObjects()[m_objIndex];

	m_poisoned = true;
	scene->GetRenderQuads()[obj.GetUUID(0).GetUUID()].SetTextureUUID(gData.m_texFoodPoisoned);
}

glm::vec2 FoodInteract::OnTick(bool* show)
{
	if (m_scene->GetPlayer().GetInventory().GetEquippiedType() != Inventory::Type::POISON)
		*show = false;

	if (m_poisoned)
		*show = false;

	Object& obj = m_scene->GetSpecialBlockManager().GetObjects()[m_objIndex];
	return obj.GetQuad(0)->GetPos();
}