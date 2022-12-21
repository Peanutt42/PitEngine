#include "pch.hpp"
#include "ProfilerPanel.hpp"
#include "Rendering/RenderingSubmodule.hpp"
#include "ECS/ECSSubmodule.hpp"
#include "Memory/MemorySubmodule.hpp"
#include <imgui/imgui.h>

using namespace Pit;
using namespace Editor;

void ProfilerPanel::OnCreate() {
	Name = "Profiler";
	Shortcut = Array<KeyCode>{ KeyCode::LeftControl, KeyCode::P };
}

void ProfilerPanel::OnDestroy() {

}

void ProfilerPanel::OnGui() {
	ImGui::Text("DeltaTime: %.3fms", Time::DeltaTime() * 1000);
	ImGui::Text("FPS: %.3f", 1 / Time::DeltaTime());

	ImGui::Text("Want to profile deeper?");
	if (ImGui::Button("Open Optick Profiler"))
		Process::Run(L"vendor/optick/Optick.exe");
	ImGui::Text(" -Start recording, then stop and there you go!");
	ImGui::Text("Otherwise hit ");
	ImGui::SameLine();
	static bool is_recording = false;
	String capturedFile = "Logs/ProfileReports/Report_" + CurrentTimeToString() + ".opt";
	if (!is_recording && ImGui::Button("Record")) {
		OPTICK_START_CAPTURE();
		is_recording = true;
	}
	else if (is_recording && ImGui::Button("Stop")) {
		OPTICK_STOP_CAPTURE();
		std::filesystem::create_directory("Logs/ProfileReports");
		OPTICK_SAVE_CAPTURE(capturedFile.c_str());
		is_recording = false;
	}
	ImGui::SameLine();
	if (is_recording)	ImGui::Text((" to stop recording and save it into " + capturedFile).c_str());
	else				ImGui::Text(" to start recording");

	ImGui::Dummy({ 20, 20 });
	ImGui::Text("=== Memory ===");
	ImGui::Text("FrameAllocator: ");
	size_t allocUsage = Engine::Memory()->GetFrameAllocatorUsage(), allocSize = Engine::Memory()->GetFrameAllocatorSize();
	ImGui::Text(" - Usage: %s (%.2f%%)", BytesToString(allocUsage).c_str(), (float)allocUsage / (float)allocSize);
	ImGui::Text(" - Size: %s", BytesToString(allocSize).c_str());
}