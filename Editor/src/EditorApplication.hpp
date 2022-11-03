#pragma once

#include "Core/Engine.hpp"
#include "EditorAssetManager.hpp"
#include "EditorWindowPanel.hpp"
#include <Debug/vcrash.h>

namespace Pit::Editor {
	class EditorApplication {
	public:
		static void Init();
		static void Shutdown();

		static void Update();

		static void MenubarCallback();

		static EditorAssetManager& AssetManager() { return s_AssetManager; }

		static void test1() { test2(); }
		static void test2() { test3(); }
		static void test3() { test4(); }
		static void test4() { stack_trace(false, false); }

	private:
		static EditorAssetManager s_AssetManager;
		static Array<EditorWindowPanel*> s_WindowPanels;
		static Array<bool> s_PanelKeyShortcutsPressed;
	};




#define CONTENT_BROWSER_DRAG_DROP "CONTENT_BROWSER_ITEM"
}