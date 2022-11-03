#pragma once

#include "Core/CoreInclude.hpp"
#include <imgui/imgui.h>

namespace Pit::UI {
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

			auto regular =		io.Fonts->AddFontFromFileTTF((FileSystem::GetEngineDir() + "assets/Fonts/JetBrainsMono/JetBrainsMono-Regular.ttf").c_str(), defaultSize);
			auto bold =			io.Fonts->AddFontFromFileTTF((FileSystem::GetEngineDir() + "assets/Fonts/JetBrainsMono/JetBrainsMono-Bold.ttf").c_str(), defaultSize);
			auto extraBold =	io.Fonts->AddFontFromFileTTF((FileSystem::GetEngineDir() + "assets/Fonts/JetBrainsMono/JetBrainsMono-ExtraBold.ttf").c_str(), defaultSize);

			io.FontDefault = regular;
		}

		static ImFont* Get(FontType type) {
			return ImGui::GetIO().Fonts->Fonts[type];
		}
	};
}