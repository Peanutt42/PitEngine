#include "pch.hpp"
#include "ProfilerPanel.hpp"
#include "Rendering/RenderingSubmodule.hpp"
#include "ECS/ECSSubmodule.hpp"
#include <imgui/imgui.h>

using namespace Pit;
using namespace Editor;

void ProfilerPanel::OnCreate() {
	Name = "Profiler";
	Shortcut = Array<KeyCode>{ LeftControl, Key_P };
}

void ProfilerPanel::OnDestroy() {

}

static std::atomic<bool> optick_opened;
static void OpenOptick() {
	optick_opened.store(true);
	system((FileSystem::GetRootDir() + "vendor/optick/Optick.exe").c_str());
	optick_opened.store(false);
}

void ProfilerPanel::OnGui() {
	ImGui::Text("DeltaTime: %.3fms", Time::DeltaTime() * 1000);
	ImGui::Text("FPS: %.3f", 1 / Time::DeltaTime());

	ImGui::Text("Want to profile deeper?");
	if (ImGui::Button("Open Optick Profiler"))
		Process::Run(L"vendor/optick/Optick.exe");
	ImGui::Text(" -Start recording, then stop and there you go!");
}