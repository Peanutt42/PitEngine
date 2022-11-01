#include "pch.hpp"
#include "Core/Engine.hpp"
#include "Window.hpp"

using namespace Pit::Rendering;

Window::Window(const String& title, int width, int height, bool fullscreen)
	: m_Title(title), m_Width(width), m_Height(height) {

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef PIT_APPLE
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	glfwWindowHint(GLFW_MAXIMIZED, fullscreen ? GLFW_TRUE : GLFW_FALSE);

	GLFWmonitor* primaryMonitor = nullptr;
	if (!Engine::GetInfo().WindowToolbar) {
		glfwWindowHint(GLFW_AUTO_ICONIFY, GLFW_FALSE); // force window on primary screen
		primaryMonitor = glfwGetPrimaryMonitor();
		m_Width = glfwGetVideoMode(primaryMonitor)->width;
		m_Height = glfwGetVideoMode(primaryMonitor)->height;
	}

	m_Window = glfwCreateWindow(m_Width, m_Height, m_Title.c_str(), primaryMonitor, nullptr);
	if (!m_Window)
		PIT_ENGINE_FATAL(Log::Rendering, "Failed to create GLFW-Window");

	if (glfwRawMouseMotionSupported())
		glfwSetInputMode(m_Window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

	glfwSetWindowUserPointer(m_Window, this);
	glfwSetFramebufferSizeCallback(m_Window, _FramebufferResizedCallback);
	glfwSetWindowSizeCallback(m_Window, _WindowResizedCallback);
	glfwSetWindowSizeLimits(m_Window, 200, 200, 9999999, 9999999);
}

Window::~Window() {
	glfwDestroyWindow(m_Window);
}

void Window::Update() {
	glfwSwapBuffers(m_Window);
	glfwPollEvents();
}

bool Window::ShouldClose() {
	return glfwWindowShouldClose(m_Window);
}

void Window::SetViewport(int width, int height) {
	m_Width = width;
	m_Height = height;

	glViewport(0, 0, m_Width, m_Height);
}

void Window::SetIcon(const String& iconFilePath) {
	int w, h, channels;
	unsigned char* pixels = stbi_load(iconFilePath.c_str(), &w, &h, &channels, 4);
	GLFWimage images[1]{ {w, h, pixels} };
	glfwSetWindowIcon(m_Window, 1, images);
}


void Window::_GLFWErrorCallback(int errorCode, const char* description) {
	PIT_ENGINE_ERR(Log::Rendering, "[GLFW]: {0:d}: {1:s}", errorCode, description);
}

void Window::_FramebufferResizedCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	auto _window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
	if (_window && !_window->WasWindowResized()) {
		_window->m_FramebufferResized = true;
		_window->m_Width = width;
		_window->m_Height = height;
	}
	Engine::Rendering()->Renderer->Update();
}

void Window::_WindowResizedCallback(GLFWwindow* window, int width, int height) {
	auto _window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
	glViewport(0, 0, width, height);
	if (_window && !_window->WasWindowResized()) {
		_window->m_FramebufferResized = true;
		_window->m_Width = width;
		_window->m_Height = height;
	}
	Engine::Rendering()->Renderer->Update();
}