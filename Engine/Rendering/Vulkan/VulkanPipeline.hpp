#pragma once

#include "Main/CoreInclude.hpp"
#include "Rendering/RenderingInclude.hpp"
#include "VulkanDevice.hpp"

namespace Pit::Rendering::Vulkan {
	struct PipelineConfigInfo {
		VkViewport viewport;
		VkRect2D scissor;
		VkPipelineViewportStateCreateInfo viewportInfo;
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
		VkPipelineRasterizationStateCreateInfo rasterizationInfo;
		VkPipelineMultisampleStateCreateInfo multisampleInfo;
		VkPipelineColorBlendAttachmentState colorBlendAttachment;
		VkPipelineColorBlendStateCreateInfo colorBlendInfo;
		VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
		VkPipelineLayout pipelineLayout = nullptr;
		VkRenderPass renderPass = nullptr;
		uint32_t subpass = 0;
	};

	class Pipeline {
	public:
		Pipeline(
			Device& device,
			const std::string& vertFilepath,
			const std::string& fragFilepath,
			const PipelineConfigInfo& configInfo);
		~Pipeline();

		Pipeline(const Pipeline&) = delete;
		void operator=(const Pipeline&) = delete;

		void Bind(VkCommandBuffer commandBuffer);
		static void DefaultConfigInfo(PipelineConfigInfo& configInfo, uint32_t width, uint32_t height);

	private:
		Device& m_Device;
		VkPipeline m_GraphicsPipeline;
		VkShaderModule m_VertShaderModule;
		VkShaderModule m_FragShaderModule;

		static std::vector<char> _ReadFile(const std::string& filepath);

		void _CreateGraphicsPipeline(const std::string& vertFilepath,
									 const std::string& fragFilepath,
									 const PipelineConfigInfo& configInfo);
		void _CreateShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);
	};
}