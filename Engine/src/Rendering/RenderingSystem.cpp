#include "Main/CoreInclude.hpp"
#include "Main/Engine.hpp"
#include "RenderingSystem.hpp"
#include "ECS/ECSComponents.hpp"
#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

using namespace Pit::Rendering;

Camera* RenderEntitiesSystem::CameraToUse = nullptr;

void RenderEntitiesSystem::Update(ECS::World& world) {
	if (!CameraToUse) {
		PIT_ENGINE_ERR(Log::Rendering, "No Camera specified to render to (RenderEntitiesSystem::CameraToUse == nullptr)");
		return;
	}

	auto* renderer = Engine::Rendering()->Renderer;
	Engine::Rendering()->RenderingSystem->Pipeline->Bind(renderer->CommandBuffers[renderer->FrameIndex]);

	renderer->TestMesh()->Bind(renderer->CommandBuffers[renderer->FrameIndex]);
	const auto& camProjection = CameraToUse->GetProjection() * CameraToUse->GetView();

	auto group = world.Group<ECS::TransformComponent, ECS::MeshRendererComponent>();
	for (auto e : group) {
		auto& transform = group.get<ECS::TransformComponent>(e);
		auto& mesh = group.get<ECS::MeshRendererComponent>(e);
		/*
		transform.rotation.y = glm::mod(transform.rotation.y + 1.f * Time::DeltaTime(), glm::two_pi<float>());
		transform.rotation.x = glm::mod(transform.rotation.x + 0.5f * Time::DeltaTime(), glm::two_pi<float>());
		transform.rotation.z = glm::mod(transform.rotation.z + 0.2f * Time::DeltaTime(), glm::two_pi<float>());*/
		transform.position = { 0, 0, 2.5f };
		transform.scale = { .5f, .5f, .5f };

		SimplePushConstantData push{};
		auto modelMatrix = transform.mat4();
		push.transform = camProjection * modelMatrix;
		push.normalMatrix = transform.normalMatrix();
		vkCmdPushConstants(renderer->CommandBuffers[renderer->FrameIndex], Engine::Rendering()->RenderingSystem->PipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(SimplePushConstantData), &push);
		renderer->TestMesh()->Draw(renderer->CommandBuffers[renderer->FrameIndex]);
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


	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 0;
	pipelineLayoutInfo.pSetLayouts = nullptr;
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