#pragma once

#include "Core/Engine.hpp"
#include "EditorAssetManager.hpp"
#include "EditorWindowPanel.hpp"

namespace Pit::Editor {
	class EditorApplication {
	public:
		EditorApplication(Engine& engine);
		~EditorApplication();

		void Init(), Shutdown();

		void Update();

		static void MenubarCallback();

		static EditorApplication* Instance;

		static EditorAssetManager& AssetManager() { return Instance->m_AssetManager; }

		void test1() { test2(); }
		void test2() { test3(); }
		void test3() { test4(); }
		void test4() {  }

	private:
		Engine& m_Engine;
		EditorAssetManager m_AssetManager;
		Array<EditorWindowPanel*> m_WindowPanels;
		Array<bool> m_PanelKeyShortcutsPressed;
	};




#define CONTENT_BROWSER_DRAG_DROP "CONTENT_BROWSER_ITEM"
}