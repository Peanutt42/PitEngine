#pragma once

#include "Core/CoreInclude.hpp"
#include "Rendering/Window.hpp"


namespace Pit::Rendering {
    struct SwapChainSupportDetails {
        VkSurfaceCapabilitiesKHR capabilities;
        Array<VkSurfaceFormatKHR> formats;
        Array<VkPresentModeKHR> presentModes;
    };

    class Device {
    public:
        const bool enableValidationLayers = PIT_ENGINE_DEBUG || PIT_ENGINE_RELEASE;

        Device(Window& window);
        ~Device();

        // Not copyable or movable
        Device(const Device&) = delete;
        Device& operator=(const Device&) = delete;
        Device(Device&&) = delete;
        Device& operator=(Device&&) = delete;

        VkInstance getInstance() { return instance; }
        VkCommandPool getCommandPool() { return commandPool; }
        VkDevice device() { return device_; }
        VkPhysicalDevice getPhysicalDevice() { return physicalDevice; }
        VkSurfaceKHR surface() { return surface_; }
        uint32 queueFamily() { return queueFamily_; }
        VkQueue queue() { return queue_; }

        SwapChainSupportDetails getSwapChainSupport() { return querySwapChainSupport(physicalDevice); }
        uint32 findMemoryType(uint32 typeFilter, VkMemoryPropertyFlags properties);
        VkFormat findSupportedFormat(const Array<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

        // Buffer Helper Functions
        void createBuffer(
            VkDeviceSize size,
            VkBufferUsageFlags usage,
            VkMemoryPropertyFlags properties,
            VkBuffer& buffer,
            VkDeviceMemory& bufferMemory);
        VkCommandBuffer beginSingleTimeCommands();
        void endSingleTimeCommands(VkCommandBuffer commandBuffer);
        void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
        void copyBufferToImage(VkBuffer buffer, VkImage image, uint32 width, uint32 height, uint32 layerCount);

        void createImageWithInfo(
            const VkImageCreateInfo& imageInfo,
            VkMemoryPropertyFlags properties,
            VkImage& image,
            VkDeviceMemory& imageMemory);

        VkPhysicalDeviceProperties properties;
        VkSampleCountFlagBits sampleCount() { return msaaSamples; }

    private:
        void createInstance();
        void setupDebugMessenger();
        void createSurface();
        void pickPhysicalDevice();
        VkSampleCountFlagBits chooseSampleCount();
        void createLogicalDevice();
        void createCommandPool();

        // helper functions
        bool isDeviceSuitable(VkPhysicalDevice device);
        Array<const char*> getRequiredExtensions();
        bool checkValidationLayerSupport();
        void findQueueFamily();
        void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
        void hasGflwRequiredInstanceExtensions();
        bool checkDeviceExtensionSupport(VkPhysicalDevice device);
        SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

        VkInstance instance;
        VkDebugUtilsMessengerEXT debugMessenger;
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
        Window& window;
        VkCommandPool commandPool;
        VkSampleCountFlagBits msaaSamples;

        VkDevice device_;
        VkSurfaceKHR surface_;
        uint32 queueFamily_;
        VkQueue queue_;

        const Array<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };
        const Array<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
    };
}