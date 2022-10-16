#include "pch.hpp"
#include <imgui/imgui.h>
#include "ContentBrowserPanel.hpp"
#include <EditorApplication.hpp>

using namespace Pit::Editor;

static std::filesystem::path s_AssetPath = std::filesystem::path(Pit::FileSystem::GetSandboxDir() + "assets");

static Pit::Rendering::Texture* folderIcon;
static Pit::Rendering::Texture* textIcon;
static Pit::Rendering::Texture* audioIcon;
static Pit::Rendering::Texture* meshIcon;
static Pit::Rendering::Texture* imageIcon;
static Pit::Rendering::Texture* shaderIcon;

void ContentBrowserPanel::OnAttach() {
	Name = "Content Browser";
	Shortcut = std::vector<KeyCode>{ LeftControl + K };

	m_CurrentDirectory = s_AssetPath;

	folderIcon = EditorApplication::AssetManager().GetIcon(FolderIcon);
	textIcon = EditorApplication::AssetManager().GetIcon(TextIcon);
	audioIcon = EditorApplication::AssetManager().GetIcon(AudioIcon);
	meshIcon = EditorApplication::AssetManager().GetIcon(MeshIcon);
	imageIcon = EditorApplication::AssetManager().GetIcon(ImageIcon);
	shaderIcon = EditorApplication::AssetManager().GetIcon(ShaderIcon);
}

void ContentBrowserPanel::OnDetach() {

}


void ContentBrowserPanel::OnGUI() {
	ImGui::Begin(Name.c_str(), &Opened);

	_DrawBrowserToolbar();
	
	// Splitview
	static float w = 200.0f;
	float h = ImGui::GetContentRegionAvail().y;//400.0f;
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
	ImGui::BeginChild("ListBrowser", ImVec2(w, h), true);

	_DrawListBrowser();

	ImGui::EndChild();
	ImGui::SameLine();
	ImGui::InvisibleButton("vsplitter", ImVec2(8.0f, h));
	if (ImGui::IsItemActive())
		w += ImGui::GetIO().MouseDelta.x;
	ImGui::SameLine();
	ImGui::BeginChild("ColumnsBrowser", ImVec2(0, h), true);

	_DrawColumnsBrowser();

	ImGui::EndChild();
	ImGui::PopStyleVar();

	ImGui::End();
}

void ContentBrowserPanel::_DrawBrowserToolbar() {
	ImVec4 tintColor = (m_CurrentDirectory == s_AssetPath) ? ImVec4(1.f, 1.f, 1.f, 0.5f) : ImVec4(1, 1, 1, 1);
	if (ImGui::ImageButton(Cast<ImTextureID>(EditorApplication::AssetManager().GetIcon(BackIcon)->GetDescriptorSet()), { 20, 20 }, { 0,1 }, { 1,0 }, -1, { 0,0,0,0 }, tintColor) ||
		/*Maybe concider hovering: prob: child blocks windowhovered*/
		Input::IsMouseButtonDown(Button3)) {
		if (m_CurrentDirectory != s_AssetPath) {
			m_LastDirectory = m_CurrentDirectory;
			m_CurrentDirectory = m_CurrentDirectory.parent_path();
		}
	}
	ImGui::SameLine();

	ImVec4 tintColor2 = (m_LastDirectory == m_CurrentDirectory ||
						 m_LastDirectory == std::filesystem::path()) ? ImVec4(1.f, 1.f, 1.f, 0.5f) : ImVec4(1, 1, 1, 1);
	if (ImGui::ImageButton(Cast<ImTextureID>(EditorApplication::AssetManager().GetIcon(BackIcon)->GetDescriptorSet()), { 20, 20 }, { 1,0 }, { 0,1 }, -1, { 0,0,0,0 }, tintColor2) ||
		(ImGui::IsWindowHovered() && Input::IsMouseButtonDown(Button4))) {
		if (m_LastDirectory != m_CurrentDirectory) {
			m_CurrentDirectory = m_LastDirectory;
		}
	}

	ImGui::SameLine();
	if (ImGui::ImageButton(Cast<ImTextureID>(EditorApplication::AssetManager().GetIcon(RefreshIcon)->GetDescriptorSet()), { 20, 20 }, { 0,1 }, { 1, 0 }) ||
		(Input::IsKeyDown(LeftControl) && Input::IsKeyDown(R))) {

		// Todo: research all elements in relative path
	}
}

void ContentBrowserPanel::_DrawListBrowser() {
	int indentation = 0;
	_DrawListBrowserStage(s_AssetPath, indentation);
}


void ContentBrowserPanel::_DrawListBrowserStage(const std::filesystem::path& stage, int& indentation) {
	for (auto& directoryEntry : std::filesystem::directory_iterator(stage)) {
		ImGui::Dummy({ 10.f * indentation, 0.f });
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
	std::string filenameString = relativePath.filename().string();

	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;

	bool opened = ImGui::TreeNodeEx(filenameString.c_str(), flags, filenameString.c_str());

	if (ImGui::BeginDragDropSource()) {
		std::string itemPath_str = relativePath.string();
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
	static float padding = 16.f;
	static float thumbnailSize = 128;
	float cellSize = thumbnailSize + padding;

	float panelWidth = ImGui::GetContentRegionAvail().x;
	int columnCount = Cast<int>(panelWidth / cellSize);
	if (columnCount < 1) columnCount = 1;

	ImGui::Columns(columnCount, 0, false);



	for (auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory)) {
		const auto& path = directoryEntry.path();
		auto relativePath = std::filesystem::relative(directoryEntry.path(), s_AssetPath);
		std::string filenameString = relativePath.filename().string();

		ImGui::PushID(filenameString.c_str());

		const auto* icon = directoryEntry.is_directory() ? folderIcon : textIcon;
		auto extention = path.extension();
		if (extention == ".wav" ||
			extention == ".mp3")
			icon = audioIcon;

		if (extention == ".obj" ||
			extention == ".fbx")
			icon = meshIcon;

		if (extention == ".png" ||
			extention == ".jpg")
			icon = imageIcon;

		if (extention == ".vert" ||
			extention == ".frag")
			icon = shaderIcon;

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
		ImGui::ImageButton(Cast<ImTextureID>(icon->GetDescriptorSet()),
						   { thumbnailSize, thumbnailSize },
						   { 0, 1 }, { 1, 0 });

		if (ImGui::BeginDragDropSource()) {
			std::string itemPath_str = relativePath.string();
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

	ImGui::SliderFloat("Thumbnail Size:", &thumbnailSize, 0, 300);
	ImGui::SliderFloat("Padding:", &padding, 0, 100);
}
