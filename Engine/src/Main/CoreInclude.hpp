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
#include <queue>
#include <numeric>
using namespace std::chrono_literals; // for ex: 200ms
#include <atomic>

// Own
#include "Debug/Logging.hpp"
#include "Debug/ProfileStats.hpp"
#include "Event/Event.hpp"
#include "Time.hpp"
#include "FileSystem.hpp"
#include "Types.hpp"
#include "EngineConfig.hpp"
#include "PlatformDetection.hpp"
#include "UUID.hpp"
#include "Result.hpp"
#include "Input/Input.hpp"
#include "Input/KeyCode.hpp"
#include "Utils/StringUtils.hpp"
#include "Utils/DateUtils.hpp"


namespace Pit {
	// Inspired by TheCherno::Hazel https://github.com/TheCherno/Hazel/blob/master/Hazel/src/Hazel/Core/Base.h
	#define BIT(x) (1 << x)

	template<typename T>
	using ScopeRef = std::unique_ptr<T>;
	template<typename T, typename ... Args>
	constexpr ScopeRef<T> CreateScope(Args&& ... args) {
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using Ref = std::shared_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args) {
		return std::make_shared<T>(std::forward<Args>(args)...);
	}
}