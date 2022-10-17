#pragma once

#include <imgui/imgui.h>

namespace Pit::UI {
	enum FontType {
		Regular = 0,
		Bold = 1,
		ExtraBold = 2
	};

	static ImFont* GetFont(FontType type) {
		return ImGui::GetIO().Fonts->Fonts[type];
	}
}