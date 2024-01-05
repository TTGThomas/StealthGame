#include "Window.h"

static int x = 0, y = 0;

static void OnResize(GLFWwindow* window, int width, int height);

Window::Window(const char* title, int width, int height, bool vSynch, bool fullScreen)
{
	if (!Init(title, width, height, vSynch, fullScreen))
	{
		glfwTerminate();
	}
}

Window::~Window()
{
    ImGui_ImplGlfw_Shutdown();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(m_window);
    glfwTerminate();
}

bool Window::Init(const char* title, int width, int height, bool vSynch, bool fullScreen)
{
	if (!glfwInit())
	{
		std::cout << "fail to init glfw" << std::endl;
		return false;
	}

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_window = glfwCreateWindow(width, height, title, (fullScreen ? glfwGetPrimaryMonitor() : nullptr), nullptr);
    if (!m_window)
    {
        std::cout << "fail to create window" << std::endl;
        return false;
    }

    x = width;
    y = height;

    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(vSynch ? 1 : 0);// for clarification

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "fail to load glad" << std::endl;
        return false;
    }

    glfwSetWindowSizeCallback(m_window, OnResize);
    glfwSetKeyCallback(m_window, KeyBoard::KeyCallback);
    glfwSetCursorPosCallback(m_window, Mouse::MousePosCallback);
    glfwSetScrollCallback(m_window, Mouse::MouseScrollCallback);
    glfwSetMouseButtonCallback(m_window, Mouse::MouseButtonCallback);

    std::cout << glGetString(GL_VERSION) << std::endl;

    std::cout << "window created" << std::endl;
    
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& m_io = ImGui::GetIO();
    m_io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    m_io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    m_io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    m_io.ConfigFlags |= (fullScreen ? 0x0 : ImGuiConfigFlags_ViewportsEnable);
    
    ImGui::StyleColorsDark();
    
    ImGui_ImplGlfw_InitForOpenGL(m_window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    std::cout << "ImGui context created" << std::endl;

    return true;
}

void Window::NewFrame()
{
    glfwPollEvents();

    ImGui_ImplGlfw_NewFrame();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();

    glClear(GL_COLOR_BUFFER_BIT);
}

void Window::EndFrame()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }

    glfwSwapBuffers(m_window);
}

int Window::GetWidth()
{
	return x;
}

int Window::GetHeight()
{
	return y;
}

bool Window::GetShouldClose()
{
    return glfwWindowShouldClose(m_window);
}

void OnResize(GLFWwindow* window, int width, int height)
{
	x = width;
	y = height;
}