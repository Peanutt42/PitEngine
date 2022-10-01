#pragma once

#include "Main/CoreInclude.h"
#include <EnTT/entt.hpp>

namespace ECS {
	/* === CONFIGURATION === */
	#define ECS_DEEP_DEBUG

#ifdef ECS_DEEP_DEBUG
	#define ECS_CHECKS true
	#define ECS_PROFILE true
#endif

#ifdef ECS_DEBUG
	#define ECS_CHECKS true
	#define ECS_PROFILE true
#endif

#ifdef ECS_DEVELOPMENT
	#define ECS_CHECKS false
	#define ECS_PROFILE true
#endif

#ifdef ECS_RELEASE
	#define ECS_CHECKS false
	#define ECS_PROFILE false
#endif
}