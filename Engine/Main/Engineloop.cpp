#include "Engineloop.hpp"
#include "ECS/Commons/ECSTransformComponent.hpp"
#include "ECS/Commons/ECSMeshComponent.hpp"

using namespace Pit;

struct SimplePushConstantData {
	glm::mat2 transform{ 1.f };
	glm::vec2 offset;
	alignas(16) glm::vec3 color;
};

Engineloop::Engineloop() {
	FileSystem::Init();

	_LoadExampleEntities();
	_CreatePipelineLayout();
	_CreatePipeline();

	RendererSpecs rendererSpecs{};
	rendererSpecs.VSync = false;
	m_Renderer.SetSpecs(rendererSpecs);
}

Engineloop::~Engineloop() {
}

int Engineloop::Run() {
	while (!m_Window.ShouldClose()) {
		//QUICK_SCOPE_PROFILE("Main thread update");

		glfwPollEvents();

		m_ECSSubsystem.Tick(0.16f);

		if (auto commandBuffer = m_Renderer.BeginFrame()) {
			m_Renderer.BeginSwapChainRenderPass(commandBuffer);
			_RenderEntities(commandBuffer);
			m_Renderer.EndSwapChainRenderPass(commandBuffer);
			m_Renderer.EndFrame();
		}

		//m_Renderer->Render();
	}

	return 0;
}


void Engineloop::_LoadExampleEntities() {
	std::vector<Vertex> vertices{
	  {{0.0f, -0.5f}},
	  {{0.5f, 0.5f}},
	  {{-0.5f, 0.5f}} };
	auto lveModel = std::make_shared<Model>(m_Device, vertices);

	auto world = m_ECSSubsystem.GetEcsWorld();
	auto e = world->CreateEntity();
	world->AddComponent<ECS::MeshComponent>(e, lveModel, glm::vec3(.1f, .8f, .1f));
	auto& transform = world->GetComponent<ECS::TransformComponent>(e);
	transform.position.x = .2f;
	transform.scale = { 2.f, .5f };
	transform.rotation = .25f * glm::two_pi<float>();
}
void Engineloop::_CreatePipelineLayout() {
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
	if (vkCreatePipelineLayout(m_Device.device(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout) !=
		VK_SUCCESS) {
		throw std::runtime_error("failed to create pipeline layout!");
	}
}
void Engineloop::_CreatePipeline() {
	assert(m_PipelineLayout != nullptr && "Can't create pipeline before pipelineLayout");

	PipelineConfigInfo pipelineConfig{};
	Pipeline::DefaultConfigInfo(pipelineConfig);
	pipelineConfig.renderPass = m_Renderer.GetSwapChainRenderPass();
	pipelineConfig.pipelineLayout = m_PipelineLayout;
	m_Pipeline = std::make_unique<Pipeline>(
		m_Device,
		"C:/dev/cpp/PitEngine/Sandbox/shaders/simple_shader.vert.spv",
		"C:/dev/cpp/PitEngine/Sandbox/shaders/simple_shader.frag.spv",
		pipelineConfig);
}
void Engineloop::_RenderEntities(VkCommandBuffer commandBuffer) {
	m_Pipeline->Bind(commandBuffer);

	auto world = m_ECSSubsystem.GetEcsWorld();
	auto group = world->Group<ECS::MeshComponent, ECS::TransformComponent>();
	for (auto e : group) {
		auto& mesh = group.get<ECS::MeshComponent>(e);
		auto& transform = group.get<ECS::TransformComponent>(e);

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