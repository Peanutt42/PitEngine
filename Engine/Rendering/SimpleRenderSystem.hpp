#pragma once

#include "Rendering/RenderingInclude.hpp"
#include "Rendering/Vulkan/VulkanPipeline.hpp"
#include "ECS/ECSSubsystem.hpp"
#include "ECS/Examples/ECSExampleMovement.hpp"

namespace Pit::Rendering {

	using namespace Rendering;
	using namespace Vulkan;
	using namespace ECS;

	class SimpleRenderSystem {
	public:
		SimpleRenderSystem(Device& device, VkRenderPass renderPass);
		~SimpleRenderSystem();

		void RenderEntities(VkCommandBuffer commandBuffer, World* ecsWorld);
	
	private:
		Device& m_Device;
		std::unique_ptr<Pipeline> m_Pipeline;
		VkPipelineLayout m_PipelineLayout;

		void _CreatePipelineLayout();
		void _CreatePipeline(VkRenderPass renderPass);
	};
}