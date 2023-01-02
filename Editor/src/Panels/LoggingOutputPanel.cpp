#include "pch.hpp"
#include "LoggingOutputPanel.hpp"
#include <imgui/imgui.h>

using namespace Pit;
using namespace Editor;

void LoggingOutputPanel::OnCreate() {
	Name = "Output";
	Shortcut = Array<KeyCode>{ KeyCode::LeftControl, KeyCode::LeftShift, KeyCode::O };

	auto copyLogFunc = [](Debug::LogVerbosity, const std::string&) {};
	Debug::Logging::GetEngineLogger()->OnLogEvent += copyLogFunc;
	Debug::Logging::GetGameLogger()->OnLogEvent += copyLogFunc;
}

void LoggingOutputPanel::OnDestroy() {

}

void LoggingOutputPanel::OnGui() {
	//ImGui::Text("%s", m_Logs.c_str());
	if (Input::IsKeyDown(KeyCode::Space)) PIT_ENGINE_FATAL(General, "This was a test");
}