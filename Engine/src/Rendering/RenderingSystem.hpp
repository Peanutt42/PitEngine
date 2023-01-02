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
		glm::vec3 lightColor{ 1.0f, 1.0f, 1.0f };
		glm::vec3 lightPos{ 1.2f, 1.0f, 2.0f };

		unsigned int VBO, cubeVAO;
		unsigned int lightCubeVAO;
		Shader lightingShader { FileSystem::GetSandboxDir() + "assets/shaders/BasicLight.shader" };
		Shader lightCubeShader { FileSystem::GetSandboxDir() + "assets/shaders/LightCube.shader" };
	};
}