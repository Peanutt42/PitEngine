#pragma once

#include "Core/CoreInclude.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Shader.hpp"
#include "Texture.hpp"
#include "Camera.hpp"
#include "RenderingSystem.hpp"
#include "Framebuffer.hpp"

namespace Pit::Rendering {
	/// <summary>
	/// Renders things into texture that may or may not be displayed onto the Window
	/// </summary>
	class Renderer {
	public:
		static void Init();
		static void Shutdown();

		static void Begin();

		static void BeginScene();
		static void EndScene();

		static void End();

		static Framebuffer* GetScreenFramebuffer();

	private:
		inline static uint s_ScreenQuadVAO, s_ScreenQuadVBO;
		inline static Framebuffer* s_Framebuffer;
		inline static Shader* s_ScreenShader;
	};
}