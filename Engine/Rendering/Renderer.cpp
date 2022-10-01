#include "Renderer.hpp"

#include "Vulkan/VulkanRenderer.hpp"

#include<filesystem>
namespace fs = std::filesystem;

using namespace Pit::Rendering;

IRenderer* Renderer::Instance = nullptr;

Renderer::Renderer() {
	switch (CurrentRenderingAPI) {
	case RenderingAPI::Vulkan:
		m_Renderer = new Vulkan::Renderer();
		break;
	}
}

Renderer::~Renderer() {
	delete m_Renderer;
}

void Renderer::Render() {
	m_Renderer->Render();
}

void Renderer::SetWindow(Window* window) {
	m_Renderer->SetWindow(window);
}

void Renderer::SetSpecs(const RendererSpecs& specs) {
	m_Renderer->SetSpecs(specs);
}
 
bool Renderer::ShouldClose() {
	return m_Renderer->ShouldClose();
}