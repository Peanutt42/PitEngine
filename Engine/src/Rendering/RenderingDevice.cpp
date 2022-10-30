#include "pch.hpp"
#include "RenderingDevice.hpp"

#include <unordered_set>

namespace Pit::Rendering {

    // local callback functions
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData) {
        PIT_ENGINE_ERR(Log::Rendering, "[Vulkan]: {}", pCallbackData->pMessage);

        return VK_FALSE;
    }

    VkResult CreateDebugUtilsMessengerEXT(
        VkInstance instance,
        const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkDebugUtilsMessengerEXT* pDebugMessenger) {
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
            instance,
            "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr)
            return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
        else
            return VK_ERROR_EXTENSION_NOT_PRESENT;
    }

    void DestroyDebugUtilsMessengerEXT(
        VkInstance instance,
        VkDebugUtilsMessengerEXT debugMessenger,
        const VkAllocationCallbacks* pAllocator) {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
            instance,
            "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr)
            func(instance, debugMessenger, pAllocator);
    }

    // class member functions
    Device::Device(Window& window) : window{ window } {
        createInstance();
        setupDebugMessenger();
        createSurface();
        pickPhysicalDevice();
        createLogicalDevice();
        createCommandPool();
    }

    Device::~Device() {
        vkDestroyCommandPool(device_, commandPool, nullptr);
        vkDestroyDevice(device_, nullptr);

        if (enableValidationLayers)
            DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);

        vkDestroySurfaceKHR(instance, surface_, nullptr);
        vkDestroyInstance(instance, nullptr);
    }

    void Device::createInstance() {
        if (enableValidationLayers && !checkValidationLayerSupport())
            PIT_ENGINE_FATAL(Log::Rendering, "Validation layers requested, but not available!");

        VkApplicationInfo appInfo {
            .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
            .pApplicationName = "Sandbox App",
            .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
            .pEngineName = "PitEngine",
            .engineVersion = VK_MAKE_VERSION(1, 0, 0),
            .apiVersion = VK_API_VERSION_1_0
        };

        auto extensions = getRequiredExtensions();
        VkInstanceCreateInfo createInfo {
            .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            .pApplicationInfo = &appInfo,
            .enabledExtensionCount = Cast<uint32>(extensions.size()),
            .ppEnabledExtensionNames = extensions.data()
        };

        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
        if (enableValidationLayers) {
            createInfo.enabledLayerCount = Cast<uint32>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();

            populateDebugMessengerCreateInfo(debugCreateInfo);
            createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
        }
        else {
            createInfo.enabledLayerCount = 0;
            createInfo.pNext = nullptr;
        }

        if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
            PIT_ENGINE_FATAL(Log::Rendering, "Failed to create instance!");

        hasGflwRequiredInstanceExtensions();
    }

    void Device::pickPhysicalDevice() {
        uint32 deviceCount = 0;
        vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
        if (deviceCount == 0)
            PIT_ENGINE_FATAL(Log::Rendering, "Failed to find GPUs with Vulkan support!");
        PIT_ENGINE_INFO(Log::Rendering, "Device count: {}", deviceCount);
        Array<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

        for (const auto& device : devices) {
            if (isDeviceSuitable(device)) {
                physicalDevice = device;
                msaaSamples = chooseSampleCount();
                break;
            }
        }

        if (physicalDevice == VK_NULL_HANDLE)
            PIT_ENGINE_FATAL(Log::Rendering, "Failed to find a suitable GPU!");

        vkGetPhysicalDeviceProperties(physicalDevice, &properties);
        PIT_ENGINE_INFO(Log::Rendering, "physical device: {}", properties.deviceName);
    }

    VkSampleCountFlagBits Device::chooseSampleCount() {
        VkPhysicalDeviceProperties physicalDeviceProperties;
        vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);

        VkSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts & physicalDeviceProperties.limits.framebufferDepthSampleCounts;
        if (AntiAliasing >= 64 && counts & VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
        if (AntiAliasing >= 32 && counts & VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
        if (AntiAliasing >= 16 && counts & VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }
        if (AntiAliasing >= 8 && counts & VK_SAMPLE_COUNT_8_BIT) { return VK_SAMPLE_COUNT_8_BIT; }
        if (AntiAliasing >= 4 && counts & VK_SAMPLE_COUNT_4_BIT) { return VK_SAMPLE_COUNT_4_BIT; }
        if (AntiAliasing >= 2 && counts & VK_SAMPLE_COUNT_2_BIT) { return VK_SAMPLE_COUNT_2_BIT; }

        return VK_SAMPLE_COUNT_1_BIT;
    }

    void Device::createLogicalDevice() {
        findQueueFamily();

        Array<VkDeviceQueueCreateInfo> queueCreateInfos;

        float queuePriorities[1] = { 1.0f };
        VkDeviceQueueCreateInfo queueCreateInfo {
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .queueFamilyIndex = queueFamily_,
            .queueCount = 1,
            .pQueuePriorities = queuePriorities
        };
        queueCreateInfos.push_back(queueCreateInfo);

        VkPhysicalDeviceFeatures deviceFeatures {
            .sampleRateShading = VK_TRUE,
            .samplerAnisotropy = VK_TRUE
        };

        VkDeviceCreateInfo createInfo {
            .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            .queueCreateInfoCount = Cast<uint32>(queueCreateInfos.size()),
            .pQueueCreateInfos = queueCreateInfos.data(),

            .enabledExtensionCount = Cast<uint32>(deviceExtensions.size()),
            .ppEnabledExtensionNames = deviceExtensions.data(),
            .pEnabledFeatures = &deviceFeatures
        };

        // might not really be necessary anymore because device specific validation layers
        // have been deprecated
        if (enableValidationLayers) {
            createInfo.enabledLayerCount = Cast<uint32>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
        }
        else
            createInfo.enabledLayerCount = 0;

        if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device_) != VK_SUCCESS)
            PIT_ENGINE_FATAL(Log::Rendering, "Failed to create logical device!");

        vkGetDeviceQueue(device_, queueFamily_, 0, &queue_);
    }

    void Device::createCommandPool() {
        findQueueFamily();

        VkCommandPoolCreateInfo poolInfo {
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
            .queueFamilyIndex = queueFamily_
        };

        if (vkCreateCommandPool(device_, &poolInfo, nullptr, &commandPool) != VK_SUCCESS)
            PIT_ENGINE_FATAL(Log::Rendering, "Failed to create command pool!");
    }

    void Device::createSurface() { window.CreateVKSurface(instance, &surface_); }

    bool Device::isDeviceSuitable(VkPhysicalDevice device) {
        bool extensionsSupported = checkDeviceExtensionSupport(device);

        bool swapChainAdequate = false;
        if (extensionsSupported) {
            SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
            swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
        }

        VkPhysicalDeviceFeatures supportedFeatures;
        vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

        return extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy;
    }

    void Device::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
        createInfo = {
            .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
            .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
            .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
            .pfnUserCallback = debugCallback,
            .pUserData = nullptr  // Optional
        };
    }

    void Device::setupDebugMessenger() {
        if (!enableValidationLayers) return;

        VkDebugUtilsMessengerCreateInfoEXT createInfo;
        populateDebugMessengerCreateInfo(createInfo);
        if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS)
            PIT_ENGINE_FATAL(Log::Rendering, "Failed to set up debug messenger!");
    }

    bool Device::checkValidationLayerSupport() {
        uint32 layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        Array<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        for (const char* layerName : validationLayers) {
            bool layerFound = false;

            for (const auto& layerProperties : availableLayers) {
                if (strcmp(layerName, layerProperties.layerName) == 0) {
                    layerFound = true;
                    break;
                }
            }

            if (!layerFound) {
                return false;
            }
        }

        return true;
    }

    Array<const char*> Device::getRequiredExtensions() {
        uint32 glfwExtensionCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        Array<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

        if (enableValidationLayers) {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        return extensions;
    }

    void Device::hasGflwRequiredInstanceExtensions() {
        uint32 extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
        Array<VkExtensionProperties> extensions(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

        PIT_ENGINE_INFO(Log::Rendering, "Available extensions: ");
        std::unordered_set<String> available;
        for (const auto& extension : extensions) {
            PIT_ENGINE_INFO(Log::Rendering, "\t{}", extension.extensionName);
            available.insert(extension.extensionName);
        }

        PIT_ENGINE_INFO(Log::Rendering, "Required extensions:");
        auto requiredExtensions = getRequiredExtensions();
        for (const auto& required : requiredExtensions) {
            PIT_ENGINE_INFO(Log::Rendering, "\t{}", required);
            if (available.find(required) == available.end())
                PIT_ENGINE_FATAL(Log::Rendering, "Missing required glfw extension");
        }
    }

    bool Device::checkDeviceExtensionSupport(VkPhysicalDevice device) {
        uint32 extensionCount;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

        Array<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(
            device,
            nullptr,
            &extensionCount,
            availableExtensions.data());

        std::set<String> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

        for (const auto& extension : availableExtensions) {
            requiredExtensions.erase(extension.extensionName);
        }

        return requiredExtensions.empty();
    }

    void Device::findQueueFamily() {
        uint32 count;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &count, NULL);
        VkQueueFamilyProperties* queues = (VkQueueFamilyProperties*)malloc(sizeof(VkQueueFamilyProperties) * count);
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &count, queues);
        for (uint32 i = 0; i < count; i++)
            if (queues[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                queueFamily_ = i;
                break;
            }
        free(queues);
        if (queueFamily_ == -1)
            PIT_ENGINE_FATAL(Log::Rendering, "Failed to find queueFamily!");
    }

    SwapChainSupportDetails Device::querySwapChainSupport(VkPhysicalDevice device) {
        SwapChainSupportDetails details;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface_, &details.capabilities);

        uint32 formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface_, &formatCount, nullptr);

        if (formatCount != 0) {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface_, &formatCount, details.formats.data());
        }

        uint32 presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface_, &presentModeCount, nullptr);

        if (presentModeCount != 0) {
            details.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(
                device,
                surface_,
                &presentModeCount,
                details.presentModes.data());
        }
        return details;
    }

    VkFormat Device::findSupportedFormat(
        const Array<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) {
        for (VkFormat format : candidates) {
            VkFormatProperties props;
            vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

            if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
                return format;
            }
            else if (
                tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
                return format;
            }
        }
        PIT_ENGINE_FATAL(Log::Rendering, "Failed to find supported format!");
        return VK_FORMAT_UNDEFINED;
    }

    uint32 Device::findMemoryType(uint32 typeFilter, VkMemoryPropertyFlags properties) {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);
        for (uint32 i = 0; i < memProperties.memoryTypeCount; i++) {
            if ((typeFilter & (1 << i)) &&
                (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
            }
        }

        PIT_ENGINE_FATAL(Log::Rendering, "Failed to find suitable memory type!");
        return -1;
    }

    void Device::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) {
        VkBufferCreateInfo bufferInfo {
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .size = size,
            .usage = usage,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE
        };

        if (vkCreateBuffer(device_, &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
            PIT_ENGINE_FATAL(Log::Rendering, "Failed to create vertex buffer!");

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(device_, buffer, &memRequirements);

        VkMemoryAllocateInfo allocInfo {
            .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
            .allocationSize = memRequirements.size,
            .memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties)
        };

        if (vkAllocateMemory(device_, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS)
            PIT_ENGINE_FATAL(Log::Rendering, "Failed to allocate vertex buffer memory!");

        vkBindBufferMemory(device_, buffer, bufferMemory, 0);
    }

    VkCommandBuffer Device::beginSingleTimeCommands() {
        VkCommandBufferAllocateInfo allocInfo {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .commandPool = commandPool,
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = 1
        };

        VkCommandBuffer commandBuffer;
        vkAllocateCommandBuffers(device_, &allocInfo, &commandBuffer);

        VkCommandBufferBeginInfo beginInfo {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT
        };

        vkBeginCommandBuffer(commandBuffer, &beginInfo);
        return commandBuffer;
    }

    void Device::endSingleTimeCommands(VkCommandBuffer commandBuffer) {
        vkEndCommandBuffer(commandBuffer);

        VkSubmitInfo submitInfo {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .commandBufferCount = 1,
            .pCommandBuffers = &commandBuffer
        };

        vkQueueSubmit(queue_, 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(queue_);

        vkFreeCommandBuffers(device_, commandPool, 1, &commandBuffer);
    }

    void Device::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
        VkCommandBuffer commandBuffer = beginSingleTimeCommands();

        VkBufferCopy copyRegion {
            .srcOffset = 0,  // Optional
            .dstOffset = 0,  // Optional
            .size = size
        };
        vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

        endSingleTimeCommands(commandBuffer);
    }

    void Device::copyBufferToImage(
        VkBuffer buffer, VkImage image, uint32 width, uint32 height, uint32 layerCount) {
        VkCommandBuffer commandBuffer = beginSingleTimeCommands();

        VkBufferImageCopy region {
            .bufferOffset = 0,
            .bufferRowLength = 0,
            .bufferImageHeight = 0,
            .imageSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0,  layerCount },
            .imageOffset = { 0, 0, 0 },
            .imageExtent = { width, height, 1 }
        };
        vkCmdCopyBufferToImage(commandBuffer,
                               buffer,
                               image,
                               VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                               1,
                               &region);
        endSingleTimeCommands(commandBuffer);
    }

    void Device::createImageWithInfo(const VkImageCreateInfo& imageInfo, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory) {
        if (vkCreateImage(device_, &imageInfo, nullptr, &image) != VK_SUCCESS)
            PIT_ENGINE_FATAL(Log::Rendering, "Failed to create image!");

        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(device_, image, &memRequirements);

        VkMemoryAllocateInfo allocInfo {
            .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
            .allocationSize = memRequirements.size,
            .memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties)
        };

        if (vkAllocateMemory(device_, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS)
            PIT_ENGINE_FATAL(Log::Rendering, "Failed to allocate image memory!");

        if (vkBindImageMemory(device_, image, imageMemory, 0) != VK_SUCCESS)
            PIT_ENGINE_FATAL(Log::Rendering, "Failed to bind image memory!");
    }

}