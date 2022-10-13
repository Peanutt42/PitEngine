#pragma once

#include "Engine/Rendering/UI/UILayer.hpp"
#include <filesystem>

namespace Pit::Editor {
	class ContentBrowserPanel : public Rendering::UI::Layer {
	public:

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void OnGUI() override;

	private:
		std::filesystem::path m_CurrentDirectory;
	};
}