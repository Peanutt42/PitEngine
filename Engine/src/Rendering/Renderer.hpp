#pragma once

#include "Core/CoreInclude.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Shader.hpp"
#include "Texture.hpp"
#include "Camera.hpp"
#include "RenderingSystem.hpp"

namespace Pit::Rendering {
	/// <summary>
	/// Renders things into texture that may or may not be displayed onto the Window
	/// </summary>
	class Renderer {
	public:
		Renderer();
		~Renderer();

		void Update();

		uint GetScreenTexColorBuffer() { return m_ScreenTexColorBuffer; }
	private:
		RenderingSystem m_RenderingSystem;
		uint m_ScreenQuadVAO, m_ScreenQuadVBO;
		uint m_ScreenTexColorBuffer;
		uint m_ScreenFramebuffer;
		Shader m_ScreenShader = Shader(FileSystem::GetEngineDir() + "assets/shaders/ScreenShader.vert",
								 FileSystem::GetEngineDir() + "assets/shaders/ScreenShader.frag");
	};
}