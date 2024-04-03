#include "Entity.h"

void Entity::Move(CollisionDetector* collision, float x, float y)
{
	GetQuad(0)->ChangePos({ x, 0 });
	if (collision->Collide(0, GetUUID(0)).m_hasHit)
		GetQuad(0)->ChangePos({ -x, 0 });

	GetQuad(0)->ChangePos({ 0, y });
	if (collision->Collide(0, GetUUID(0)).m_hasHit)
		GetQuad(0)->ChangePos({ 0, -y });
}