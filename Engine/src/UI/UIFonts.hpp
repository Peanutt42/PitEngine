#pragma once

#include "Core/CoreInclude.hpp"
#include <imgui/imgui.h>

namespace Pit::UI {
	/// <summary>
	/// Libary of fonts to load and use
	/// </summary>
	class Fonts {
	public:
		enum FontType {
			Regular = 0,
			Bold = 1,
			ExtraBold = 2
		};

		static void Init() {
			ImGuiIO& io = ImGui::GetIO(); (void)io;
			constexpr float defaultSize = 18.f;

			auto regular =		io.Fonts->AddFontFromFileTTF((FileSystem::GetEngineDir() + "Resources/Fonts/JetBrainsMono/JetBrainsMono-Regular.ttf").c_str(), defaultSize);
			io.Fonts->AddFontFromFileTTF((FileSystem::GetEngineDir() + "Resources/Fonts/JetBrainsMono/JetBrainsMono-Bold.ttf").c_str(), defaultSize);
			io.Fonts->AddFontFromFileTTF((FileSystem::GetEngineDir() + "Resources/Fonts/JetBrainsMono/JetBrainsMono-ExtraBold.ttf").c_str(), defaultSize);

			io.FontDefault = regular;
		}

		static ImFont* Get(FontType type) {
			return ImGui::GetIO().Fonts->Fonts[type];
		}
	};
}