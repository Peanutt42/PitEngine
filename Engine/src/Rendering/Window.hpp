#pragma once

#include "Core/CoreInclude.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#pragma warning(push)
#pragma warning(disable: 4201)
#include <glm/glm.hpp>
#pragma warning(pop)

namespace Pit::Rendering {
	class Window {
	public:
		Window(const String& title, int width, int height, bool fullscreen = true);
		~Window();

		void Update();
		bool ShouldClose();

		void SetViewport(int width, int height);

		int GetWidth() { return m_Width; }
		int GetHeight() { return m_Height; }
		GLFWwindow* GetWindowHandle() { return m_Window; }
		bool IsMaximized() { return glfwGetWindowAttrib(m_Window, GLFW_MAXIMIZED) == GLFW_TRUE; }
		void SetMaximized(bool maximized) { return glfwSetWindowAttrib(m_Window, GLFW_MAXIMIZED, maximized ? GLFW_TRUE : GLFW_FALSE); }
		bool IsMinimized() { return m_Width <= 0 || m_Height <= 0; }
		bool IsFocused() { return glfwGetWindowAttrib(m_Window, GLFW_FOCUSED) == GLFW_TRUE; }
		bool IsHovered() { return glfwGetWindowAttrib(m_Window, GLFW_HOVERED) == GLFW_TRUE; }

		void GetPosition(float& x, float& y) { int _x, _y; glfwGetWindowPos(m_Window, &_x, &_y); x = Cast<float>(_x); y = Cast<float>(_y); }

		void SetIcon(const String& iconFilePath);

		bool WasWindowResized() { return m_FramebufferResized; }
		void SetWindowResizedFlag(bool resized) { m_FramebufferResized = resized; }

	private:
		GLFWwindow* m_Window = nullptr;
		String m_Title;
		int m_Width, m_Height;
		bool m_FramebufferResized = false;

		static void _GLFWErrorCallback(int errorCode, const char* description);
		static void _FramebufferResizedCallback(GLFWwindow* window, int width, int height);
		static void _WindowResizedCallback(GLFWwindow* window, int width, int height);
		static void _WindowPositionCallback(GLFWwindow* window, int xpos, int ypos);
	};
}