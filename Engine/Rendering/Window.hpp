#pragma once

#include "Main/CoreInclude.hpp"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace Pit::Rendering {
	class Window {
	public:
		Window(const std::string& title, int width, int height);
		~Window();

		static void UpdateAllWindows();
		bool ShouldClose();

		void CreateVKSurface(VkInstance instance, VkSurfaceKHR* surface);
		int GetWidth() { return m_Width; }
		int GetHeight() { return m_Height; }
		GLFWwindow* GetWindowHandle() { return m_Window; }
		bool IsMinimized() { return m_Width <= 0 || m_Height <= 0; }

	private:
		GLFWwindow* m_Window = nullptr;
		std::string m_Title;
		int m_Width, m_Height;
	};
}