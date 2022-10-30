#include "pch.hpp"
#include "Core/Engine.hpp"
#include "Rendering/Renderer.hpp"
#include "RenderingPipeline.hpp"
#include "Mesh.hpp"
#include <fstream>

using namespace Pit;
using namespace Rendering;


static Array<char> read_file(const String& filepath) {
	std::ifstream file{ filepath, std::ios::ate | std::ios::binary };

	if (!file.is_open())
		PIT_ENGINE_ERR(Log::Rendering, "Failed to open shader code file: '{}'", filepath);

	size fileSize = Cast<size>(file.tellg());
	Array<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();
	return buffer;
}


Pipeline::Pipeline(Device& deviceRef, const PipelineConfigInfo& configInfo, const String& vertFilepath, const String& fragFilepath)
	: m_Device(deviceRef) {

	_CreatePipeline(configInfo, vertFilepath, fragFilepath);
}

Pipeline::~Pipeline() {
	vkDestroyShaderModule(m_Device.device(), m_VertShaderModule, nullptr);
	vkDestroyShaderModule(m_Device.device(), m_FragShaderModule, nullptr);
	vkDestroyPipeline(m_Device.device(), m_Pipeline, nullptr);
}

void Pipeline::DefaultConfigInfo(PipelineConfigInfo& configInfo) {
	configInfo.inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	configInfo.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	configInfo.inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

	configInfo.viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	configInfo.viewportInfo.viewportCount = 1;
	configInfo.viewportInfo.pViewports = nullptr;
	configInfo.viewportInfo.scissorCount = 1;
	configInfo.viewportInfo.pScissors = nullptr;

	configInfo.rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	configInfo.rasterizationInfo.depthClampEnable = VK_FALSE;
	configInfo.rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
	configInfo.rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
	configInfo.rasterizationInfo.lineWidth = 1.0f;
	configInfo.rasterizationInfo.cullMode = VK_CULL_MODE_NONE;
	configInfo.rasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
	configInfo.rasterizationInfo.depthBiasEnable = VK_FALSE;
	configInfo.rasterizationInfo.depthBiasConstantFactor = 0.0f;  // Optional
	configInfo.rasterizationInfo.depthBiasClamp = 0.0f;           // Optional
	configInfo.rasterizationInfo.depthBiasSlopeFactor = 0.0f;     // Optional

	configInfo.multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	configInfo.multisampleInfo.sampleShadingEnable = VK_TRUE;
	configInfo.multisampleInfo.rasterizationSamples = Engine::Rendering()->Renderer->Device.sampleCount();
	configInfo.multisampleInfo.minSampleShading = 1.0f;           // Optional
	configInfo.multisampleInfo.pSampleMask = nullptr;             // Optional
	configInfo.multisampleInfo.alphaToCoverageEnable = VK_FALSE;  // Optional
	configInfo.multisampleInfo.alphaToOneEnable = VK_FALSE;       // Optional

	configInfo.colorBlendAttachment.colorWriteMask =
		VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
		VK_COLOR_COMPONENT_A_BIT;
	configInfo.colorBlendAttachment.blendEnable = VK_FALSE;
	configInfo.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
	configInfo.colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
	configInfo.colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;              // Optional
	configInfo.colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
	configInfo.colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
	configInfo.colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;              // Optional

	configInfo.colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	configInfo.colorBlendInfo.logicOpEnable = VK_FALSE;
	configInfo.colorBlendInfo.logicOp = VK_LOGIC_OP_COPY;  // Optional
	configInfo.colorBlendInfo.attachmentCount = 1;
	configInfo.colorBlendInfo.pAttachments = &configInfo.colorBlendAttachment;
	configInfo.colorBlendInfo.blendConstants[0] = 0.0f;  // Optional
	configInfo.colorBlendInfo.blendConstants[1] = 0.0f;  // Optional
	configInfo.colorBlendInfo.blendConstants[2] = 0.0f;  // Optional
	configInfo.colorBlendInfo.blendConstants[3] = 0.0f;  // Optional

	configInfo.depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	configInfo.depthStencilInfo.depthTestEnable = VK_TRUE;
	configInfo.depthStencilInfo.depthWriteEnable = VK_TRUE;
	configInfo.depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
	configInfo.depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
	configInfo.depthStencilInfo.minDepthBounds = 0.0f;  // Optional
	configInfo.depthStencilInfo.maxDepthBounds = 1.0f;  // Optional
	configInfo.depthStencilInfo.stencilTestEnable = VK_FALSE;
	configInfo.depthStencilInfo.front = {};  // Optional
	configInfo.depthStencilInfo.back = {};   // Optional

	configInfo.dynamicStateEnables = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
	configInfo.dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	configInfo.dynamicStateInfo.pDynamicStates = configInfo.dynamicStateEnables.data();
	configInfo.dynamicStateInfo.dynamicStateCount = Cast<uint32>(configInfo.dynamicStateEnables.size());
	configInfo.dynamicStateInfo.flags = 0;
}


