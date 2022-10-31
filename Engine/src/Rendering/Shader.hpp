#pragma once

#include "Core/CoreInclude.hpp"
#include "RenderingPipeline.hpp"

namespace Pit::Rendering {
	class Shader {
	public:
		Shader(size pushConstantDataSize, const String& vertexShaderFile, const String& fragmentShaderFile);
		~Shader();

	private:
		ScopeRef<Rendering::Pipeline> m_Pipeline;
		VkPipelineLayout m_PipelineLayout;

		void _CreatePipelineLayout(size pushConstantDataSize);
		void _CreatePipeline(VkRenderPass renderPass, const String& vertexShaderFile, const String& fragmentShaderFile);
	};
	
	struct ShaderBuilder {
		ShaderBuilder(const String& vertexShaderFile, const String& fragmentShaderFile, size PushConstantDataSize) {

		}

		Shader Build() {

		}
	};
}