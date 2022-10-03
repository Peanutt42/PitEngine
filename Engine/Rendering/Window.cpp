#include "Window.hpp"
#include "Renderer.hpp"
#include "Main/Engineloop.hpp"

using namespace Pit::Rendering;

std::vector<GLFWwindow*> Window::m_PitsWindows = std::vector<GLFWwindow*>();
GLFWwindow* Window::CurrentlySelectedWindow = nullptr;

Window::Window(uint32_t width, uint32_t height, const std::string& title)
	: m_Width(width), m_Height(height), m_Title(title) {

	if (m_PitsWindows.size() <= 0)
		glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	glfwWindowHint(GLFW_DECORATED, GLFW_TRUE); // titlebar

	m_Window = glfwCreateWindow(m_Width, m_Height, m_Title.c_str(), nullptr, nullptr);
	m_PitsWindows.push_back(m_Window);
	glfwSetWindowUserPointer(m_Window, this);
	glfwSetFramebufferSizeCallback(m_Window, _FramebufferResizeCallback);
	glfwSetWindowSizeCallback(m_Window, _FramebufferResizeCallback);
	glfwSetWindowPosCallback(m_Window, _WindowSetPosCallback);
}

Window::~Window() {
	glfwDestroyWindow(m_Window);
	for (size_t i = 0; i < m_PitsWindows.size(); i++) {
		if (m_PitsWindows[i] == m_Window) {
			m_PitsWindows.erase(m_PitsWindows.begin() + i);
			break;
		}
	}
	if (m_PitsWindows.size() <= 0)
		glfwTerminate();
}

void Window::_FramebufferResizeCallback(GLFWwindow* window, int width, int height) {
	auto _window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
	_window->m_Resized = true;
	_window->m_Width = static_cast<uint32_t>(width);
	_window->m_Height = static_cast<uint32_t>(height);
	Engineloop::Instance->Update();
}

void Window::_WindowSetPosCallback(GLFWwindow* window, int posx, int posy) {
	Engineloop::Instance->Update();
}