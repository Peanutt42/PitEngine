#include "pch.hpp"
#include "Shader.hpp"
#include "Core/Engine.hpp"

using namespace Pit;
using namespace Rendering;

Shader::Shader(size pushConstantDataSize, const String& vertexShaderFile, const String& fragmentShaderFile) {
	_CreatePipelineLayout(pushConstantDataSize);
	_CreatePipeline(Engine::Rendering()->Renderer->SwapChain->getRenderPass(), vertexShaderFile, fragmentShaderFile);
}

Shader::~Shader() {
	vkDestroyPipelineLayout(Engine::Rendering()->Renderer->Device.device(), m_PipelineLayout, nullptr);
}

void Shader::_CreatePipelineLayout(size pushConstantDataSize) {
	VkPushConstantRange pushConstantRange{
		.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
		.offset = 0,
		.size = Cast<uint32>(pushConstantDataSize)
	};

	auto discriptorSetLayout = Engine::Rendering()->Renderer->GlobalSetLayout->getDescriptorSetLayout();

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		.setLayoutCount = 1,
		.pSetLayouts = &discriptorSetLayout,
		.pushConstantRangeCount = 1,
		.pPushConstantRanges = &pushConstantRange
	};
	if (vkCreatePipelineLayout(Engine::Rendering()->Renderer->Device.device(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS)
		PIT_ENGINE_FATAL(Log::Rendering, "Failed to create pipelineLayout!");
}

void Shader::_CreatePipeline(VkRenderPass renderPass, const String& vertexShaderFile, const String& fragmentShaderFile) {
	PipelineConfigInfo pipelineConfig{};
	Pipeline::DefaultConfigInfo(pipelineConfig);
	pipelineConfig.renderPass = renderPass;
	pipelineConfig.pipelineLayout = m_PipelineLayout;
	m_Pipeline = CreateScope<Rendering::Pipeline>(Engine::Rendering()->Renderer->Device, pipelineConfig, vertexShaderFile, fragmentShaderFile);
}