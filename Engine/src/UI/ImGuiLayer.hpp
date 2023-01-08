#pragma once

#include "Core/CoreInclude.hpp"
#include "Core/Layer.hpp"

namespace Pit::UI {
	/// <summary>
	/// Manager to render ImGui stuff to the window
	/// </summary>
	class ImGuiLayer : public Layer {
	public:
		ImGuiLayer();

		virtual void OnCreate() override;
		virtual void OnDestroy() override;

		void Begin(), End();
	};
}