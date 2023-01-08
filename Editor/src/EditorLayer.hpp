#pragma once

#include "Core/CoreInclude.hpp"
#include "Core/Engine.hpp"
#include "Core/Layer.hpp"
#include "EditorAssetManager.hpp"
#include "EditorWindowPanel.hpp"

namespace Pit::Editor {
	struct ProjectInfo {
		String Name = "Unnamed";
		Version EngineVersion = Version(0, 0, 1);
	};

	class EditorLayer : public Layer {
	public:
		EditorLayer();
		~EditorLayer();

		virtual void OnCreate() override;
		virtual void OnDestroy() override;

		virtual void OnUpdate() override;
		virtual void OnRender() override;
		virtual void OnUIRender() override;

		static EditorAssetManager& AssetManager() { return s_Instance->m_AssetManager; }

	private:
		inline static EditorLayer* s_Instance;

		EditorAssetManager m_AssetManager;
		Array<EditorWindowPanel*> m_WindowPanels;
		Array<bool> m_PanelKeyShortcutsPressed;

		ProjectInfo m_CurrentProject;
		std::filesystem::path m_CurrentSceneFilepath;

		void OpenSceneFromFile();
		void SaveSceneToFile();
		void SaveScene();
	};




#define CONTENT_BROWSER_DRAG_DROP "CONTENT_BROWSER_ITEM"
}