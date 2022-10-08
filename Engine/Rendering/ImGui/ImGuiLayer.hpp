#pragma once

#include "Main/CoreInclude.hpp"

namespace Pit::Rendering {
	class ImGuiLayer {
	public:
		virtual ~ImGuiLayer() = default;

		virtual void OnAttach() {}
		virtual void OnDetach() {}

		virtual void OnUpdate() {}
		virtual void OnUIRender() {}
	};
}