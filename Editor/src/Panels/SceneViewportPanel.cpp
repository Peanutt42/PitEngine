#include "pch.hpp"
#include "SceneViewportPanel.hpp"
#include "Engine/Main/Engine.hpp"
#include <imgui/imgui.h>
#include <stb_image.h>
#include <vulkan/vulkan.h>

using namespace Pit::Editor;

void SceneViewportPanel::OnAttach() {
	Name = "Viewport";
	Shortcut = std::vector<KeyCode>{ LeftControl, T };
	m_SceneTexture = new Pit::Rendering::Texture("C:/Users/Peter/Pictures/Wallpaper/texture.jpeg");
}


void SceneViewportPanel::OnDetach() {
	delete m_SceneTexture;
}

void SceneViewportPanel::OnGUI() {
	ImGui::Begin(Name.c_str(), &Opened);
	auto region = ImGui::GetContentRegionAvail();
	ImGui::Image((ImTextureID)m_SceneTexture->GetDescriptorSet(),
				 ImVec2(region.x, region.y));
	ImGui::End();
}