#pragma once

#include "Core/CoreInclude.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Shader.hpp"
#include "Texture.hpp"
#include "Camera.hpp"

namespace Pit::Rendering {
	class Renderer {
	public:
		Renderer();
		~Renderer();

		void Update();

		Camera m_Camera = Camera({0.f, 0.f, 3.f});
	private:

		uint m_VBO, m_VAO, m_EBO;
		//uint m_Texture;
		Shader m_Shader = Shader(FileSystem::GetSandboxDir() + "assets/shaders/Shader.vert",
								 FileSystem::GetSandboxDir() + "assets/shaders/Shader.frag");
		Texture m_Texture = Texture(FileSystem::GetSandboxDir() + "assets/textures/wall.jpg");
	};
}