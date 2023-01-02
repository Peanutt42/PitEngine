#pragma once

#include "EditorWindowPanel.hpp"

namespace Pit::Editor {
	class BuildGamePanel : public EditorWindowPanel {
	public:

		virtual void OnCreate() override;
		virtual void OnDestroy() override;

		virtual void OnGui() override;
	};
}