#pragma once

#include <optick.h>

#if DEBUG | RELEASE
#define PIT_PROFILE_FUNCTION(...) OPTICK_EVENT(##__VA_ARGS__)
#else
#define PIT_PROFILE_FUNCTION(...)
#endif