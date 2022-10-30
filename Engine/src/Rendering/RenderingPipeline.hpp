#pragma once

#include "Core/CoreInclude.hpp"
#include "RenderingDevice.hpp"

namespace Pit::Rendering {
	struct PipelineConfigInfo {
		PipelineConfigInfo() = default;
		PipelineConfigInfo(const PipelineConfigInfo&) = delete;

		VkPipelineViewportStateCreateInfo viewportInfo;
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
		VkPipelineRasterizationStateCreateInfo rasterizationInfo;
		VkPipelineMultisampleStateCreateInfo multisampleInfo;
		VkPipelineColorBlendAttachmentState colorBlendAttachment;
		VkPipelineColorBlendStateCreateInfo colorBlendInfo;
		VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
		Array<VkDynamicState> dynamicStateEnables;
		VkPipelineDynamicStateCreateInfo dynamicStateInfo;
		VkPipelineLayout pipelineLayout = nullptr;
		VkRenderPass renderPass = nullptr;
		uint32 subpass = 0;
	};

	class Pipeline {
	public:
		Pipeline(Device& deviceRef, const PipelineConfigInfo& configInfo, const String& vertFilepath, const String& fragFilepath);
		~Pipeline();

		Pipeline(const Pipeline&) = delete;
		Pipeline& operator=(const Pipeline&) = delete;

		static void DefaultConfigInfo(PipelineConfigInfo& configInfo);

		void Bind(VkCommandBuffer commandBuffer);

	private:
		Device& m_Device;
		VkPipeline m_Pipeline;
		VkShaderModule m_VertShaderModule;
		VkShaderModule m_FragShaderModule;

		void _CreatePipeline(const PipelineConfigInfo& configInfo, const String& vertFilepath, const String& fragFilepath);
		void _CreateShaderModule(const Array<char>& code, VkShaderModule* shaderModule);
	};
}
