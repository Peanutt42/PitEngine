#pragma once

#include "Main/CoreInclude.hpp"

namespace Pit::Rendering::UI {
	class Layer {
	public:
		virtual ~Layer() = default;

		virtual void OnAttach() {}
		virtual void OnDetach() {}

		virtual void OnGUI() {}
	};
}