#pragma once

#include "EditorWindowPanel.hpp"

namespace Pit::Editor {
	class LoggingOutputPanel : public EditorWindowPanel {
	public:

		virtual void OnCreate() override;
		virtual void OnDestroy() override;

		virtual void OnGui() override;
		
	private:
		std::string m_Logs;
	};
}