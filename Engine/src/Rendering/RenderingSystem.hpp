#pragma once

#include "Core/CoreInclude.hpp"
#include "Texture.hpp"
#include "Shader.hpp"
#include "ECS/ECSScene.hpp"

namespace Pit::Rendering {
	class RenderingSystem {
	public:

		static void Setup();
		static void RenderScene(ECS::Scene& scene);
		static void Shutdown();

	private:
		inline static glm::vec3 lightColor{ 1.0f, 1.0f, 1.0f };
		inline static glm::vec3 lightPos{ 1.2f, 1.0f, 2.0f };

		inline static unsigned int VBO, cubeVAO;
		inline static unsigned int lightCubeVAO;
		inline static Shader* lightingShader;
		inline static Shader* lightCubeShader;
	};
}