#pragma once

#include "Core/CoreInclude.hpp"
#include "Core/Submodule.hpp"

namespace Pit::UI {
	class UIRenderer {
	public:
		UIRenderer();
		~UIRenderer();

		void Update();
	};
}