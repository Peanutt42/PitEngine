#pragma once

#include "EditorWindowPanel.hpp"

namespace Pit::Editor {
	class ProfilerPanel : public EditorWindowPanel {
	public:

		virtual void OnCreate() override;
		virtual void OnDestroy() override;

		virtual void OnGui() override;
	};
}