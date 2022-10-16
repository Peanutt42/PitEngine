#include "pch.hpp"
#include <imgui/imgui.h>
#include "ProfilerPanel.hpp"
#include "Rendering/RenderingSubmodule.hpp"
#include "ECS/ECSSubmodule.hpp"

using namespace Pit;
using namespace Editor;


DECLARE_PROFILE_STAT_FLOAT(TestStat, General, "Test");

void ProfilerPanel::OnCreate() {
	Name = "Profiler";
	Shortcut = std::vector<KeyCode>{ LeftControl, P };
}

void ProfilerPanel::OnDestroy() {

}

template<typename T>
static void DrawProfileStatGroup() {
	for (auto& statEntry : T::s_FloatProfileStats)
		ImGui::Text((statEntry.name + ": %fms").c_str(), *statEntry.pValue);
	for (auto& statEntry : T::s_IntProfileStats)
		ImGui::Text((statEntry.name + ": %d").c_str(), *statEntry.pValue);
	for (auto& statEntry : T::s_StringProfileStats)
		ImGui::Text((statEntry.name + ": %s").c_str(), *statEntry.pValue);
	for (auto& statEntry : T::s_MemoryProfileStats)
		ImGui::Text((statEntry.name + ": %dbytes").c_str(), *statEntry.pValue);
}

void ProfilerPanel::OnGui() {
	SCOPE_STAT(TestStat);

	ImGuiTreeNodeFlags header_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth;

	ImGui::Text("DeltaTime: %fms", Time::DeltaTime() * 1000);
	ImGui::Text("FPS: %f", 1 / Time::DeltaTime());

	ImGui::Dummy({ 0, 8 });

	if (ImGui::TreeNodeEx("ProfileStats:", header_flags, "ProfileStats:")) {

		if (ImGui::TreeNodeEx("[General]", header_flags, "[General]")) {
			DrawProfileStatGroup<STAT_GROUP_General>();
			ImGui::Dummy({ 0, 8 });
			ImGui::TreePop();
		}

		if (ImGui::TreeNodeEx("[Game]", header_flags, "[Game]")) {
			DrawProfileStatGroup<STAT_GROUP_Game>();
			ImGui::Dummy({ 0, 8 });
			ImGui::TreePop();
		}

		if (ImGui::TreeNodeEx("[Editor]", header_flags, "[Editor]")) {
			DrawProfileStatGroup<STAT_GROUP_Editor>();
			ImGui::Dummy({ 0, 8 });
			ImGui::TreePop();
		}

		if (ImGui::TreeNodeEx("[Rendering]", header_flags, "[Rendering]")) {
			DrawProfileStatGroup<STAT_GROUP_Rendering>();
			ImGui::Dummy({ 0, 8 });
			ImGui::TreePop();
		}

		if (ImGui::TreeNodeEx("[Audio]", header_flags, "[Audio]")) {
			DrawProfileStatGroup<STAT_GROUP_Audio>();
			ImGui::Dummy({ 0, 8 });
			ImGui::TreePop();
		}
		
		if (ImGui::TreeNodeEx("[ECS]", header_flags, "[ECS]")) {
			DrawProfileStatGroup<STAT_GROUP_ECS>();
			ImGui::Dummy({ 0, 8 });
			ImGui::TreePop();
		}

		if (ImGui::TreeNodeEx("[Networking]", header_flags, "[Networking]")) {
			DrawProfileStatGroup<STAT_GROUP_Networking>();
			ImGui::Dummy({ 0, 8 });
			ImGui::TreePop();
		}

		ImGui::TreePop();
	}

	ImGui::Dummy({ 0, 8 });
}