#pragma once

#include "Core/CoreInclude.hpp"
#include <glad/glad.h>
#include "Shader.hpp"
#include "Texture.hpp"

namespace Pit::Rendering {
	class Renderer {
	public:
		Renderer();
		~Renderer();

		void Update();

	private:
		uint m_VBO, m_VAO, m_EBO;
		//uint m_Texture;
		Shader m_Shader = Shader(FileSystem::GetSandboxDir() + "assets/shaders/Shader.vert",
								 FileSystem::GetSandboxDir() + "assets/shaders/Shader.frag");
		Texture m_Texture = Texture(FileSystem::GetSandboxDir() + "assets/textures/wall.jpg");
	};
}