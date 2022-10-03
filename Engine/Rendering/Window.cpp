#include "Window.hpp"
#include "Renderer.hpp"
#include "Main/Engineloop.hpp"

using namespace Pit::Rendering;

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