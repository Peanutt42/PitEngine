#include "pch.hpp"
#include "BuildGamePanel.hpp"
#include "BuildSystem/BuildSystem.hpp"
#include <imgui/imgui.h>

using namespace Pit;
using namespace Editor;

void BuildGamePanel::OnCreate() {
	Name = "BuildGame";
	Shortcut = Array<KeyCode>{ KeyCode::LeftControl, KeyCode::B };
	Opened = false;
}

void BuildGamePanel::OnDestroy() {

}

void BuildGamePanel::OnGui() {
	if (Input::IsKeyDown(KeyCode::LeftControl) &&
		Input::IsKeyDown(KeyCode::LeftShift) &&
		Input::IsKeyPressed(KeyCode::B)) {

		BuildReport report = BuildSystem::BuildGame(OperatingSystem::Type::Win64, BuildConfiguration::Release);
		PIT_ENGINE_INFO(General, "Build finished, Report:");
		report.Print();
	}
}