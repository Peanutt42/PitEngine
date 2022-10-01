#include "ImGuiRenderer.h"
#include <ImGui/imgui_impl_vulkan.h>

using namespace Pit::Rendering;

ImGuiRenderer::ImGuiRenderer(GLFWwindow* window) 
	: m_Window(window) {

	/*IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	//ImGui_ImplGlfw_InitForVulkan(m_Window, true);*/
}

ImGuiRenderer::~ImGuiRenderer() {
	/*ImGui_ImplGlfw_Shutdown();
	//ImGui_ImplVulkan_Shutdown();
	ImGui::DestroyContext();*/
}

void ImGuiRenderer::NewFrame() {
	/*ImGui_ImplGlfw_NewFrame();
	//ImGui_ImplVulkan_NewFrame();
	ImGui::NewFrame();*/
}

void ImGuiRenderer::Render() {
	//ImGui::Render();
	//ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData());
}