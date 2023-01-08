#include "pch.hpp"
#include "EditorAssetManager.hpp"
#include "EditorLayer.hpp"

namespace Pit::Editor {

	void EditorAssetManager::Init() {
		if (!Engine::GetSettings().Headless) {
			// see Icon enum for indexing
			m_Icons.reserve(Icon::COUNT);
			m_Icons.emplace_back("Editor/Resources/Icons/FolderIcon.png");
			m_Icons.emplace_back("Editor/Resources/Icons/TextIcon.png");
			m_Icons.emplace_back("Editor/Resources/Icons/AudioIcon.png");
			m_Icons.emplace_back("Editor/Resources/Icons/MeshIcon.png");
			m_Icons.emplace_back("Editor/Resources/Icons/ImageIcon.png");
			m_Icons.emplace_back("Editor/Resources/Icons/ShaderIcon.png");
			m_Icons.emplace_back("Editor/Resources/Icons/BackIcon.png");
			m_Icons.emplace_back("Editor/Resources/Icons/RefreshIcon.png");
			m_Icons.emplace_back("Editor/Resources/Icons/Options.png");
			m_Icons.emplace_back("Editor/Resources/Icons/PlayButton.png");
			m_Icons.emplace_back("Editor/Resources/Icons/StopButton.png");
			m_Icons.emplace_back("Editor/Resources/Icons/SimulateButton.png");
			m_Icons.emplace_back("Editor/Resources/Icons/StepButton.png");
			m_Icons.emplace_back("Editor/Resources/Icons/ExitButton.png");
		}
	}

	ImTextureID EditorAssetManager::GetIcon(Icon icon) {
		return (ImTextureID)Cast<uint64>(EditorLayer::AssetManager().m_Icons[icon].GetRendererID());
	}

	const Rendering::Texture& EditorAssetManager::GetIconTexture(Icon icon) {
		return EditorLayer::AssetManager().m_Icons[icon];
	}

	void EditorAssetManager::Shutdown() {
		m_Icons.clear();
	}
}