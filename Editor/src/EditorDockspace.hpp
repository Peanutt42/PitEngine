#pragma once

#include <imgui/imgui.h>
#include <functional>
#include "Core/Engine.hpp"

namespace Pit::Editor {
	class EditorDockspace {
	public:
		static void OnBegin(std::function<void()> MenubarCallback) {
            if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DockingEnable) {
                static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

                ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
                const ImGuiViewport* viewport = ImGui::GetMainViewport();
                ImGui::SetNextWindowPos(viewport->WorkPos);
                ImGui::SetNextWindowSize(viewport->WorkSize);
                ImGui::SetNextWindowViewport(viewport->ID);
                ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
                ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
                window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
                window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

                if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
                    window_flags |= ImGuiWindowFlags_NoBackground;

                ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
                static bool opened = true;
                ImGui::Begin("DockSpace", &opened, window_flags);

                ImGui::PopStyleVar();
                ImGui::PopStyleVar(2);

                ImGuiIO& io = ImGui::GetIO();
                if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
                    ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
                    ImGui::DockSpace(dockspace_id, ImVec2(0, 0), dockspace_flags);
                }

                if (ImGui::BeginMenuBar()) {
                    if (ImGui::BeginMenu("File")) {
                        if (ImGui::MenuItem("Exit"))
                            Pit::Engine::ForceShutdown();

                        ImGui::EndMenu();
                    }
                    if (MenubarCallback)
                        MenubarCallback();
                    ImGui::EndMenuBar();
                }
            }
		}
        static void OnEnd() {
            if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DockingEnable) {
                ImGui::End();
            }
        }
	};
}