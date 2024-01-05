#include "App.h"

App::App()
	: m_window("Stealth Game", 800, 600, false, false)
{
	m_renderer.AddShader("shaders/default/Vertex.glsl", "shaders/default/Fragment.glsl");
	m_renderer.AddTexture("res/logo.png");
	m_renderer.AddQuad(glm::vec2(0.0f), glm::vec2(0.5f), 0, 0);
}

App::~App()
{
}

int App::Exec()
{
	while (!m_window.GetShouldClose())
	{
		m_window.NewFrame();
		Tick();
		Mouse::Flush();
		KeyBoard::Flush();
		m_window.EndFrame();
	}
	return 0;
}

void App::Tick()
{
	ImGui::Begin("Stats");
	ImGui::Text("FPS: %ffps", ImGui::GetIO().Framerate);
	ImGui::Text("MS: %fms", ImGui::GetIO().DeltaTime);
	ImGui::End();

	ImGui::Begin("Renderer");
	ImGui::Text("Quad count: %u", m_renderer.GetQuads().size());
	Quad& quad = m_renderer.GetQuads()[0];
	ImGui::DragFloat2("Position", glm::value_ptr(quad.GetPos()), 0.001f);
	ImGui::DragFloat2("Scale", glm::value_ptr(quad.GetScale()), 0.001f);
	ImGui::End();

	m_renderer.Render();
}