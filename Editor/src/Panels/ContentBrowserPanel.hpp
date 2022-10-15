#pragma once

#include "Rendering/UI/UILayer.hpp"
#include <filesystem>

namespace Pit::Editor {
	class ContentBrowserPanel : public Rendering::UI::Layer {
	public:

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void OnGUI() override;

	private:
		std::filesystem::path m_CurrentDirectory;
		std::filesystem::path m_LastDirectory;

		void _DrawBrowserToolbar();
		void _DrawListBrowser();
		void _DrawColumnsBrowser();
	};
}