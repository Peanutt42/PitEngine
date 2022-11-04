#pragma once

#include "Core/Assert.hpp"
#include "Utils/PlatformUtils.hpp"
#include <memory>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace Pit::Debug {
	class Logging {
	public:
		static void Init();

		static void Shutdown();

		static std::shared_ptr<spdlog::logger>& GetEngineLogger() { return s_EngineLogger; }
		static std::shared_ptr<spdlog::logger>& GetGameLogger() { return s_GameLogger; }

		static bool LoggerInitialized;
	private:
		static std::shared_ptr<spdlog::sinks::basic_file_sink_mt> s_FileSink;

		static std::shared_ptr<spdlog::logger> s_EngineLogger;
		static std::shared_ptr<spdlog::logger> s_GameLogger;
	};

	enum Category {
		General,
		Audio,
		Networking,
		ECS,
		Phyisics,
		Rendering
	};
}


using Log = Pit::Debug::Category;

// Engine logging
#if DEBUG || RELEASE
#define _GetEngineLogger() \
	Pit::Debug::Logging::GetEngineLogger()

#define PIT_ENGINE_BASE_LOG(logFunc, prefix, category, msg, ...)	\
	if (Pit::Debug::Logging::LoggerInitialized) {																		\
		if constexpr (category == Log::General) logFunc(_GetEngineLogger(), prefix "[General] " msg, ##__VA_ARGS__);	\
		else if constexpr (category == Log::Audio) logFunc(_GetEngineLogger(), prefix "[Audio] " msg, ##__VA_ARGS__);	\
		else if constexpr (category == Log::Networking) logFunc(_GetEngineLogger(), prefix "[Networking] " msg, ##__VA_ARGS__);	\
		else if constexpr (category == Log::ECS) logFunc(_GetEngineLogger(), prefix "[ECS] " msg, ##__VA_ARGS__);	\
		else if constexpr (category == Log::Phyisics) logFunc(_GetEngineLogger(), prefix "[Phyisics] " msg, ##__VA_ARGS__);	\
		else if constexpr (category == Log::Rendering) logFunc(_GetEngineLogger(), prefix "[Rendering] " msg, ##__VA_ARGS__);	\
	}

#define PIT_ENGINE_TRACE(category, msg, ...)	PIT_ENGINE_BASE_LOG(SPDLOG_LOGGER_TRACE, "[CoreTrace]: ", category, msg, ##__VA_ARGS__)
#define PIT_ENGINE_INFO(category, msg, ...)		PIT_ENGINE_BASE_LOG(SPDLOG_LOGGER_INFO, "[CoreInfo]: ", category, msg, ##__VA_ARGS__)
#define PIT_ENGINE_WARN(category, msg, ...)		PIT_ENGINE_BASE_LOG(SPDLOG_LOGGER_WARN, "[CoreWarn]: ", category, msg, ##__VA_ARGS__)
#define PIT_ENGINE_ERR(category, msg, ...)		PIT_ENGINE_BASE_LOG(SPDLOG_LOGGER_ERROR, "[CoreErr]: ", category, msg,  ##__VA_ARGS__)
#define PIT_ENGINE_FATAL(category, msg, ...)	{ PIT_ENGINE_ERR(category, msg, ##__VA_ARGS__) Pit::MessagePrompts::ErrorMessage(L#msg); PIT_DEBUGBREAK(); }
#else
#define PIT_ENGINE_TRACE(category, msg, ...) {}
#define PIT_ENGINE_INFO(category, msg, ...)	 {}
#define PIT_ENGINE_WARN(category, msg, ...)	 {}
#define PIT_ENGINE_ERR(category, msg, ...)	 {}
#define PIT_ENGINE_FATAL(category, msg, ...) { throw std::runtime_error(msg); while (true){}; }
#endif

// Game logging
#if DEBUG || RELEASE
#define _GetGameLogger() \
	Pit::Debug::Logging::GetGameLogger();

#define PIT_BASE_LOG(logFunc, prefix, category, msg, ...)	\
	if (Pit::Debug::Logging::LoggerInitialized) {																		\
		if constexpr (category == Log::General) logFunc(_GetEngineLogger(), prefix "[General] " msg, ##__VA_ARGS__);	\
		else if constexpr (category == Log::Audio) logFunc(_GetEngineLogger(), prefix "[Audio] " msg, ##__VA_ARGS__);	\
		else if constexpr (category == Log::Networking) logFunc(_GetEngineLogger(), prefix "[Networking] " msg, ##__VA_ARGS__);	\
		else if constexpr (category == Log::ECS) logFunc(_GetEngineLogger(), prefix "[ECS] " msg, ##__VA_ARGS__);	\
		else if constexpr (category == Log::Phyisics) logFunc(_GetEngineLogger(), prefix "[Phyisics] " msg, ##__VA_ARGS__);	\
		else if constexpr (category == Log::Rendering) logFunc(_GetEngineLogger(), prefix "[Rendering] " msg, ##__VA_ARGS__);	\
	}

#define PIT_TRACE(msg, ...)			PIT_BASE_LOG(SPDLOG_LOGGER_TRACE, "[GameTrace]", msg, ##__VA_ARGS__)
#define PIT_INFO(msg, ...)			PIT_BASE_LOG(SPDLOG_LOGGER_INFO, "[GameInfo]", msg, ##__VA_ARGS__)
#define PIT_WARN(msg, ...)			PIT_BASE_LOG(SPDLOG_LOGGER_WARN, "[GameWarn]", msg, ##__VA_ARGS__)
#define PIT_ERR(msg, ...)			PIT_BASE_LOG(SPDLOG_LOGGER_ERROR, "[GameErr]", msg, ##__VA_ARGS__)
#define PIT_FATAL(msg, ...)			{ PIT_ERR(msg, ##__VA_ARGS__); Pit::MessagePrompts::ErrorMessage(L#msg); PIT_DEBUGBREAK(); }
#else
#define PIT_TRACE(msg, ...)	{}
#define PIT_INFO(msg, ...)	{}
#define PIT_WARN(msg, ...)	{}
#define PIT_ERR(msg, ...)	{}
#define PIT_FATAL(msg, ...)	{ throw std::runtime_error(msg); while (true){}; }
#endif