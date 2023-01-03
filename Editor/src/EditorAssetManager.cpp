#include "pch.hpp"
#include "EditorAssetManager.hpp"
#include "EditorApplication.hpp"

namespace Pit::Editor {

	void EditorAssetManager::Init() {
		if (!Engine::GetSettings().Headless) {
			// see Icon enum for indexing
			m_Icons.reserve(9);
			m_Icons.push_back(new Rendering::Texture("Editor/Resources/Icons/FolderIcon.png"));
			m_Icons.push_back(new Rendering::Texture("Editor/Resources/Icons/TextIcon.png"));
			m_Icons.push_back(new Rendering::Texture("Editor/Resources/Icons/AudioIcon.png"));
			m_Icons.push_back(new Rendering::Texture("Editor/Resources/Icons/MeshIcon.png"));
			m_Icons.push_back(new Rendering::Texture("Editor/Resources/Icons/ImageIcon.png"));
			m_Icons.push_back(new Rendering::Texture("Editor/Resources/Icons/ShaderIcon.png"));
			m_Icons.push_back(new Rendering::Texture("Editor/Resources/Icons/BackIcon.png"));
			m_Icons.push_back(new Rendering::Texture("Editor/Resources/Icons/RefreshIcon.png"));
			m_Icons.push_back(new Rendering::Texture("Editor/Resources/Icons/Options.png"));
		}
	}

	ImTextureID EditorAssetManager::GetIcon(Icon icon) {
		return (ImTextureID)Cast<uint64>(EditorApplication::AssetManager().m_Icons[icon]->GetRendererID());
	}

	const Rendering::Texture* EditorAssetManager::GetIconTexture(Icon icon) {
		return (EditorApplication::AssetManager().m_Icons[icon]);
	}

	void EditorAssetManager::Shutdown() {
		for (int i = 0; i < m_Icons.size(); i++)
			delete m_Icons[i];

		m_Icons.clear();
	}
}