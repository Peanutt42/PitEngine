#pragma once

enum RenderingAPI {
	VulkanAPI
};

// Could get loaded from some Graphics.ini etc. beforehand
const static RenderingAPI RendereringAPI = RenderingAPI::VulkanAPI;

const static bool UseVsync = true;
const static int AntiAliasing = 1;

#define PIT_CONFIGURATION_DEBUG 1
#define PIT_CONFIGURATION_RELEASE 2
#define PIT_CONFIGURATION_DIST 3

#if defined(DEBUG)
#define PIT_ENGINE_CONFIGURATION PIT_CONFIGURATION_DEBUG
#elif defined(RELEASE)
#define PIT_ENGINE_CONFIGURATION PIT_CONFIGURATION_RELEASE
#elif defined(DIST)
#define PIT_ENGINE_CONFIGURATION PIT_CONFIGURATION_DIST
#else
#error "No configuration provided"
#endif

#define PIT_ENGINE_DEBUG		(PIT_ENGINE_CONFIGURATION == PIT_CONFIGURATION_DEBUG)
#define PIT_ENGINE_RELEASE		(PIT_ENGINE_CONFIGURATION == PIT_CONFIGURATION_RELEASE)
#define PIT_ENGINE_DIST			(PIT_ENGINE_CONFIGURATION == PIT_CONFIGURATION_DIST)