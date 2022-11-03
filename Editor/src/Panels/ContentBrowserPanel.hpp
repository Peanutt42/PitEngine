#pragma once

#include "EditorWindowPanel.hpp"
#include <filesystem>

namespace Pit::Editor {
	class ContentBrowserPanel : public EditorWindowPanel {
	public:

		virtual void OnCreate() override;
		virtual void OnDestroy() override;

		virtual void OnGui() override;

	private:
		std::filesystem::path m_CurrentDirectory;
		std::filesystem::path m_LastDirectory;

		float padding = 16.f;
		float thumbnailSize = 64;

		bool drawSettingsWindow = false;

		void _DrawBrowserToolbar();
		void _DrawSettings();
		void _DrawListBrowser();
		void _DrawListBrowserStage(const std::filesystem::path& stage, int& indentation);
		bool _DrawListBrowserItem(const std::filesystem::directory_entry& entry);
		void _DrawColumnsBrowser();
	};
}