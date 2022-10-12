#include "CoreInclude.hpp"
#include "Window.hpp"
#include "Main/Engine.hpp"

using namespace Pit::Rendering;

static bool glfwInited = false;

Window::Window(const std::string& title, int width, int height) 
	: m_Title(title), m_Width(width), m_Height(height) {
	
	if (!glfwInited) {
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
#ifndef PIT_EDITOR
		glfwWindowHint(GLFW_DECORATED, NULL);
#endif
		glfwInited = true;
	}

	GLFWmonitor* primaryMonitor = nullptr;
#ifndef PIT_EDITOR
	primaryMonitor = glfwGetPrimaryMonitor();
	m_Width = glfwGetVideoMode(primaryMonitor)->width;
	m_Height = glfwGetVideoMode(primaryMonitor)->height;
#endif

	m_Window = glfwCreateWindow(m_Width, m_Height, m_Title.c_str(), primaryMonitor, nullptr);

	glfwSetWindowUserPointer(m_Window, this);
	glfwSetFramebufferSizeCallback(m_Window, _FramebufferResizedCallback);
	glfwSetWindowSizeCallback(m_Window, _WindowResizedCallback);
}

Window::~Window() {
	glfwDestroyWindow(m_Window);
	if (glfwInited) {
		glfwTerminate();
		glfwInited = false;
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
		Engine::Instance->Renderer->Update();
	}
}

void Window::_WindowResizedCallback(GLFWwindow* window, int width, int height) {
	auto _window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
	if (_window && !_window->WasWindowResized()) {
		_window->m_FramebufferResized = true;
		_window->m_Width = width;
		_window->m_Height = height;
		Engine::Instance->Renderer->Update();
	}
}