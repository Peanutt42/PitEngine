#include "pch.hpp"
#include "Core/Engine.hpp"
#include "UIRenderer.hpp"
#include "Core/Engine.hpp"

using namespace Pit::UI;

DEFINE_EXTERN_PROFILE_STAT_FLOAT(UIRenderingRecord, UIRendering);
DEFINE_EXTERN_PROFILE_STAT_FLOAT(UIRenderingRender, UIRendering);

static void check_vk_result(VkResult err) {
    if (err == 0) return;

    PIT_ENGINE_ERR(Log::Rendering, "[Vulkan] VkResult: {}", (int)err);

    if (err < 0) abort();
}

void SetDarkThemeColors();

Renderer::Renderer(Rendering::Renderer* renderer)
    : m_Device(renderer->Device.device()) {

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    //io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowMinSize.x = 60.f;
    style.WindowMinSize.y = 60.f;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    SetDarkThemeColors();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForVulkan(renderer->Window.GetWindowHandle(), true);
    ImGui_ImplVulkan_InitInfo init_info {
        .Instance = renderer->Device.getInstance(),
        .PhysicalDevice = renderer->Device.getPhysicalDevice(),
        .Device = renderer->Device.device(),
        .QueueFamily = renderer->Device.queueFamily(),
        .Queue = renderer->Device.queue(),
        .PipelineCache = VK_NULL_HANDLE,
        .DescriptorPool = renderer->GlobalPool->GetPool(),
        .Subpass = 0,
        .MinImageCount = renderer->MinImageCount,
        .ImageCount = Cast<uint32_t>(renderer->SwapChain->getImageCount()),
        .MSAASamples = renderer->Device.sampleCount(),
        .Allocator = nullptr,
        .CheckVkResultFn = check_vk_result
    };
    ImGui_ImplVulkan_Init(&init_info, renderer->SwapChain->getRenderPass());

    // See UIFonts.hpp->FontType for ordering of loading!!!
    io.FontDefault = io.Fonts->AddFontFromFileTTF((FileSystem::GetEngineDir() + "assets/Fonts/JetBrainsMono/JetBrainsMono-Regular.ttf").c_str(), 18.f);
    io.Fonts->AddFontFromFileTTF((FileSystem::GetEngineDir() + "assets/Fonts/JetBrainsMono/JetBrainsMono-Bold.ttf").c_str(), 18.f);
    io.Fonts->AddFontFromFileTTF((FileSystem::GetEngineDir() + "assets/Fonts/JetBrainsMono/JetBrainsMono-ExtraBold.ttf").c_str(), 18.f);
    
    VkCommandBuffer command_buffer = renderer->CommandBuffers[renderer->FrameIndex];

    VkCommandBufferBeginInfo begin_info {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT
    };
    VkResult err = vkBeginCommandBuffer(command_buffer, &begin_info);
    check_vk_result(err);

    ImGui_ImplVulkan_CreateFontsTexture(command_buffer);

    VkSubmitInfo end_info {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .commandBufferCount = 1,
        .pCommandBuffers = &command_buffer
    };
    err = vkEndCommandBuffer(command_buffer);
    check_vk_result(err);
    err = vkQueueSubmit(renderer->Device.queue(), 1, &end_info, VK_NULL_HANDLE);
    check_vk_result(err);

    err = vkDeviceWaitIdle(renderer->Device.device());
    check_vk_result(err);
    ImGui_ImplVulkan_DestroyFontUploadObjects();
    
    m_UILayerManager = new LayerManager();
}

Renderer::~Renderer() {
    vkDeviceWaitIdle(m_Device);

    delete m_UILayerManager;
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void Renderer::Record() {
    SCOPE_STAT(UIRenderingRecord);

    BeginFrame();
    DrawLayers();
    Engine::UIRenderEvent.Invoke();
    EndFrame();
}

void Renderer::BeginFrame() {
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void Renderer::EndFrame() {
    ImGui::Render();
}

void Renderer::DrawLayers() {
    m_UILayerManager->RenderLayers();
}

void Renderer::Render(VkCommandBuffer commandBuffer) {
    SCOPE_STAT(UIRenderingRender);

    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);
}

void SetDarkThemeColors() {
    // Copied from Hazel, the theme looks professional :] (src: https://github.com/TheCherno/Hazel/blob/master/Hazel/src/Hazel/ImGui/ImGuiLayer.cpp, line 116-144)
    auto& colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_WindowBg] = ImVec4{ 0.1f, 0.105f, 0.11f, 1.0f };

    // Headers
    colors[ImGuiCol_Header] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
    colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
    colors[ImGuiCol_HeaderActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

    // Buttons
    colors[ImGuiCol_Button] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
    colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
    colors[ImGuiCol_ButtonActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

    // Frame BG
    colors[ImGuiCol_FrameBg] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
    colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
    colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

    // Tabs
    colors[ImGuiCol_Tab] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
    colors[ImGuiCol_TabHovered] = ImVec4{ 0.38f, 0.3805f, 0.381f, 1.0f };
    colors[ImGuiCol_TabActive] = ImVec4{ 0.28f, 0.2805f, 0.281f, 1.0f };
    colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };

    // Title
    colors[ImGuiCol_TitleBg] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
    colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
}
