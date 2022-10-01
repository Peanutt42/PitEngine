#pragma once

#include "Main/CoreInclude.hpp"
#include "Rendering/RenderingInclude.hpp"

namespace Pit::Rendering {
	class Window {
	public:
		Window(uint32_t width, uint32_t height, const std::string& title)
			: m_Width(width), m_Height(height), m_Title(title) {

			glfwInit();

			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
			glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

			m_Window = glfwCreateWindow(m_Width, m_Height, m_Title.c_str(), nullptr, nullptr);
			if (!m_Window)
				PIT_ENGINE_ERR("window null");
		}

		~Window() {
			glfwDestroyWindow(m_Window);
			glfwTerminate();
		}

		inline void Update() {
			glfwSwapBuffers(m_Window);
		}

		inline bool ShouldClose() { return glfwWindowShouldClose(m_Window); }

		void CreateWindowSurface(VkInstance instance, VkSurfaceKHR* surface) {
			if (glfwCreateWindowSurface(instance, m_Window, nullptr, surface) != VK_SUCCESS) {
				throw std::runtime_error("failed to create window surface!");
			}
		}

		inline VkExtent2D GetExtent() { return { m_Width, m_Height }; }

		inline GLFWwindow* GetWindowHandle() { return m_Window; }
		inline uint32_t GetWidth() { return m_Width; }
		inline uint32_t GetHeight() { return m_Height; }

	private:
		uint32_t m_Width, m_Height;
		std::string m_Title;
		GLFWwindow* m_Window;
	};
}