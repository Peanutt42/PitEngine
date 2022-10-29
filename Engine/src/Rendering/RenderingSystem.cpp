#include "pch.hpp"
#include "Core/Engine.hpp"
#include "RenderingSystem.hpp"
#include "ECS/ECSComponents.hpp"
#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

using namespace Pit::Rendering;

Camera* RenderEntitiesSystem::CameraToUse = nullptr;

struct RenderEntityEntry {
	Pit::ECS::TransformComponent* transform;
};

struct RenderInstancedEntry {
	Pit::Rendering::Mesh* mesh;
	std::vector<RenderEntityEntry> entries;

	RenderInstancedEntry(Pit::Rendering::Mesh* mesh)
		: mesh(mesh), entries(std::vector<RenderEntityEntry>()) {

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

	std::vector<RenderInstancedEntry> renderEntries {
		{Engine::Rendering()->Renderer->VaseMesh().get()},
		{Engine::Rendering()->Renderer->QuadMesh().get()}
	};

	auto group = world.Group<ECS::TransformComponent, ECS::MeshRendererComponent>();
	for (auto e : group) {
		auto& transform = group.get<ECS::TransformComponent>(e);
		auto& mesh = group.get<ECS::MeshRendererComponent>(e);
		
		//transform.position = { .5f, .5f, 0.f };
		//transform.scale = { 3.f, 1.5f, 3.f };
		for (int i = 0; i < renderEntries.size(); i++)
			if (renderEntries[i].mesh == mesh.Mesh)
				renderEntries[i].entries.emplace_back(&transform);
	}

	for (auto& renderEntry : renderEntries) {
		renderEntry.mesh->Bind(commandBuffer);
		for (auto& renderEntityEntry : renderEntry.entries) {
			SimplePushConstantData push{};
			push.modelMatrix = renderEntityEntry.transform->mat4();
			push.normalMatrix = renderEntityEntry.transform->normalMatrix();
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
	VkPushConstantRange pushConstantRange{};
	pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
	pushConstantRange.offset = 0;
	pushConstantRange.size = sizeof(SimplePushConstantData);

	std::vector<VkDescriptorSetLayout> discriptorSetLayouts {
		Engine::Rendering()->Renderer->GlobalSetLayout->getDescriptorSetLayout()
	};

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(discriptorSetLayouts.size());
	pipelineLayoutInfo.pSetLayouts = discriptorSetLayouts.data();
	pipelineLayoutInfo.pushConstantRangeCount = 1;
	pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
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