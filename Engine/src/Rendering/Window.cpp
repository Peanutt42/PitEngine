#include "pch.hpp"
#include "Core/Engine.hpp"
#include "Rendering/RenderingSubmodule.hpp"
#include "Window.hpp"

namespace Pit::Rendering {

	Window::Window(const String& title, int width, int height, bool fullscreen)
		: m_Title(title), m_Width(width), m_Height(height) {

		PIT_PROFILE_FUNCTION();

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef PIT_APPLE
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

		glfwWindowHint(GLFW_MAXIMIZED, fullscreen ? GLFW_TRUE : GLFW_FALSE);

		glfwWindowHint(GLFW_SAMPLES, 0);// Weil wir die scene eh in einen seperaten framebuffer render (f�r post-processing) Engine::GetSettings().AntiAliasing);

		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);

		GLFWmonitor* primaryMonitor = nullptr;
		if (!Engine::GetSettings().WindowToolbar) {
			glfwWindowHint(GLFW_AUTO_ICONIFY, GLFW_FALSE);
			primaryMonitor = glfwGetPrimaryMonitor(); // Force window on primary screen
		}

		if (fullscreen) {
			m_Width = glfwGetVideoMode(glfwGetPrimaryMonitor())->width;
			m_Height = glfwGetVideoMode(glfwGetPrimaryMonitor())->height;
		}

		m_Window = glfwCreateWindow(m_Width, m_Height, m_Title.c_str(), primaryMonitor, nullptr);
		if (!m_Window)
			PIT_ENGINE_FATAL(Rendering, "Failed to create GLFW-Window");

		if (glfwRawMouseMotionSupported())
			glfwSetInputMode(m_Window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

		glfwSetWindowUserPointer(m_Window, this);
		glfwSetFramebufferSizeCallback(m_Window, _FramebufferResizedCallback);
		glfwSetWindowSizeCallback(m_Window, _WindowResizedCallback);
		glfwSetWindowPosCallback(m_Window, _WindowPositionCallback);
		glfwSetWindowSizeLimits(m_Window, 200, 200, GLFW_DONT_CARE, GLFW_DONT_CARE);
	}

	Window::~Window() {
		PIT_PROFILE_FUNCTION();

		glfwDestroyWindow(m_Window);
	}

	void Window::Update() {
		PIT_PROFILE_FUNCTION();

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

	int Window::GetWidth() { return m_Width; }
	int Window::GetHeight() { return m_Height; }

	GLFWwindow* Window::GetWindowHandle() { return m_Window; }

	void Window::SetMaximized(bool maximized) { return glfwSetWindowAttrib(m_Window, GLFW_MAXIMIZED, maximized ? GLFW_TRUE : GLFW_FALSE); }

	bool Window::IsMaximized() { return glfwGetWindowAttrib(m_Window, GLFW_MAXIMIZED) == GLFW_TRUE; }
	bool Window::IsMinimized() { return m_Width <= 0 || m_Height <= 0; }
	bool Window::IsFocused() { return glfwGetWindowAttrib(m_Window, GLFW_FOCUSED) == GLFW_TRUE; }
	bool Window::IsHovered() { return glfwGetWindowAttrib(m_Window, GLFW_HOVERED) == GLFW_TRUE; }

	void Window::GetPosition(float& x, float& y) {
		int _x, _y;
		glfwGetWindowPos(m_Window, &_x, &_y);
		x = Cast<float>(_x);
		y = Cast<float>(_y);
	}

	void Window::SetIcon(const String& iconFilePath) {
		int w, h, channels;
		stbi_set_flip_vertically_on_load(iconFilePath.ends_with(".jpg") ? 1 : 0);
		unsigned char* pixels = stbi_load(iconFilePath.c_str(), &w, &h, &channels, 4);
		GLFWimage images[1]{ {w, h, pixels} };
		glfwSetWindowIcon(m_Window, 1, images);
		stbi_image_free(pixels);
	}

	bool Window::WasWindowResized() { return m_FramebufferResized; }
	void Window::SetWindowResizedFlag(bool resized) { m_FramebufferResized = resized; }


	void Window::_GLFWErrorCallback([[maybe_unused]] int errorCode, [[maybe_unused]] const char* description) {
		PIT_ENGINE_ERR(Rendering, "[GLFW]: {0:d}: {1:s}", errorCode, description);
	}

	void Window::_FramebufferResizedCallback(GLFWwindow* window, int width, int height) {
		glViewport(0, 0, width, height);
		auto _window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
		if (_window && !_window->WasWindowResized()) {
			_window->m_FramebufferResized = true;
			_window->m_Width = width;
			_window->m_Height = height;
		}
		Engine::Update();
	}

	void Window::_WindowResizedCallback(GLFWwindow* window, int width, int height) {
		auto _window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
		glViewport(0, 0, width, height);
		if (_window && !_window->WasWindowResized()) {
			_window->m_FramebufferResized = true;
			_window->m_Width = width;
			_window->m_Height = height;
		}
		Engine::Update();
	}

	void Window::_WindowPositionCallback(GLFWwindow* /*window*/, int /*xpos*/, int /*ypos*/) {
		Engine::Update();
	}
}