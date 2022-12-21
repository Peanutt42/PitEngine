#pragma once

// std(c++20)
// - containers
#include <vector>
#include <array>
#include <set>
#include <unordered_set>
#include <hash_set>
#include <map>
#include <unordered_map>
#include <hash_map>
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
#include <thread>
#include <functional>
#include <memory>

#include <chrono>
using namespace std::chrono_literals; // for ex: 200ms


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
#include "Debug/Profiling.hpp"
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