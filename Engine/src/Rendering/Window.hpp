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
	/// <summary>
	/// Helper class to handle Windows
	/// </summary>
	class Window {
	public:
		Window(const String& title, int width, int height, bool fullscreen = true);
		~Window();

		void Update();
		bool ShouldClose();

		void SetViewport(int width, int height);

		int GetWidth(), GetHeight();
		GLFWwindow* GetWindowHandle();
		void SetMaximized(bool maximized);
		bool IsMaximized(), IsMinimized(), IsFocused(), IsHovered();

		void GetPosition(float& x, float& y);

		void SetIcon(const String& iconFilePath);

		bool WasWindowResized();
		void SetWindowResizedFlag(bool resized);

	private:
		GLFWwindow* m_Window = nullptr;
		String m_Title;
		int m_Width, m_Height;
		bool m_Maximized = false, m_Minimized = false;
		bool m_FramebufferResized = false;

		inline static int s_WindowCount;

		static void _GLFWErrorCallback(int errorCode, const char* description);
		static void _FramebufferResizedCallback(GLFWwindow* window, int width, int height);
		static void _WindowResizedCallback(GLFWwindow* window, int width, int height);
		static void _WindowPositionCallback(GLFWwindow* window, int xpos, int ypos);
		static void _WindowMaximizeCallback(GLFWwindow* window, int maximized);
	};
}