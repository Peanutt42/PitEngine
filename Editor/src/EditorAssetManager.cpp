#include "pch.hpp"
#include "EditorAssetManager.hpp"
#include "EditorApplication.hpp"

using namespace Pit::Editor;

void EditorAssetManager::Init() {
	// see Icon enum for indexing
	m_Icons.reserve(9);
	m_Icons.push_back(new Rendering::Texture("Editor/assets/Icons/FolderIcon.png"));
	m_Icons.push_back(new Rendering::Texture("Editor/assets/Icons/TextIcon.png"));
	m_Icons.push_back(new Rendering::Texture("Editor/assets/Icons/AudioIcon.png"));
	m_Icons.push_back(new Rendering::Texture("Editor/assets/Icons/MeshIcon.png"));
	m_Icons.push_back(new Rendering::Texture("Editor/assets/Icons/ImageIcon.png"));
	m_Icons.push_back(new Rendering::Texture("Editor/assets/Icons/ShaderIcon.png"));
	m_Icons.push_back(new Rendering::Texture("Editor/assets/Icons/BackIcon.png"));
	m_Icons.push_back(new Rendering::Texture("Editor/assets/Icons/RefreshIcon.png"));
	m_Icons.push_back(new Rendering::Texture("Editor/assets/Icons/Options.png"));
}

ImTextureID EditorAssetManager::GetIcon(Icon icon) {
	return (ImTextureID)Pit::Editor::EditorApplication::AssetManager().m_Icons[icon]->GetRendererID();
}

const Pit::Rendering::Texture* EditorAssetManager::GetIconTexture(Icon icon) {
	return (Pit::Editor::EditorApplication::AssetManager().m_Icons[icon]);
}

void EditorAssetManager::Shutdown() {
	for (int i = 0; i < m_Icons.size(); i++)
		delete m_Icons[i];
	
	m_Icons.clear();
}