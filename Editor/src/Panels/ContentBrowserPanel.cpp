#include "pch.hpp"
#include <imgui/imgui.h>
#include "ContentBrowserPanel.hpp"

using namespace Pit::Editor;

static std::filesystem::path s_AssetPath = std::filesystem::path(Pit::FileSystem::GetSandboxDir() + "assets");

void ContentBrowserPanel::OnAttach() {
	Name = "Content Browser";
	Shortcut = std::vector<KeyCode>{ LeftControl + K };

	m_CurrentDirectory = s_AssetPath;
}

void ContentBrowserPanel::OnDetach() {

}

void ContentBrowserPanel::OnGUI() {
	ImGui::Begin(Name.c_str(), &Opened);

	if (m_CurrentDirectory != s_AssetPath) {
		if (ImGui::Button("<-"))
			m_CurrentDirectory = m_CurrentDirectory.parent_path();
	}

	for (auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory)) {
		const auto& path = directoryEntry.path();
		auto relativePath = std::filesystem::relative(directoryEntry.path(), s_AssetPath);
		std::string filenameString = relativePath.filename().string();
		if (directoryEntry.is_directory()) {
			if (ImGui::Button(filenameString.c_str())) {
				m_CurrentDirectory /= path.filename();
			}
		}
		else {
			if (ImGui::Button(filenameString.c_str())) {
			}
		}
	}

	ImGui::End();
}