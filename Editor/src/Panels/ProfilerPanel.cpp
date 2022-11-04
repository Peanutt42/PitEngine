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

void ProfilerPanel::OnGui() {
	ImGui::Text("DeltaTime: %.3fms", Time::DeltaTime() * 1000);
	ImGui::Text("FPS: %.3f", 1 / Time::DeltaTime());
}