#pragma once

// std(c++20)
// - containers
#include <vector>
#include <array>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>
#include <queue>
#include <deque>
#include <span>
#include <tuple>
#include <utility>
#include <numeric>
#include <algorithm>

// - file io
#include <filesystem>
#include <iostream>
#include <fstream>
#include <istream>
#include <ostream>

// - string
#include <string>
#include <sstream>
#include <string_view>

// - low level
#include <atomic>
#include <mutex>
#include <thread>
#include <functional>
#include <memory>
#include <signal.h>

#include <chrono>
using namespace std::chrono_literals; // for ex: 200ms


// Own
#include "Assert.hpp"
#include "PlatformDetection.hpp"
#include "Time.hpp"
#include "FileSystem.hpp"
#include "UUID.hpp"
#include "Types.hpp"
#include "Version.hpp"
#include "Debug/Logging.hpp"
#include "Debug/Profiling.hpp"
#include "Core/Event.hpp"
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