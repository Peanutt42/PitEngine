#include "pch.hpp"
#include "UI/UI.hpp"
#include "ContentBrowserPanel.hpp"
#include <imgui/imgui.h>
#include <EditorApplication.hpp>

using namespace Pit::Editor;

static std::filesystem::path s_AssetPath = std::filesystem::path(Pit::FileSystem::GetSandboxDir() + "assets");

void ContentBrowserPanel::OnCreate() {
	Name = "Content Browser";
	Shortcut = Array<KeyCode>{ KeyCode::LeftControl , KeyCode::K };
	Opened = false;

	m_CurrentDirectory = s_AssetPath;
}

void ContentBrowserPanel::OnDestroy() {

}


void ContentBrowserPanel::OnGui() {
	_DrawBrowserToolbar();
	
	// Splitview
	static float w = 200;
	float h = ImGui::GetContentRegionAvail().y;//400;
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
	ImGui::BeginChild("ListBrowser", ImVec2(w, h), true);

	_DrawListBrowser();

	ImGui::EndChild();
	ImGui::SameLine();
	ImGui::InvisibleButton("vsplitter", ImVec2(8, h));
	if (ImGui::IsItemActive())
		w += ImGui::GetIO().MouseDelta.x;
	ImGui::SameLine();
	ImGui::BeginChild("ColumnsBrowser", ImVec2(0, h), true);

	_DrawColumnsBrowser();

	ImGui::EndChild();
	ImGui::PopStyleVar();
}

void ContentBrowserPanel::_DrawBrowserToolbar() {
	ImVec4 tintColor = (m_CurrentDirectory == s_AssetPath) ? ImVec4(1, 1, 1, .5f) : ImVec4(1, 1, 1, 1);
	if (UI::ImageButton(EditorAssetManager::GetIconTexture(BackIcon), {20, 20}, false, false, -1, {0,0,0,0}, tintColor) ||
		// Maybe concider hovering: prob: child blocks windowhovered
		Input::IsMouseButtonDown(MouseButton::Button3)) {
		if (m_CurrentDirectory != s_AssetPath) {
			m_LastDirectory = m_CurrentDirectory;
			m_CurrentDirectory = m_CurrentDirectory.parent_path();
		}
	}
	ImGui::SameLine();

	ImVec4 tintColor2 = (m_LastDirectory == m_CurrentDirectory ||
						 m_LastDirectory == std::filesystem::path()) ? ImVec4(1, 1, 1, .5f) : ImVec4(1, 1, 1, 1);
	if (UI::ImageButton(EditorAssetManager::GetIconTexture(BackIcon), { 20, 20 }, true, false, -1, { 0,0,0,0 }, tintColor2) ||
		(ImGui::IsWindowHovered() && Input::IsMouseButtonDown(MouseButton::Button4))) {
		if (m_LastDirectory != m_CurrentDirectory) {
			m_CurrentDirectory = m_LastDirectory;
		}
	}

	ImGui::SameLine();
	if (UI::ImageButton(EditorAssetManager::GetIconTexture(RefreshIcon), { 20, 20 }, false, false) ||
		(Input::IsKeyDown(KeyCode::LeftControl) && Input::IsKeyDown(KeyCode::R))) {

		// Todo: research all elements in relative path
	}

	ImGui::SameLine();

	if (UI::ImageButton(EditorAssetManager::GetIconTexture(OptionsIcon), {20, 20}))
		drawSettingsWindow = true;
	if (drawSettingsWindow)
		_DrawSettings();
}

void ContentBrowserPanel::_DrawSettings() {
	ImGui::Begin("ContentBrowser-Settings", &drawSettingsWindow);

	UI::DragFloat("Thumbnail Size:", &thumbnailSize);
	UI::DragFloat("Padding:", &padding, 0, 100);

	ImGui::End();
}

void ContentBrowserPanel::_DrawListBrowser() {
	int indentation = 0;
	_DrawListBrowserStage(s_AssetPath, indentation);
}


void ContentBrowserPanel::_DrawListBrowserStage(const std::filesystem::path& stage, int& indentation) {
	for (auto& directoryEntry : std::filesystem::directory_iterator(stage)) {
		ImGui::Dummy({ 10.f * indentation, 0 });
		ImGui::SameLine();
		if (_DrawListBrowserItem(directoryEntry)) {
			if (directoryEntry.is_directory()) {
				indentation++;
				_DrawListBrowserStage(directoryEntry.path(), indentation);
			}
		}
	}
	indentation--;
}

bool ContentBrowserPanel::_DrawListBrowserItem(const std::filesystem::directory_entry& entry) {
	const auto& path = entry.path();
	auto relativePath = std::filesystem::relative(entry.path(), s_AssetPath);
	String filenameString = relativePath.filename().string();

	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;

	bool opened = ImGui::TreeNodeEx(filenameString.c_str(), flags, filenameString.c_str());

	if (ImGui::BeginDragDropSource()) {
		String itemPath_str = relativePath.string();
		const char* itemPath = itemPath_str.c_str();
		ImGui::SetDragDropPayload(CONTENT_BROWSER_DRAG_DROP, itemPath, sizeof(itemPath_str));

		ImGui::EndDragDropSource();
	}

	if ((ImGui::IsItemHovered() || ImGui::IsItemActive()) &&
		ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {

		if (entry.is_directory())
			m_CurrentDirectory /= path.filename();
	}

	if (opened)
		ImGui::TreePop();

	return opened;
}



void ContentBrowserPanel::_DrawColumnsBrowser() {
	float cellSize = thumbnailSize + padding;

	float panelWidth = ImGui::GetContentRegionAvail().x;
	int columnCount = Cast<int>(panelWidth / cellSize);
	if (columnCount < 1) columnCount = 1;

	ImGui::Columns(columnCount, 0, false);



	for (auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory)) {
		const auto& path = directoryEntry.path();
		auto relativePath = std::filesystem::relative(directoryEntry.path(), s_AssetPath);
		String filenameString = relativePath.filename().string();

		ImGui::PushID(filenameString.c_str());

		auto* icon = directoryEntry.is_directory() ? EditorAssetManager::GetIconTexture(FolderIcon) :
													EditorAssetManager::GetIconTexture(TextIcon);
		auto extention = path.extension();
		if (extention == ".wav" ||
			extention == ".mp3")
			icon = EditorAssetManager::GetIconTexture(AudioIcon);

		if (extention == ".obj" ||
			extention == ".fbx")
			icon = EditorAssetManager::GetIconTexture(MeshIcon);

		if (extention == ".png" ||
			extention == ".jpg")
			icon = EditorAssetManager::GetIconTexture(ImageIcon);

		if (extention == ".vert" ||
			extention == ".frag")
			icon = EditorAssetManager::GetIconTexture(ShaderIcon);

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
		UI::ImageButton(icon, { thumbnailSize, thumbnailSize });

		if (ImGui::BeginDragDropSource()) {
			String itemPath_str = relativePath.string();
			const char* itemPath = itemPath_str.c_str();
			ImGui::SetDragDropPayload(CONTENT_BROWSER_DRAG_DROP, itemPath, sizeof(itemPath_str));

			ImGui::EndDragDropSource();
		}

		ImGui::PopStyleColor();

		if (ImGui::IsItemHovered() &&
			ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {

			if (directoryEntry.is_directory())
				m_CurrentDirectory /= path.filename();
		}
		ImGui::TextWrapped(filenameString.c_str());

		ImGui::NextColumn();

		ImGui::PopID();
	}

	ImGui::Columns(1);
}
