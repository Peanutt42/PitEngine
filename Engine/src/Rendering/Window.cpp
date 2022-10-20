#include "Main/CoreInclude.hpp"
#include "Window.hpp"
#include "Main/Engine.hpp"

using namespace Pit::Rendering;

static bool glfwInitialized = false;

Window::Window(const std::string& title, int width, int height, bool fullscreen)
	: m_Title(title), m_Width(width), m_Height(height) {

	if (!glfwInitialized) {
		glfwInit();
		glfwInitialized = true;
	}
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
	GLFWmonitor* primaryMonitor = nullptr;
	if (Engine::GetInfo().WindowToolbar) {
		glfwWindowHint(GLFW_DECORATED, NULL);
		primaryMonitor = glfwGetPrimaryMonitor();
		m_Width = glfwGetVideoMode(primaryMonitor)->width;
		m_Height = glfwGetVideoMode(primaryMonitor)->height;
	}

	m_Window = glfwCreateWindow(m_Width, m_Height, m_Title.c_str(), primaryMonitor, nullptr);

	glfwSetWindowUserPointer(m_Window, this);
	glfwSetFramebufferSizeCallback(m_Window, _FramebufferResizedCallback);
	glfwSetWindowSizeCallback(m_Window, _WindowResizedCallback);
	glfwSetWindowSizeLimits(m_Window, 200, 200, 9999999, 9999999);
}

Window::~Window() {
	glfwDestroyWindow(m_Window);
	if (glfwInitialized) {
		glfwTerminate();
		glfwInitialized = false;
	}
}

void Window::UpdateAllWindows() {
	glfwPollEvents();
}

bool Window::ShouldClose() {
	return glfwWindowShouldClose(m_Window);
}

void Window::CreateVKSurface(VkInstance instance, VkSurfaceKHR* surface) {
	if (glfwCreateWindowSurface(instance, m_Window, nullptr, surface) != VK_SUCCESS)
		PIT_ENGINE_ERR(Log::Rendering, "Failed to create Vulkan surface!");
}


void Window::_FramebufferResizedCallback(GLFWwindow* window, int width, int height) {
	auto _window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
	if (_window && !_window->WasWindowResized()) {
		_window->m_FramebufferResized = true;
		_window->m_Width = width;
		_window->m_Height = height;
		if (Engine::Exists())
			Engine::Rendering()->GetRenderer()->Update();
	}
}

void Window::_WindowResizedCallback(GLFWwindow* window, int width, int height) {
	auto _window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
	if (_window && !_window->WasWindowResized()) {
		_window->m_FramebufferResized = true;
		_window->m_Width = width;
		_window->m_Height = height;
		if (Engine::Exists())
			Engine::Rendering()->GetRenderer()->Update();
	}
}