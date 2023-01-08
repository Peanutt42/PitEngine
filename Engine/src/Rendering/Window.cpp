#include "pch.hpp"
#include "Rendering/Window.hpp"
#include "Core/Engine.hpp"
#include "Window.hpp"

namespace Pit::Rendering {

	static void GLFWErrorCallback(int error, const char* description) {
		PIT_ENGINE_ERR(Rendering, "GLFW Error ({0}): {1}", error, description);
	}

	Window::Window(const String& title, int width, int height, bool fullscreen)
		: m_Title(title), m_Width(width), m_Height(height) {

		PIT_PROFILE_FUNCTION();

		if (s_WindowCount == 0) {
			glfwInit();
			glfwSetErrorCallback(GLFWErrorCallback);
		}
		s_WindowCount++;

#ifdef PIT_APPLE
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

		glfwWindowHint(GLFW_MAXIMIZED, fullscreen);

		glfwWindowHint(GLFW_SAMPLES, 0);// Weil wir die scene eh in einen seperaten framebuffer render (für post-processing) Engine::GetSettings().AntiAliasing);

		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);

		GLFWmonitor* primaryMonitor = nullptr;
		if (!Engine::GetSettings().WindowToolbar) {
			glfwWindowHint(GLFW_AUTO_ICONIFY, GLFW_FALSE);
			glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
		}

		if (fullscreen) {
			primaryMonitor = glfwGetPrimaryMonitor(); // Force window on primary screen
			m_Width = glfwGetVideoMode(primaryMonitor)->width;
			m_Height = glfwGetVideoMode(primaryMonitor)->height;
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
		glfwSetWindowMaximizeCallback(m_Window, _WindowMaximizeCallback);
	}

	Window::~Window() {
		PIT_PROFILE_FUNCTION();

		s_WindowCount--;

		glfwDestroyWindow(m_Window);

		if (s_WindowCount == 0)
			glfwTerminate();
	}

	void Window::Update() {
		PIT_PROFILE_FUNCTION();

		{
			PIT_PROFILE_FUNCTION("Pit::Rendering::Window::Update::PollEvents");
			glfwPollEvents();
		}
		{
			PIT_PROFILE_FUNCTION("Pit::Rendering::Window::Update::SwapBuffers");
			glfwSwapBuffers(m_Window);
		}
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

	bool Window::IsMaximized() { return m_Maximized; }
	bool Window::IsMinimized() { return m_Minimized; }
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
		PIT_ENGINE_ASSERT(Rendering, std::filesystem::exists(iconFilePath), "IconFile '{}' doesn't exist!", iconFilePath);
		stbi_set_flip_vertically_on_load(iconFilePath.ends_with(".jpg"));
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
		if (_window) {
			_window->m_FramebufferResized = true;
			_window->m_Width = width;
			_window->m_Height = height;
		}
		Engine::Update();
	}

	void Window::_WindowResizedCallback(GLFWwindow* window, int width, int height) {
		auto _window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
		glViewport(0, 0, width, height);
		if (_window) {
			_window->m_FramebufferResized = true;
			_window->m_Width = width;
			_window->m_Height = height;
		}
		Engine::Update();
	}

	void Window::_WindowPositionCallback(GLFWwindow* /*window*/, int /*xpos*/, int /*ypos*/) {
		Engine::Update();
	}
	
	void Window::_WindowMaximizeCallback(GLFWwindow* window, int maximized) {
		auto _window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
		if (_window) {
			_window->m_Maximized = maximized == 1;
			_window->m_Minimized = maximized == 0;
		}
	}
}