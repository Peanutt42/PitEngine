#pragma once

#include "Core/CoreInclude.hpp"
#include "Texture.hpp"
#include "Shader.hpp"

namespace Pit::Rendering {
	class RenderingSystem {
	public:

		void Setup();
		void Render();

	private:
		uint m_VBO, m_VAO, m_EBO;
		Texture m_Texture = Texture(FileSystem::GetSandboxDir() + "assets/textures/wall.jpg");
		Shader m_Shader = Shader(FileSystem::GetSandboxDir() + "assets/shaders/Shader.vert",
								 FileSystem::GetSandboxDir() + "assets/shaders/Shader.frag");
	};
}