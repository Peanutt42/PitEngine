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

static void DrawStatGroups() {
#if PIT_ENGINE_DEBUG || PIT_ENGINE_RELEASE
	ImGuiTreeNodeFlags header_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth;
	
	if (ImGui::TreeNodeEx("ProfileStats:", header_flags, "ProfileStats:")) {
	
		for (auto& groupEntry : Debug::ProfileStatGroups::s_ProfileStatGroups) {
			if (ImGui::TreeNodeEx(groupEntry.name.c_str(), header_flags, groupEntry.name.c_str())) {
				for (auto& statEntry : *groupEntry.pFloatStats)
					ImGui::Text((statEntry.name + ": %.3fms (avg: %.3fms)").c_str(), *statEntry.pValue, statEntry.avarageFunc());
				for (auto& statEntry : *groupEntry.pIntStats)
					ImGui::Text((statEntry.name + ": %d (avg: %d)").c_str(), *statEntry.pValue, statEntry.avarageFunc());
				for (auto& statEntry : *groupEntry.pStringStats)
					ImGui::Text((statEntry.name + ": %s").c_str(), *statEntry.pValue);
				for (auto& statEntry : *groupEntry.pMemoryStats)
					ImGui::Text((statEntry.name + ": %dbytes (avg: %dbytes").c_str(), *statEntry.pValue, statEntry.avarageFunc());

				ImGui::Dummy({ 0, 8 });
				ImGui::TreePop();
			}
		}
		
		ImGui::TreePop();
	}
#endif
}

void ProfilerPanel::OnGui() {
	ImGui::Text("DeltaTime: %.3fms", Time::DeltaTime() * 1000);
	ImGui::Text("FPS: %.3f", 1 / Time::DeltaTime());

	ImGui::Dummy({ 0, 8 });

	DrawStatGroups();

	ImGui::Dummy({ 0, 8 });
}