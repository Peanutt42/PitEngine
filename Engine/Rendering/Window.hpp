#pragma once

#include "Rendering/RenderingInclude.hpp"

namespace Pit::Rendering {
	class Window {
	public:
		Window(uint32_t width, uint32_t height, const std::string& title)
			: m_Width(width), m_Height(height), m_Title(title) {

			glfwInit();

			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
			glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

			m_Window = glfwCreateWindow(m_Width, m_Height, m_Title.c_str(), nullptr, nullptr);
			glfwSetWindowUserPointer(m_Window, this);
			glfwSetFramebufferSizeCallback(m_Window, _FramebufferResizeCallback);
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
		inline bool WasResized() { return m_Resized; }
		inline void ResetResizedFlag() { m_Resized = false; }
		inline void CreateWindowSurface(VkInstance instance, VkSurfaceKHR* surface) {
			if (glfwCreateWindowSurface(instance, m_Window, nullptr, surface) != VK_SUCCESS) {
				throw std::runtime_error("failed to create window surface!");
			}
		}

		inline VkExtent2D GetExtent() { return { m_Width, m_Height }; }

		inline GLFWwindow* GetWindowHandle() { return m_Window; }
		inline uint32_t GetWidth() { return m_Width; }
		inline uint32_t GetHeight() { return m_Height; }

	private:
		static void _FramebufferResizeCallback(GLFWwindow* window, int width, int height) {
			auto _window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
			_window->m_Resized = true;
			_window->m_Width = static_cast<uint32_t>(width);
			_window->m_Height = static_cast<uint32_t>(height);
		}
		uint32_t m_Width, m_Height;
		bool m_Resized = false;
		std::string m_Title;
		GLFWwindow* m_Window;
	};
}