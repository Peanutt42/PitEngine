#pragma once

#include "Main/CoreInclude.hpp"
#include "VulkanTypes.hpp"
#include <vulkan/vulkan.h>
#include <set>


namespace Pit::Rendering {

    SwapchainConfiguration querySwapchainSupport(const VkPhysicalDevice& device, VkSurfaceKHR surface);

    bool checkDeviceExtensions(VkPhysicalDevice device, const std::vector<const char*>& deviceExtensions);

    bool isDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface, const std::vector<const char*>& deviceExtensions);

    VkPhysicalDevice pickPhysicalDevice(VkInstance instance, VkSurfaceKHR surface, const std::vector<const char*>& deviceExtensions);

    VkSurfaceFormatKHR pickSwapchainSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats);

    VkExtent2D pickSwapchainExtent(const VkSurfaceCapabilitiesKHR& surfaceCapabilities, int windowWidth, int windowHeight);

    VkPresentModeKHR pickSwapchainPresentMode(const std::vector<VkPresentModeKHR>& presentModes);

    VkShaderModule createShaderModule(VkDevice logicalDevice, const std::vector<char>& shaderCode);
}