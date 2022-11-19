#pragma once

#include "Core/CoreInclude.hpp"
#include "Core/Submodule.hpp"

namespace Pit::UI {
	/// <summary>
	/// Manager to render ImGui stuff to the window
	/// </summary>
	class UIRenderer {
	public:
		UIRenderer();
		~UIRenderer();

		void Update();
	};
}