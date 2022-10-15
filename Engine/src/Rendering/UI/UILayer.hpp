#pragma once

#include "Main/CoreInclude.hpp"

namespace Pit::Rendering::UI {
	class Layer {
	public:
		virtual ~Layer() = default;

		virtual void OnAttach() {}
		virtual void OnDetach() {}

		virtual void OnGUI() {}

		bool Opened = true;
		std::string Name = "Window";
		std::vector<KeyCode> Shortcut;
		bool Enabled = true;
	};
}