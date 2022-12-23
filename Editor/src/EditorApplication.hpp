#pragma once

#include "Core/Engine.hpp"
#include "EditorAssetManager.hpp"
#include "EditorWindowPanel.hpp"

namespace Pit::Editor {
	struct ProjectInfo {
		String Name;
		Version EngineVersion;
	};

	class EditorApplication {
	public:
		static void SelectProject(const ProjectInfo& projectInfo);

		static void Init();
		static void Shutdown();

		static void Update();

		static void MenubarCallback();

		static EditorAssetManager& AssetManager() { return s_AssetManager; }

	private:
		static EditorAssetManager s_AssetManager;
		static Array<EditorWindowPanel*> s_WindowPanels;
		static Array<bool> s_PanelKeyShortcutsPressed;

		static ProjectInfo s_CurrentProject;
	};




#define CONTENT_BROWSER_DRAG_DROP "CONTENT_BROWSER_ITEM"
}