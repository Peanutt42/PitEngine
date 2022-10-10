#include "CoreInclude.hpp"
#include "UIRenderer.hpp"
#include "Main/Engine.hpp"

using namespace Pit::Rendering::UI;

static void check_vk_result(VkResult err) {
    if (err == 0) return;

    PIT_ENGINE_ERR(Log::Rendering, "[Vulkan] VkResult: %d", err);

    if (err < 0) abort();
}

void SetDarkThemeColors();

Renderer::Renderer(GLFWwindow* window, VkInstance instance, VkPhysicalDevice physicalDevice, VkDevice device,
                             uint32_t queueFamily, VkQueue queue, VkPipelineCache pipelineCache, VkDescriptorPool descriptorPool,
                             uint32_t minImageCount, uint32_t imageCount, VkAllocationCallbacks* allocator)
    : m_Device(device) {

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowMinSize.x = 60.f;
    style.WindowMinSize.y = 60.f;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    SetDarkThemeColors();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForVulkan(window, true);
    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = instance;
    init_info.PhysicalDevice = physicalDevice;
    init_info.Device = device;
    init_info.QueueFamily = queueFamily;
    init_info.Queue = queue;
    init_info.PipelineCache = pipelineCache;
    init_info.DescriptorPool = descriptorPool;
    init_info.Subpass = 0;
    init_info.MinImageCount = minImageCount;
    init_info.ImageCount = imageCount;
    init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    init_info.Allocator = allocator;
    init_info.CheckVkResultFn = check_vk_result;
    ImGui_ImplVulkan_Init(&init_info, Engine::Instance->Renderer->m_RenderPass);

    // See UIFonts.hpp->FontType for ordering of loading!!!
    io.FontDefault = io.Fonts->AddFontFromFileTTF((FileSystem::GetEngineDir() + "Resources/Fonts/JetBrainsMono/JetBrainsMono-Regular.ttf").c_str(), 18.f);
    io.Fonts->AddFontFromFileTTF((FileSystem::GetEngineDir() + "Resources/Fonts/JetBrainsMono/JetBrainsMono-Bold.ttf").c_str(), 18.f);
    io.Fonts->AddFontFromFileTTF((FileSystem::GetEngineDir() + "Resources/Fonts/JetBrainsMono/JetBrainsMono-ExtraBold.ttf").c_str(), 18.f);
    
    VkCommandPool command_pool = Engine::Instance->Renderer->m_Frames[Engine::Instance->Renderer->m_FrameIndex].CommandPool;
    VkCommandBuffer command_buffer = Engine::Instance->Renderer->m_Frames[Engine::Instance->Renderer->m_FrameIndex].CommandBuffer;

    VkResult err = vkResetCommandPool(device, command_pool, 0);
    check_vk_result(err);
    VkCommandBufferBeginInfo begin_info = {};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    err = vkBeginCommandBuffer(command_buffer, &begin_info);
    check_vk_result(err);

    ImGui_ImplVulkan_CreateFontsTexture(command_buffer);

    VkSubmitInfo end_info = {};
    end_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    end_info.commandBufferCount = 1;
    end_info.pCommandBuffers = &command_buffer;
    err = vkEndCommandBuffer(command_buffer);
    check_vk_result(err);
    err = vkQueueSubmit(queue, 1, &end_info, VK_NULL_HANDLE);
    check_vk_result(err);

    err = vkDeviceWaitIdle(device);
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

void Renderer::BeginFrame() {
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void Renderer::EndFrame() {
    ImGui::Render();
}

void Renderer::RenderLayers() {
    m_UILayerManager->RenderLayers();
}

Pit::Rendering::Frame* Renderer::PresentFrame() {
    Frame* frame = &Engine::Instance->Renderer->m_Frames[Engine::Instance->Renderer->m_FrameIndex];
    {
        VkResult err = vkWaitForFences(m_Device, 1, &frame->Fence, VK_TRUE, UINT64_MAX);
        check_vk_result(err);

        err = vkResetFences(m_Device, 1, &frame->Fence);
        check_vk_result(err);
    }
    {
        VkResult err = vkResetCommandPool(m_Device, frame->CommandPool, 0);
        check_vk_result(err);
        VkCommandBufferBeginInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        err = vkBeginCommandBuffer(frame->CommandBuffer, &info);
        check_vk_result(err);
    }
    {
        VkRenderPassBeginInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        info.renderPass = Engine::Instance->Renderer->m_RenderPass;
        info.framebuffer = frame->Framebuffer;
        info.renderArea.extent.width = Engine::Instance->Renderer->m_Width;
        info.renderArea.extent.height = Engine::Instance->Renderer->m_Height;
        info.clearValueCount = 1;
        info.pClearValues = &Engine::Instance->Renderer->m_ClearValue;
        vkCmdBeginRenderPass(frame->CommandBuffer, &info, VK_SUBPASS_CONTENTS_INLINE);
    }

    // Record dear imgui primitives into command buffer
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), frame->CommandBuffer);

    return frame;
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