#include "pch.hpp"
#include "LoggingOutputPanel.hpp"
#include <imgui/imgui.h>
#include "Memory\MemorySubmodule.hpp"

using namespace Pit;
using namespace Editor;

void LoggingOutputPanel::OnCreate() {
	Name = "Output";
	Shortcut = Array<KeyCode>{ KeyCode::LeftControl, KeyCode::LeftShift, KeyCode::O };

	auto copyLogFunc = [this]([[maybe_unused]] Debug::LogVerbosity verbosity, const std::string& msg) {
		if (this && !Engine::ShouldClose() && msg.c_str() != nullptr) {
			bool prevFrameAllocActive = Engine::Memory()->GetFrameAllocatorActive();
			Engine::Memory()->SetFrameAllocatorActive(false);
			m_Logs += msg;
			m_Logs += '\n';
			Engine::Memory()->SetFrameAllocatorActive(prevFrameAllocActive);
		}
	};
	Debug::Logging::GetEngineLogger()->OnLogEvent += copyLogFunc;
	Debug::Logging::GetGameLogger()->OnLogEvent += copyLogFunc;
}

void LoggingOutputPanel::OnDestroy() {

}

void LoggingOutputPanel::OnGui() {
	ImGui::Text(m_Logs.c_str());
	//if (Input::IsKeyDown(KeyCode::Space)) PIT_ENGINE_FATAL(General, "This was a test");
}