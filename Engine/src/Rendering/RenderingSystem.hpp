#pragma once

#include "Core/CoreInclude.hpp"
#include "Texture.hpp"
#include "Shader.hpp"

namespace Pit::Rendering {
#define TEST1 1
#define TEST2 2
#define TEST TEST2
	class RenderingSystem {
	public:

		void Setup();
		void Render();

	private:
#if TEST == TEST1
		Texture m_Texture = Texture(FileSystem::GetSandboxDir() + "assets/textures/wall.jpg");
#elif TEST == TEST2
		glm::vec3 lightColor{ 1.0f, 1.0f, 1.0f };
		glm::vec3 lightPos{ 1.2f, 1.0f, 2.0f };

		unsigned int VBO, cubeVAO;
		unsigned int lightCubeVAO;
		Shader lightingShader { FileSystem::GetSandboxDir() + "assets/shaders/BasicLight.vert",
								FileSystem::GetSandboxDir() + "assets/shaders/BasicLight.frag" };
		Shader lightCubeShader { FileSystem::GetSandboxDir() + "assets/shaders/LightCube.vert",
								 FileSystem::GetSandboxDir() + "assets/shaders/LightCube.frag" };
#endif
	};
}