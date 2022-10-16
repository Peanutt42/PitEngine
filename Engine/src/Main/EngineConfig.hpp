#pragma once

enum RenderingAPI {
	VulkanAPI
};

// Could get loaded from some Graphics.ini etc. beforehand
const static RenderingAPI RendereringAPI = RenderingAPI::VulkanAPI;

const static bool UseVsync = false;

#define PIT_CONFIGURATION_DEBUG 1
#define PIT_CONFIGURATION_DEVELOPMENT 2
#define PIT_CONFIGURATION_RELEASE 3

#define PIT_ENGINE_CONFIGURATION PIT_CONFIGURATION_DEBUG

#define PIT_ENGINE_DEBUG (PIT_ENGINE_CONFIGURATION == PIT_CONFIGURATION_DEBUG)
#define PIT_ENGINE_DEVELOPMENT (PIT_ENGINE_CONFIGURATION == PIT_CONFIGURATION_DEVELOPMENT)
#define PIT_ENGINE_RELEASE (PIT_ENGINE_CONFIGURATION == PIT_CONFIGURATION_RELEASE)


#if _WIN32 || _WIN64
#define PIT_WINDOWS
#endif
#if __APPLE__
#define PIT_APPLE
#endif
#if __linux__
#define PIT_LINUX
#endif