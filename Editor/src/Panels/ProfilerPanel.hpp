#pragma once

#include "Rendering/UI/UILayer.hpp"

namespace Pit::Editor {
	class ProfilerPanel : public Rendering::UI::Layer {
	public:

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void OnGUI() override;
	};
}