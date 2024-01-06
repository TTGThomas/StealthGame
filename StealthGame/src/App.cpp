#include "App.h"

App::App()
	: m_window("Stealth Game", 800, 600, false, false)
{
	m_renderer.BindCamera(&m_camera);

	m_renderer.AddShader("shaders/default/Vertex.glsl", "shaders/default/Fragment.glsl");
	m_renderer.AddTexture("res/logo.png");
	for (float y = -5.0f; y < 5.0f; y += 0.2f)
		for (float x = -5.0f; x < 5.0f; x += 0.2f)
			m_renderer.AddQuad(glm::vec2(x, y), glm::vec2(0.1f), 0, 0);
}

App::~App()
{
}

int App::Exec()
{
	while (!m_window.GetShouldClose())
	{
		m_window.NewFrame();
		glClearColor(0.2f, 0.2f, 0.3f, 1.0f);
		Tick();
		Mouse::Flush();
		KeyBoard::Flush();
		m_window.EndFrame();
	}
	return 0;
}

void App::Tick()
{
	UpdateCamera();

	ImGui::Begin("Stats");
	ImGui::Text("FPS: %ffps", ImGui::GetIO().Framerate);
	ImGui::Text("MS: %fms", ImGui::GetIO().DeltaTime);
	ImGui::Text("Wnd Scale: %i, %i", m_window.GetWidth(), m_window.GetHeight());
	ImGui::End();

	m_renderer.ShowStatsWindow();
	m_camera.ShowStatsWindow();

	m_renderer.Render((float)m_window.GetHeight() / (float)m_window.GetWidth());
}

void App::UpdateCamera()
{
	float speed = 1.0f * ImGui::GetIO().DeltaTime;
	glm::vec2 add{};
	if (KeyBoard::IsKeyDown(GLFW_KEY_W))
		add.y += speed;
	if (KeyBoard::IsKeyDown(GLFW_KEY_A))
		add.x -= speed;
	if (KeyBoard::IsKeyDown(GLFW_KEY_S))
		add.y -= speed;
	if (KeyBoard::IsKeyDown(GLFW_KEY_D))
		add.x += speed;
	m_camera.ChangePos(add);
	m_camera.ChangeZoom((float)Mouse::GetMouseScrollDY() * 0.1f);
}