#include "App.h"

App::App()
//				name		   x    y    vSynch fullscreen maximize
	: m_window("Stealth Game", 800, 600, false, false,     true ), 
	m_renderer(&m_scene), 
	m_collision(&m_scene)
{
	m_renderer.BindCamera(&m_camera);

	GameTickDesc desc;
	desc.m_window = &m_window;
	desc.m_renderer = &m_renderer;
	desc.m_collision = &m_collision;
	desc.m_scene = &m_scene;
	desc.m_camera = &m_camera;
	desc.m_tickTimer = &m_tickTimer;
	m_game.Init(desc);
}

App::~App()
{
}

int App::Exec()
{
	while (!m_window.GetShouldClose())
	{
		m_tickTimer.Start();
		m_window.NewFrame();
		glClearColor(0.2f, 0.2f, 0.3f, 1.0f);
		Tick();
		Mouse::Flush();
		KeyBoard::Flush();
		m_window.EndFrame();
		m_tickTimer.Stop();
	}
	return 0;
}

void App::Tick()
{
	//UpdateCamera();
	UpdateSelection();

#ifndef IMGUI_DISABLE
	ImGui::Begin("Stats");
	ImGui::Text("FPS: %ffps", ImGui::GetIO().Framerate);
	ImGui::Text("MS: %fms", m_tickTimer.Ms());
	ImGui::Text("GPU Timer: %fms", m_gpuTimer.Ms());
	ImGui::Text("CPU Timer: %fms", m_tickTimer.Ms() - m_gpuTimer.Ms());
	ImGui::Text("Hovered Index: %i", m_hoveredIndex);
	ImGui::Text("Wnd Scale: %i, %i", m_window.GetWidth(), m_window.GetHeight());
	ImGui::End();
#endif

	m_renderer.ShowStatsWindow();
	m_camera.ShowStatsWindow();
	MemoryCounter::ShowStatsWindow();

	UpdateGame();

	if (m_window.GetIsResized())
	{
		m_game.OnResize(m_window.GetWidth(), m_window.GetHeight());
	}

	m_gpuTimer.Start();
	m_renderer.Render(m_window.GetRatio(), m_hoveredIndex);
	m_gpuTimer.Stop();
}

void App::UpdateCamera()
{
	float speed = 1.5f * m_tickTimer.Second();
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

void App::UpdateSelection()
{
	// hovered
	// (pos - camPos) * camZoom
	glm::vec2 cursPos = GetMouseGLPos();
	cursPos.x /= m_window.GetRatio();
	cursPos /= m_camera.GetZoom();
	cursPos += m_camera.GetPos();
	CollisionPayload payload = m_collision.Collide(cursPos);
	
	// selected
#ifndef IMGUI_DISABLE
	if (Mouse::IsMousePressDown(GLFW_MOUSE_BUTTON_LEFT) && !ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow))
#else
	if (Mouse::IsMousePressDown(GLFW_MOUSE_BUTTON_LEFT))
#endif
		m_selectedIndex = m_hoveredIndex;
}

void App::UpdateGame()
{
	GameTickDesc desc;
	desc.m_window = &m_window;
	desc.m_renderer = &m_renderer;
	desc.m_collision = &m_collision;
	desc.m_camera = &m_camera;
	desc.m_tickTimer = &m_tickTimer;
	desc.m_scene = &m_scene;
	m_game.Tick(desc);
}

glm::vec2 App::GetMouseGLPos()
{
	float x, y;
	x = (float)Mouse::GetMouseX() / (float)m_window.GetWidth();
	y = (float)Mouse::GetMouseY() / (float)m_window.GetHeight();

	x *= 2.0f;
	y *= 2.0f;

	x -= 1.0f;
	y -= 1.0f;

	y = -y;

	return { x, y };
}
