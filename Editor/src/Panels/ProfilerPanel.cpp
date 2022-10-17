#include "pch.hpp"
#include "ProfilerPanel.hpp"
#include "Rendering/RenderingSubmodule.hpp"
#include "ECS/ECSSubmodule.hpp"
#include <imgui/imgui.h>

using namespace Pit;
using namespace Editor;


DECLARE_PROFILE_STAT_FLOAT(TestStat, General, "Test");

void ProfilerPanel::OnCreate() {
	Name = "Profiler";
	Shortcut = std::vector<KeyCode>{ LeftControl, P };
}

void ProfilerPanel::OnDestroy() {

}

static void DrawStatGroups() {
	ImGuiTreeNodeFlags header_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth;
	
	for (auto& groupEntry : Debug::ProfileStatGroups::s_ProfileStatGroups) {
		if (ImGui::TreeNodeEx(groupEntry.name.c_str(), header_flags, groupEntry.name.c_str())) {
			for (auto& statEntry : *groupEntry.pFloatStats)
				ImGui::Text((statEntry.name + ": %fms").c_str(), *statEntry.pValue);
			for (auto& statEntry : *groupEntry.pIntStats)
				ImGui::Text((statEntry.name + ": %d").c_str(), *statEntry.pValue);
			for (auto& statEntry : *groupEntry.pStringStats)
				ImGui::Text((statEntry.name + ": %s").c_str(), *statEntry.pValue);
			for (auto& statEntry : *groupEntry.pMemoryStats)
				ImGui::Text((statEntry.name + ": %dbytes").c_str(), *statEntry.pValue);

			ImGui::Dummy({ 0, 8 });
			ImGui::TreePop();
		}
	}
}

void ProfilerPanel::OnGui() {
	SCOPE_STAT(TestStat);

	ImGuiTreeNodeFlags header_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth;

	ImGui::Text("DeltaTime: %fms", Time::DeltaTime() * 1000);
	ImGui::Text("FPS: %f", 1 / Time::DeltaTime());

	ImGui::Dummy({ 0, 8 });

	if (ImGui::TreeNodeEx("ProfileStats:", header_flags, "ProfileStats:")) {
		DrawStatGroups();

		ImGui::TreePop();
	}

	ImGui::Dummy({ 0, 8 });
}