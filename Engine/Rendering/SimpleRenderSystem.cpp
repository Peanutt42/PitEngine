#include "SimpleRenderSystem.hpp"
#include "ECS/Commons/ECSTransform2DComponent.hpp"
#include "ECS/Commons/ECSMeshComponent.hpp"

using namespace Pit::Rendering;

struct SimplePushConstantData {
	glm::mat2 transform{ 1.f };
	glm::vec2 offset;
	alignas(16) glm::vec3 color;
};

SimpleRenderSystem::SimpleRenderSystem(Device& device, VkRenderPass renderPass)
	: m_Device(device) {
	
	_CreatePipelineLayout();
	_CreatePipeline(renderPass);
}

SimpleRenderSystem::~SimpleRenderSystem() {
	vkDestroyPipelineLayout(m_Device.device(), m_PipelineLayout, nullptr);
}

void SimpleRenderSystem::_CreatePipelineLayout() {
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
	if (vkCreatePipelineLayout(m_Device.device(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS)
		PIT_ENGINE_ERR("Failed to create pipeline layout!");
}

void SimpleRenderSystem::_CreatePipeline(VkRenderPass renderPass) {
	assert(m_PipelineLayout != nullptr && "Can't create pipeline before pipelineLayout");

	PipelineConfigInfo pipelineConfig{};
	Pipeline::DefaultConfigInfo(pipelineConfig);
	pipelineConfig.renderPass = renderPass;
	pipelineConfig.pipelineLayout = m_PipelineLayout;
	m_Pipeline = std::make_unique<Pipeline>(
		m_Device,
		"C:/dev/cpp/PitEngine/Sandbox/shaders/simple_shader.vert.spv",
		"C:/dev/cpp/PitEngine/Sandbox/shaders/simple_shader.frag.spv",
		pipelineConfig);
}

void SimpleRenderSystem::RenderEntities(VkCommandBuffer commandBuffer, World* ecsWorld) {
	m_Pipeline->Bind(commandBuffer);

	auto group = ecsWorld->Group<ECS::MeshComponent, ECS::Transform2DComponent>();
	for (auto e : group) {
		auto& mesh = group.get<ECS::MeshComponent>(e);
		auto& transform = group.get<ECS::Transform2DComponent>(e);

		transform.rotation = glm::mod(transform.rotation + 0.01f, glm::two_pi<float>());

		SimplePushConstantData push{};
		push.offset = transform.position;
		push.color = mesh.color;
		push.transform = transform.mat2();

		vkCmdPushConstants(
			commandBuffer,
			m_PipelineLayout,
			VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
			0,
			sizeof(SimplePushConstantData),
			&push);
		mesh.model->Bind(commandBuffer);
		mesh.model->Draw(commandBuffer);
	}
}