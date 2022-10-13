#include "pch.hpp"
#include <imgui/imgui.h>
#include "ProfilerPanel.hpp"

using namespace Pit::Editor;

void ProfilerPanel::OnAttach() {
	Name = "Profiler";
	Shortcut = std::vector<KeyCode>{ LeftControl, P };
}

void ProfilerPanel::OnDetach() {

}

void ProfilerPanel::OnGUI() {
	ImGui::Begin(Name.c_str(), &Opened);

	ImGui::End();
}