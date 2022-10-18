#include "pch.hpp"
#include "EditorAssetManager.hpp"
#include "EditorApplication.hpp"

using namespace Pit::Editor;

void EditorAssetManager::Init() {
	// see Icon enum for indexing
	m_Icons.push_back(new Rendering::Texture(FileSystem::GetEditorDir() + "assets/Icons/FolderIcon.png"));
	m_Icons.push_back(new Rendering::Texture(FileSystem::GetEditorDir() + "assets/Icons/TextIcon.png"));
	m_Icons.push_back(new Rendering::Texture(FileSystem::GetEditorDir() + "assets/Icons/AudioIcon.png"));
	m_Icons.push_back(new Rendering::Texture(FileSystem::GetEditorDir() + "assets/Icons/MeshIcon.png"));
	m_Icons.push_back(new Rendering::Texture(FileSystem::GetEditorDir() + "assets/Icons/ImageIcon.png"));
	m_Icons.push_back(new Rendering::Texture(FileSystem::GetEditorDir() + "assets/Icons/ShaderIcon.png"));
	m_Icons.push_back(new Rendering::Texture(FileSystem::GetEditorDir() + "assets/Icons/BackIcon.png"));
	m_Icons.push_back(new Rendering::Texture(FileSystem::GetEditorDir() + "assets/Icons/RefreshIcon.png"));
	m_Icons.push_back(new Rendering::Texture(FileSystem::GetEditorDir() + "assets/Icons/Options.png"));
}

ImTextureID EditorAssetManager::GetIcon(Icon icon) {
	return Cast<ImTextureID>(Pit::Editor::EditorApplication::AssetManager().m_Icons[icon]->GetDescriptorSet());
}

const Pit::Rendering::Texture* EditorAssetManager::GetIconTexture(Icon icon) {
	return (Pit::Editor::EditorApplication::AssetManager().m_Icons[icon]);
}

void EditorAssetManager::Shutdown() {
	for (int i = 0; i < m_Icons.size(); i++)
		delete m_Icons[i];
	
	m_Icons.clear();
}