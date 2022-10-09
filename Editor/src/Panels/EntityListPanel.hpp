#pragma once

#include "Engine/Rendering/UI/UILayer.hpp"

namespace Pit::Editor {
	class EntityListPanel : public Rendering::UI::Layer {
	public:

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void OnGUI() override;

	private:
	};
}