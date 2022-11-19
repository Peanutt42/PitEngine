#pragma once

#include "Core/CoreInclude.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Shader.hpp"
#include "Texture.hpp"
#include "Camera.hpp"

namespace Pit::Rendering {
	/// <summary>
	/// Renders things into texture that may or may not be displayed onto the Window
	/// </summary>
	class Renderer {
	public:
		Renderer();
		~Renderer();

		void Update();

		Camera m_Camera = Camera({0.f, 0.f, 3.f});
		uint GetScreenTexColorBuffer() { return m_ScreenTexColorBuffer; }
	private:

		uint m_VBO, m_VAO, m_EBO;
		uint m_ScreenQuadVAO, m_ScreenQuadVBO;
		uint m_ScreenTexColorBuffer;
		uint m_ScreenFramebuffer;
		//uint m_Texture;
		Shader m_Shader = Shader(FileSystem::GetSandboxDir() + "assets/shaders/Shader.vert",
								 FileSystem::GetSandboxDir() + "assets/shaders/Shader.frag");
		Shader m_ScreenShader = Shader(FileSystem::GetEngineDir() + "assets/shaders/ScreenShader.vert",
								 FileSystem::GetEngineDir() + "assets/shaders/ScreenShader.frag");
		Texture m_Texture = Texture(FileSystem::GetSandboxDir() + "assets/textures/wall.jpg");
	};
}