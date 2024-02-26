#include "ExitInteract.h"

#include "../GameScene.h"
#include "../Game.h"

void ExitInteract::OnInteract()
{
	m_game->OnExit(m_levelIndex);
}

glm::vec2 ExitInteract::OnTick(bool* show)
{
	Object& obj = m_scene->GetSpecialBlockManager().GetObjects()[m_objIndex];
	
	if (!m_scene->GetTargets().empty())
		*show = false;

	return obj.GetQuad(0)->GetPos();
}