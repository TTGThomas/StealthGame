#include "Disguise.h"

void Disguise::Init(QuadRenderer* renderer, Type type, glm::vec2 pos, float depth, int shaderIndex)
{
	m_map = renderer;
	m_type = type;

	renderer->AddQuad(pos, glm::vec2(0.2f, 0.2f), shaderIndex, depth, 0);
	renderer->GetQuads().back().GetAABB().SetEnabled(false);
	m_index = (int)renderer->GetQuads().size() - 1;
}

void Disguise::OnCollect(Scene* scene)
{
	scene->GetItems().DeleteItem(m_itemIndex);
}