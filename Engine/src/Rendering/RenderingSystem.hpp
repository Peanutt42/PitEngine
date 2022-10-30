#pragma once

#include "Core/CoreInclude.hpp"
#include "ECS/ECSSystem.hpp"
#include "ECS/ECSWorld.hpp"
#include "Camera.hpp"

struct SimplePushConstantData {
	glm::mat4 modelMatrix{ 1.f };
	glm::mat4 normalMatrix{ 1.f };
};

namespace Pit::Rendering {
	class RenderEntitiesSystem {
	public:
		static ECS::SystemTopic GetTopic() { return ECS::SystemTopic::Render; }
		static int32 GetExecuteOrder() { return 0; }
		static void Update(ECS::World& world);

		static Camera* CameraToUse;
	};

	class RenderingSystem {
	public:
		RenderingSystem();
		~RenderingSystem();

		void CreatePipelineLayout();
		void CreatePipeline(VkRenderPass renderPass);

		ScopeRef<Rendering::Pipeline> Pipeline;
		VkPipelineLayout PipelineLayout;
	};
}