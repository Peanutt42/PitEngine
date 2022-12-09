#pragma once

#include "Core/Assert.hpp"
#include "Platform/PlatformUtils.hpp"
#include <memory>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace Pit::Debug {
	/// <summary>
	/// Holder for Logging Stuff
	/// </summary>
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
		Editor,
		Audio,
		AssetManagment,
		Networking,
		ECS,
		Phyisics,
		Rendering,
		UI
	};
}


using Log = Pit::Debug::Category;

// Engine logging
#if DEBUG || RELEASE
#define _GetEngineLogger() \
	Pit::Debug::Logging::GetEngineLogger()

#define PIT_ENGINE_BASE_LOG(logFunc, category, msg, ...)	\
	if (Pit::Debug::Logging::LoggerInitialized) {																		\
		if constexpr (category == Log::General)				logFunc(_GetEngineLogger(), "[Engine::General]    " msg, ##__VA_ARGS__);	\
		else if constexpr (category == Log::Editor)			logFunc(_GetEngineLogger(), "[Engine::Editor]     " msg, ##__VA_ARGS__);	\
		else if constexpr (category == Log::Audio)			logFunc(_GetEngineLogger(), "[Engine::Audio]      " msg, ##__VA_ARGS__);	\
		else if constexpr (category == Log::AssetManagment)	logFunc(_GetEngineLogger(), "[Engine::Assets]     " msg, ##__VA_ARGS__);	\
		else if constexpr (category == Log::Networking)		logFunc(_GetEngineLogger(), "[Engine::Networking] " msg, ##__VA_ARGS__);	\
		else if constexpr (category == Log::ECS)			logFunc(_GetEngineLogger(), "[Engine::ECS]        " msg, ##__VA_ARGS__);	\
		else if constexpr (category == Log::Phyisics)		logFunc(_GetEngineLogger(), "[Engine::Phyisics]   " msg, ##__VA_ARGS__);	\
		else if constexpr (category == Log::Rendering)		logFunc(_GetEngineLogger(), "[Engine::Rendering]  " msg, ##__VA_ARGS__);	\
		else if constexpr (category == Log::UI)				logFunc(_GetEngineLogger(), "[Engine::UI]         " msg, ##__VA_ARGS__);	\
	}

#define PIT_ENGINE_TRACE(category, msg, ...)	PIT_ENGINE_BASE_LOG(SPDLOG_LOGGER_TRACE, Log::category, msg, ##__VA_ARGS__)
#define PIT_ENGINE_INFO(category, msg, ...)		PIT_ENGINE_BASE_LOG(SPDLOG_LOGGER_INFO, Log::category, msg, ##__VA_ARGS__)
#define PIT_ENGINE_WARN(category, msg, ...)		PIT_ENGINE_BASE_LOG(SPDLOG_LOGGER_WARN, Log::category, msg, ##__VA_ARGS__)
#define PIT_ENGINE_ERR(category, msg, ...)		PIT_ENGINE_BASE_LOG(SPDLOG_LOGGER_ERROR, Log::category, msg,  ##__VA_ARGS__)
#define PIT_ENGINE_FATAL(category, msg, ...)	{ PIT_ENGINE_ERR(category, msg, ##__VA_ARGS__) Pit::MessagePrompts::ErrorMessage(L"PitEngine::" L#category, L#msg); PIT_DEBUGBREAK(); }
#define PIT_ENGINE_ASSERT(result, category, msg, ...)	if (!result) { PIT_ENGINE_FATAL(category, "Assertion Failed: " msg, ##__VA_ARGS__); }
#else
#define PIT_ENGINE_TRACE(category, msg, ...) {}
#define PIT_ENGINE_INFO(category, msg, ...)	 {}
#define PIT_ENGINE_WARN(category, msg, ...)	 {}
#define PIT_ENGINE_ERR(category, msg, ...)	 {}
#define PIT_ENGINE_FATAL(category, msg, ...) { Pit::MessagePrompts::ErrorMessage(L"PitEngine::" L#category, L#msg); PIT_DEBUGBREAK(); }
#define PIT_ENGINE_ASSERT(result, category, msg, ...) {}
#endif

// Game logging
#if DEBUG || RELEASE
#define _GetGameLogger() \
	Pit::Debug::Logging::GetGameLogger();

#define PIT_BASE_LOG(logFunc, category, msg, ...)	\
	if (Pit::Debug::Logging::LoggerInitialized) {																		\
		if constexpr (category == Log::General)			logFunc(_GetEngineLogger(), "[Game::General]    " msg, ##__VA_ARGS__);	\
		else if constexpr (category == Log::Audio)		logFunc(_GetEngineLogger(), "[Game::Audio]      " msg, ##__VA_ARGS__);	\
		else if constexpr (category == Log::Networking) logFunc(_GetEngineLogger(), "[Game::Networking] " msg, ##__VA_ARGS__);	\
		else if constexpr (category == Log::ECS)		logFunc(_GetEngineLogger(), "[Game::ECS]        " msg, ##__VA_ARGS__);	\
		else if constexpr (category == Log::Phyisics)	logFunc(_GetEngineLogger(), "[Game::Phyisics]   " msg, ##__VA_ARGS__);	\
		else if constexpr (category == Log::Rendering)	logFunc(_GetEngineLogger(), "[Game::Rendering]  " msg, ##__VA_ARGS__);	\
	}

#define PIT_TRACE(msg, ...)			PIT_BASE_LOG(SPDLOG_LOGGER_TRACE, msg, ##__VA_ARGS__)
#define PIT_INFO(msg, ...)			PIT_BASE_LOG(SPDLOG_LOGGER_INFO, msg, ##__VA_ARGS__)
#define PIT_WARN(msg, ...)			PIT_BASE_LOG(SPDLOG_LOGGER_WARN, msg, ##__VA_ARGS__)
#define PIT_ERR(msg, ...)			PIT_BASE_LOG(SPDLOG_LOGGER_ERROR, msg, ##__VA_ARGS__)
#define PIT_FATAL(msg, ...)			{ PIT_ERR(msg, ##__VA_ARGS__); Pit::MessagePrompts::ErrorMessage(L"Sandbox", L#msg); PIT_DEBUGBREAK(); }
#else
#define PIT_TRACE(msg, ...)	{}
#define PIT_INFO(msg, ...)	{}
#define PIT_WARN(msg, ...)	{}
#define PIT_ERR(msg, ...)	{}
#define PIT_FATAL(msg, ...)	{ Pit::MessagePrompts::ErrorMessage(L"Sandbox", L#msg); PIT_DEBUGBREAK(); }
#endif