void Pipeline::_CreatePipeline(const PipelineConfigInfo& configInfo, const String& vertFilepath, const String& fragFilepath) {
	assert(configInfo.pipelineLayout != VK_NULL_HANDLE && "Can't create pipeline, no pipelineLayout provided in configInfo!");
	assert(configInfo.renderPass != VK_NULL_HANDLE && "Can't create pipeline, no renderpass provided in configInfo!");

	auto vertCode = read_file(vertFilepath);
	auto fragCode = read_file(fragFilepath);

	_CreateShaderModule(vertCode, &m_VertShaderModule);
	_CreateShaderModule(fragCode, &m_FragShaderModule);

	VkPipelineShaderStageCreateInfo shaderStages[2];
	shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
	shaderStages[0].module = m_VertShaderModule;
	shaderStages[0].pName = "main";
	shaderStages[0].flags = 0;
	shaderStages[0].pNext = nullptr;
	shaderStages[0].pSpecializationInfo = nullptr;

	shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	shaderStages[1].module = m_FragShaderModule;
	shaderStages[1].pName = "main";
	shaderStages[1].flags = 0;
	shaderStages[1].pNext = nullptr;
	shaderStages[1].pSpecializationInfo = nullptr;

	auto bindingDescriptions = Vertex::GetBindingDescription();
	auto attributeDescriptions = Vertex::GetAttributeDescription();
	VkPipelineVertexInputStateCreateInfo vertexInputInfo {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
		.vertexBindingDescriptionCount = Cast<uint32>(bindingDescriptions.size()),
		.pVertexBindingDescriptions = bindingDescriptions.data(),
		.vertexAttributeDescriptionCount = Cast<uint32>(attributeDescriptions.size()),
		.pVertexAttributeDescriptions = attributeDescriptions.data()
	};

	VkGraphicsPipelineCreateInfo pipelineInfo {
		.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
		.stageCount = 2,
		.pStages = shaderStages,
		.pVertexInputState = &vertexInputInfo,
		.pInputAssemblyState = &configInfo.inputAssemblyInfo,
		.pViewportState = &configInfo.viewportInfo,
		.pRasterizationState = &configInfo.rasterizationInfo,
		.pMultisampleState = &configInfo.multisampleInfo,
		.pDepthStencilState = &configInfo.depthStencilInfo,
		.pColorBlendState = &configInfo.colorBlendInfo,
		.pDynamicState = &configInfo.dynamicStateInfo,
		.layout = configInfo.pipelineLayout,
		.renderPass = configInfo.renderPass,
		.subpass = configInfo.subpass,
		.basePipelineHandle = VK_NULL_HANDLE,
		.basePipelineIndex = -1
	};

	if (vkCreateGraphicsPipelines(m_Device.device(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_Pipeline) != VK_SUCCESS)
		PIT_ENGINE_ERR(Log::Rendering, "Failed to create pipeline!");
}

void Pipeline::_CreateShaderModule(const Array<char>& code, VkShaderModule* shaderModule) {
	VkShaderModuleCreateInfo createInfo {
		.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		.codeSize = code.size(),
		.pCode = reinterpret_cast<const uint32*>(code.data())
	};

	if (vkCreateShaderModule(m_Device.device(), &createInfo, nullptr, shaderModule) != VK_SUCCESS)
		PIT_ENGINE_ERR(Log::Rendering, "Failed to create ShaderModule!");
}

void Pipeline::Bind(VkCommandBuffer commandBuffer) {
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_Pipeline);
}