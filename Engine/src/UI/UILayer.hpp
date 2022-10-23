#pragma once

#include "Core/CoreInclude.hpp"

namespace Pit::UI {
	class Layer {
	public:
		virtual ~Layer() = default;

		virtual void OnAttach() {}
		virtual void OnDetach() {}

		virtual void OnGUI() {}
	};
}