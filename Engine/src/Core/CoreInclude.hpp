#pragma once

// std(c++20)
#include <string>
#include <vector>
#include <array>
#include <iostream>
#include <functional>
#include <memory>
#include <thread>
#include <fstream>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>
#include <queue>
#include <numeric>
using namespace std::chrono_literals; // for ex: 200ms
#include <atomic>

// Own
#include "Assert.hpp"
#include "PlatformDetection.hpp"
#include "Result.hpp"
#include "Time.hpp"
#include "FileSystem.hpp"
#include "UUID.hpp"
#include "Types.hpp"
#include "Version.hpp"
#include "Debug/Logging.hpp"
#include "Debug/ProfileStats.hpp"
#include "Event/Event.hpp"
#include "Input/Input.hpp"
#include "Input/KeyCode.hpp"
#include "Math/CoreMath.hpp"
#include "Utils/StringUtils.hpp"
#include "Utils/DateUtils.hpp"


namespace Pit {
	#define BIT(x) (1 << x)
}

#ifdef DEBUG
#ifndef _DEBUG
#define _DEBUG
#endif
#else
#ifndef NDEBUG
#define NDEBUG
#endif
#endif