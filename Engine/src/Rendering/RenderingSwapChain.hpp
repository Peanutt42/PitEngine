#pragma once

#include "RenderingDevice.hpp"

namespace Pit::Rendering {

    class SwapChain {
    public:
        static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

        SwapChain(Device& device, VkExtent2D windowExtent);
        SwapChain(Device& device, VkExtent2D windowExtent, Ref<SwapChain> previous);
        ~SwapChain();

        SwapChain(const SwapChain&) = delete;
        SwapChain& operator=(const SwapChain&) = delete;

        VkFramebuffer getFrameBuffer(int index) { return swapChainFramebuffers[index]; }
        VkRenderPass getRenderPass() { return renderPass; }
        VkImageView getImageView(int index) { return swapChainImageViews[index]; }
        size getImageCount() { return swapChainImages.size(); }
        VkFormat getSwapChainImageFormat() { return swapChainImageFormat; }
        VkExtent2D getSwapChainExtent() { return swapChainExtent; }
        uint32 width() { return swapChainExtent.width; }
        uint32 height() { return swapChainExtent.height; }

        float extentAspectRatio() {
            return Cast<float>(swapChainExtent.width) / Cast<float>(swapChainExtent.height);
        }
        VkFormat findDepthFormat();
        size getImageIndex() { return currentFrame; }

        VkResult acquireNextImage(uint32& imageIndex);
        VkResult submitCommandBuffers(const VkCommandBuffer* buffers, uint32* imageIndex);

        bool compareSwapFormats(const SwapChain& swapChain) const {
            return swapChain.swapChainDepthFormat == swapChainDepthFormat && swapChain.swapChainImageFormat == swapChainImageFormat;
        }

        VkPresentModeKHR GetPresentMode() { return presentMode; }

    private:
        void createSwapChain();
        void createImageViews();
        void createColorResources();
        void createDepthResources();
        void createRenderPass();
        void createFramebuffers();
        void createSyncObjects();

        // Helper functions
        VkSurfaceFormatKHR chooseSwapSurfaceFormat(const Array<VkSurfaceFormatKHR>& availableFormats);
        VkPresentModeKHR chooseSwapPresentMode(const Array<VkPresentModeKHR>& availablePresentModes);
        VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
        VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32 mipLevels);
        void createImage(uint32 width, uint32 height, uint32 mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
        uint32 findMemoryType(uint32 typeFilter, VkMemoryPropertyFlags properties);

        VkPresentModeKHR presentMode;
        VkFormat swapChainImageFormat;
        VkFormat swapChainDepthFormat;
        VkExtent2D swapChainExtent;

        Array<VkFramebuffer> swapChainFramebuffers;
        VkRenderPass renderPass;

        Array<VkImage> colorImages;
        Array<VkDeviceMemory> colorImageMemorys;
        Array<VkImageView> colorImageViews;

        Array<VkImage> depthImages;
        Array<VkDeviceMemory> depthImageMemorys;
        Array<VkImageView> depthImageViews;

        Array<VkImage> swapChainImages;
        Array<VkDeviceMemory> swapChainImageMemorys;
        Array<VkImageView> swapChainImageViews;

        Device& device;
        VkExtent2D windowExtent;

        VkSwapchainKHR swapChain;
        std::shared_ptr<SwapChain> oldSwapChain;

        std::vector<VkSemaphore> imageAvailableSemaphores;
        std::vector<VkSemaphore> renderFinishedSemaphores;
        std::vector<VkFence> inFlightFences;
        std::vector<VkFence> imagesInFlight;
        size currentFrame = 0;
    };

}