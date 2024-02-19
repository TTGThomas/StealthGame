#include "Window.h"

static bool resized = false;
static int x = 0, y = 0;
static float ratio = 1.0f;

Window::Window(const char* title, int width, int height, bool vSynch, bool fullScreen, bool maximize)
{
	if (!Init(title, width, height, vSynch, fullScreen, maximize))
		glfwTerminate();
}

Window::~Window()
{
#ifndef IMGUI_DISABLE
    ImGui_ImplGlfw_Shutdown();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();
#endif
    glfwDestroyWindow(m_window);
    glfwTerminate();
}

bool Window::Init(const char* title, int width, int height, bool vSynch, bool fullScreen, bool maximized)
{
    std::cout << "program started" << std::endl;
	if (!glfwInit())
	{
		std::cout << "fail to init glfw" << std::endl;
		return false;
	}

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHint(GLFW_MAXIMIZED, (maximized ? GL_TRUE : GL_FALSE));
    
    m_window = glfwCreateWindow(width, height, title, (fullScreen ? glfwGetPrimaryMonitor() : nullptr), nullptr);
    
    if (!m_window)
    {
        std::cout << "fail to create window" << std::endl;
        return false;
    }

    glfwGetFramebufferSize(m_window, &x, &y);

    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(vSynch ? 1 : 0);// for clarification

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "fail to load glad" << std::endl;
        return false;
    }

    OnResize(m_window, x, y);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //glfwSetWindowContentScaleCallback(m_window, OnContentScaled);
    glfwSetFramebufferSizeCallback(m_window, OnResize);
    glfwSetKeyCallback(m_window, KeyBoard::KeyCallback);
    glfwSetCursorPosCallback(m_window, Mouse::MousePosCallback);
    glfwSetScrollCallback(m_window, Mouse::MouseScrollCallback);
    glfwSetMouseButtonCallback(m_window, Mouse::MouseButtonCallback);

    std::cout << glGetString(GL_VERSION) << std::endl;

    std::cout << "window created" << std::endl;
    
#ifndef IMGUI_DISABLE
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

    ImFontConfig fontConfig;
#if 0
    fontConfig.SizePixels = 40.0f;
#else
    fontConfig.SizePixels = 20.0f;
#endif
    ImGui::GetIO().Fonts->AddFontDefault(&fontConfig);

    std::cout << "ImGui context created" << std::endl;
#endif
    return true;
}

void Window::NewFrame()
{
    glfwPollEvents();

#ifndef IMGUI_DISABLE
    ImGui_ImplGlfw_NewFrame();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();
#endif
    glClear(GL_COLOR_BUFFER_BIT);
    //glClear(GL_DEPTH_BUFFER_BIT);
}

void Window::EndFrame()
{
#ifndef IMGUI_DISABLE
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }
#endif
    glfwSwapBuffers(m_window);
}

void Window::SetWindowPos(int x, int y)
{
    // retrieve menu bar height
    int wndLeft, wndTop, wndRight, wndBottom;
    glfwGetWindowFrameSize(m_window, &wndLeft, &wndTop, &wndRight, &wndBottom);
    int menuBarWidth = wndTop;

    glfwSetWindowMonitor(m_window, nullptr, x, y + menuBarWidth, GetWidth(), GetHeight(), GLFW_DONT_CARE);
}

int Window::GetWidth()
{
	return x;
}

int Window::GetHeight()
{
	return y;
}

bool Window::GetIsResized()
{
    bool ret = resized;
    resized = false;
    return ret;
}

float Window::GetRatio()
{
    return ratio;
}

bool Window::GetShouldClose()
{
    return glfwWindowShouldClose(m_window);
}

void OnResize(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    resized = true;
	x = width;
	y = height;
    ratio = (float)y / (float)x;
}

void OnContentScaled(GLFWwindow* window, float width, float height)
{
    printf("hi");
}