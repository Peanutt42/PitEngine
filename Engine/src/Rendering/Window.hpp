#pragma once

#include "Main/CoreInclude.hpp"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <stb_image.h>

namespace Pit::Rendering {
	class Window {
	public:
		Window(const std::string& title, int width, int height, bool fullscreen = true);
		~Window();

		static void UpdateAllWindows();
		bool ShouldClose();

		void CreateVKSurface(VkInstance instance, VkSurfaceKHR* surface);
		int GetWidth() { return m_Width; }
		int GetHeight() { return m_Height; }
		bool IsMaximized() { return glfwGetWindowAttrib(m_Window, GLFW_MAXIMIZED) == GLFW_TRUE; }
		void SetMaximized(bool maximized) { return glfwSetWindowAttrib(m_Window, GLFW_MAXIMIZED, maximized ? GLFW_TRUE : GLFW_FALSE); }
		GLFWwindow* GetWindowHandle() { return m_Window; }
		bool IsMinimized() { return m_Width <= 0 || m_Height <= 0; }
		bool IsFocused() { return glfwGetWindowAttrib(m_Window, GLFW_FOCUSED) == GLFW_TRUE; }
		bool IsHovered() { return glfwGetWindowAttrib(m_Window, GLFW_HOVERED) == GLFW_TRUE; }

		bool WasWindowResized() { return m_FramebufferResized; }
		void SetWindowResizedFlag(bool resized) { m_FramebufferResized = resized; }

	private:
		GLFWwindow* m_Window = nullptr;
		std::string m_Title;
		int m_Width, m_Height;
		bool m_FramebufferResized = false;

		static void _GLFWErrorCallback(int errorCode, const char* description);
		static void _FramebufferResizedCallback(GLFWwindow* window, int width, int height);
		static void _WindowResizedCallback(GLFWwindow* window, int width, int height);
	};

	static void SetGLFWWindowIcon(GLFWwindow* window, const std::string& path) {
		int w, h, channels;
		unsigned char* pixels = stbi_load(path.c_str(), &w, &h, &channels, 4);
		GLFWimage images[1];
		images[0].width = w;
		images[0].height = h;
		images[0].pixels = pixels;
		glfwSetWindowIcon(window, 1, images);
	}
}