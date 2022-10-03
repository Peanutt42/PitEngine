#pragma once

#include "Rendering/RenderingInclude.hpp"

namespace Pit::Rendering {
	class Renderer;

	class Window {
	public:
		Window(uint32_t width, uint32_t height, const std::string& title);

		~Window();

		inline void Update() {
			glfwSwapBuffers(m_Window);
		}

		static void UpdateWidnows() {
			for (size_t i = 0; i < m_PitsWindows.size(); i++)
				if (glfwGetWindowAttrib(m_PitsWindows[i], GLFW_FOCUSED))
					CurrentlySelectedWindow = m_PitsWindows[i];
		}
		static GLFWwindow* CurrentlySelectedWindow;

		inline void SetRenderer(Renderer* renderer) { m_Renderer = renderer; }
		inline bool ShouldClose() { return glfwWindowShouldClose(m_Window); }
		inline bool WasResized() { return m_Resized; }
		inline void ResetResizedFlag() { m_Resized = false; }
		inline void CreateWindowSurface(VkInstance instance, VkSurfaceKHR* surface) {
			if (glfwCreateWindowSurface(instance, m_Window, nullptr, surface) != VK_SUCCESS) PIT_ENGINE_ERR("Failed to create window surface!");
		}
		inline VkExtent2D GetExtent() { return { m_Width, m_Height }; }
		inline GLFWwindow* GetWindowHandle() { return m_Window; }
		inline uint32_t GetWidth() { return m_Width; }
		inline uint32_t GetHeight() { return m_Height; }

	private:
		static void _FramebufferResizeCallback(GLFWwindow* window, int width, int height);
		static void _WindowSetPosCallback(GLFWwindow* window, int posx, int posy);
		static std::vector<GLFWwindow*> m_PitsWindows;
		uint32_t m_Width, m_Height;
		bool m_Resized = false;
		std::string m_Title;
		GLFWwindow* m_Window;
		Renderer* m_Renderer;
	};
}