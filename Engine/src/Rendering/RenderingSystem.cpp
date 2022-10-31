#include "pch.hpp"
#include "Core/Engine.hpp"
#include "RenderingSystem.hpp"
#include "ECS/ECSComponents.hpp"
#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

using namespace Pit;
using namespace Rendering;

Camera* RenderEntitiesSystem::CameraToUse = nullptr;

struct RenderEntityEntry {
	Pit::ECS::TransformComponent* transform;
};

struct InstancedRenderEntry {
	Pit::Rendering::Mesh* mesh;
	Array<RenderEntityEntry> entries;

	InstancedRenderEntry(Pit::Rendering::Mesh* mesh)
		: mesh(mesh), entries(Array<RenderEntityEntry>()) {

	}
};

void RenderEntitiesSystem::Update(ECS::World& world) {
	if (!CameraToUse) {
		PIT_ENGINE_ERR(Log::Rendering, "No Camera specified to render to (RenderEntitiesSystem::CameraToUse == nullptr)");
		return;
	}

	auto* renderer = Engine::Rendering()->Renderer;
	auto commandBuffer = renderer->CommandBuffers[renderer->FrameIndex];

	Engine::Rendering()->RenderingSystem->Pipeline->Bind(commandBuffer);

	vkCmdBindDescriptorSets(commandBuffer,
							VK_PIPELINE_BIND_POINT_GRAPHICS,
							Engine::Rendering()->RenderingSystem->PipelineLayout,
							0,
							1,
							&renderer->GlobalDescriptorSets[renderer->FrameIndex],
							0,
							nullptr);

	Array<InstancedRenderEntry> renderEntries {
		{Engine::Rendering()->Renderer->CubeMesh().get()}
	};

	auto group = world.Group<ECS::TransformComponent, ECS::MeshRendererComponent>();
	for (auto e : group) {
		auto& transform = group.get<ECS::TransformComponent>(e);
		auto& mesh = group.get<ECS::MeshRendererComponent>(e);
		
		for (int i = 0; i < renderEntries.size(); i++)
			if (renderEntries[i].mesh == mesh.Mesh)
				renderEntries[i].entries.emplace_back(&transform);
	}

	for (auto& renderEntry : renderEntries) {
		renderEntry.mesh->Bind(commandBuffer);
		for (auto& renderEntityEntry : renderEntry.entries) {
			SimplePushConstantData push {
				.modelMatrix = renderEntityEntry.transform->mat4(),
				.normalMatrix = renderEntityEntry.transform->normalMatrix()
			};
			vkCmdPushConstants(commandBuffer, Engine::Rendering()->RenderingSystem->PipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(SimplePushConstantData), &push);
			renderEntry.mesh->Draw(commandBuffer);
		}
	}
}


static void RecreatePipeline() {
	Pit::Engine::Rendering()->RenderingSystem->CreatePipeline(Pit::Engine::Rendering()->Renderer->SwapChain->getRenderPass());
}

RenderingSystem::RenderingSystem() {
	CreatePipelineLayout();
	CreatePipeline(Engine::Rendering()->Renderer->SwapChain->getRenderPass());
	Engine::OnWindowResizeEvent += RecreatePipeline;
}

RenderingSystem::~RenderingSystem() {
	vkDestroyPipelineLayout(Engine::Rendering()->Renderer->Device.device(), PipelineLayout, nullptr);
}

void RenderingSystem::CreatePipelineLayout() {
	VkPushConstantRange pushConstantRange {
		.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
		.offset = 0,
		.size = sizeof(SimplePushConstantData)
	};

	Array<VkDescriptorSetLayout> discriptorSetLayouts {
		Engine::Rendering()->Renderer->GlobalSetLayout->getDescriptorSetLayout()
	};

	VkPipelineLayoutCreateInfo pipelineLayoutInfo {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		.setLayoutCount = static_cast<uint32>(discriptorSetLayouts.size()),
		.pSetLayouts = discriptorSetLayouts.data(),
		.pushConstantRangeCount = 1,
		.pPushConstantRanges = &pushConstantRange
	};
	if (vkCreatePipelineLayout(Engine::Rendering()->Renderer->Device.device(), &pipelineLayoutInfo, nullptr, &PipelineLayout) != VK_SUCCESS)
		PIT_ENGINE_FATAL(Log::Rendering, "Failed to create pipelineLayout!");
}

void RenderingSystem::CreatePipeline(VkRenderPass renderPass) {
	PipelineConfigInfo pipelineConfig{};
	Pipeline::DefaultConfigInfo(pipelineConfig);
	pipelineConfig.renderPass = renderPass;
	pipelineConfig.pipelineLayout = PipelineLayout;
	Pipeline = std::make_unique<Rendering::Pipeline>(Engine::Rendering()->Renderer->Device,
													 pipelineConfig,
													 FileSystem::GetSandboxDir() + "cache/shaders/vert.spv",
													 FileSystem::GetSandboxDir() + "cache/shaders/frag.spv");

}