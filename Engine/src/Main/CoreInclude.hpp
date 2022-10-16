#pragma once

// std(c++20)
#include <string>
#include <vector>
#include <array>
#include <iostream>
#include <functional>
#include <memory>
#include <thread>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>
#include <numeric>
using namespace std::chrono_literals;
#include <atomic>

// boost
#include <boost/stacktrace.hpp>

// Own
#include "Debug/Logging.hpp"
#include "Debug/ProfileStats.hpp"
#include "Time.hpp"
#include "FileSystem.hpp"
#include "Types.hpp"
#include "EngineConfig.hpp"
#include "UUID.hpp"
#include "Input/Input.hpp"
#include "Input/KeyCode.hpp"
#include "Utils/StringUtils.hpp"
#include "Utils/DateUtils.hpp"