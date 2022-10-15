#include "pch.hpp"
#include <imgui/imgui.h>
#include "ProfilerPanel.hpp"
#include "Rendering/RenderingSubmodule.hpp"
#include "Audio/AudioSubmodule.hpp"
#include "ECS/ECSSubmodule.hpp"

using namespace Pit;
using namespace Editor;


DECLARE_PROFILE_STAT_FLOAT(TestStat, General, "Test");

void ProfilerPanel::OnAttach() {
	Name = "Profiler";
	Shortcut = std::vector<KeyCode>{ LeftControl, P };
}

void ProfilerPanel::OnDetach() {

}

template<typename T>
static void DrawProfileStat() {
	if (T::GetProfileType() == Debug::ProfileStatType::Float)
		ImGui::Text((T::GetName() + ": %fms").c_str(), T::Value);
	else if (T::GetProfileType() == Debug::ProfileStatType::Int)
		ImGui::Text((T::GetName() + ": %d").c_str(), T::Value);
	else if (T::GetProfileType() == Debug::ProfileStatType::String)
		ImGui::Text((T::GetName() + ": %s").c_str(), T::Value);
	else if (T::GetProfileType() == Debug::ProfileStatType::Memory)
		ImGui::Text((T::GetName() + ": %sbytes").c_str(), T::Value);
}

void ProfilerPanel::OnGUI() {
	SCOPE_STAT(TestStat);

	ImGui::Begin(Name.c_str(), &Opened);

	ImGuiTreeNodeFlags header_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth;

	ImGui::Text("DeltaTime: %fms", Time::DeltaTime() * 1000);
	ImGui::Text("FPS: %f", 1 / Time::DeltaTime());

	ImGui::Dummy({ 0, 8 });

	if (ImGui::TreeNodeEx("ProfileStats:", header_flags, "ProfileStats:")) {

		if (ImGui::TreeNodeEx("[General]", header_flags, "[General]")) {
			DrawProfileStat<STAT_TestStat>();
			ImGui::TreePop();
		}
		ImGui::Dummy({ 0, 8 });

		if (ImGui::TreeNodeEx("[Rendering]", header_flags, "[Rendering]")) {
			DrawProfileStat<STAT_RenderingUpdate>();
			ImGui::TreePop();
		}
		ImGui::Dummy({ 0, 8 });

		if (ImGui::TreeNodeEx("[Audio]", header_flags, "[Audio]")) {
			DrawProfileStat<STAT_AudioUpdate>();
			ImGui::TreePop();
		}
		ImGui::Dummy({ 0, 8 });
		
		if (ImGui::TreeNodeEx("[ECS]", header_flags, "[ECS]")) {
			DrawProfileStat<STAT_ECSUpdate>();
			ImGui::TreePop();
		}
		ImGui::Dummy({ 0, 8 });

		ImGui::TreePop();
	}

	ImGui::Dummy({ 0, 8 });

	ImGui::End();
}