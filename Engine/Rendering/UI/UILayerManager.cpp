#include "CoreInclude.hpp"
#include "UILayerManager.hpp"
#include <imgui/imgui.h>
#include "Main/Engine.hpp"

using namespace Pit::Rendering::UI;

LayerManager::LayerManager() {

}

LayerManager::~LayerManager() {
    for (int i = 0; i < m_LayerStack.size(); i++) {
        m_LayerStack[i]->OnDetach();
        m_LayerStack.erase(m_LayerStack.begin() + i);
    }
}

void LayerManager::RenderLayers() {
#ifdef PIT_EDITOR
    float minWinSizeX = ImGui::GetStyle().WindowMinSize.x;
    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DockingEnable) {
        ImGui::GetStyle().WindowMinSize.x = 300;
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
            window_flags |= ImGuiWindowFlags_NoBackground;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        static bool opened = true;
        ImGui::Begin("DockSpace", &opened, window_flags);

        ImGui::PopStyleVar();
        ImGui::PopStyleVar(2);

        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
            ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
        }

        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("Exit"))
                    Engine::Instance->ForceShutdown();

                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }
    }
#endif

    for (auto layer : m_LayerStack)
        if (layer)
            layer->OnGUI();

#ifdef PIT_EDITOR
    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DockingEnable) {
        ImGui::End();
        ImGui::GetStyle().WindowMinSize.x = minWinSizeX;
    }
#endif
}