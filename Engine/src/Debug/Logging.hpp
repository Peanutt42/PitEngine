#pragma once

#include "Core/Assert.hpp"
#include "Core/Event.hpp"
#include "Platform/PlatformUtils.hpp"
#include "ConsoleLogger.hpp"
#include "vcrash.h"

namespace Pit::Debug {
	/// <summary>
	/// Holder for Logging Stuff
	/// </summary>
	class Logging {
	public:
		enum Category {
			General,
			Editor,
			Audio,
			AssetManagment,
			Serialization,
			Memory,
			Networking,
			Scripting,
			ECS,
			Phyisics,
			Rendering,
			UI
		};

		static void Init();

		static void Shutdown();

		static ConsoleLoggerMT* GetEngineLogger();
		static ConsoleLoggerMT* GetGameLogger();

		static const std::string& GetLastErrorMsg() { return s_LastErrorMsg; }

		static bool LoggerInitialized;

	private:
		static ConsoleLoggerMT* s_EngineLogger;
		static ConsoleLoggerMT* s_GameLogger;

		static std::ofstream s_LogFile;
		static std::string s_LastErrorMsg;
	};
}


using Log = Pit::Debug::Logging::Category;

// Engine logging
#if DEBUG || RELEASE
#define _GetEngineLogger() \
	Pit::Debug::Logging::GetEngineLogger()

#define PIT_ENGINE_BASE_LOG(logFunc, category, msg, ...)	\
	if (Pit::Debug::Logging::LoggerInitialized) {																		\
		if constexpr (category == Log::General)				_GetEngineLogger()->logFunc("[Engine::General]      " msg, ##__VA_ARGS__);	\
		else if constexpr (category == Log::Editor)			_GetEngineLogger()->logFunc("[Engine::Editor]       " msg, ##__VA_ARGS__);	\
		else if constexpr (category == Log::Audio)			_GetEngineLogger()->logFunc("[Engine::Audio]        " msg, ##__VA_ARGS__);	\
		else if constexpr (category == Log::AssetManagment)	_GetEngineLogger()->logFunc("[Engine::Assets]       " msg, ##__VA_ARGS__);	\
		else if constexpr (category == Log::Serialization)	_GetEngineLogger()->logFunc("[Engine::Serialization]" msg, ##__VA_ARGS__);	\
		else if constexpr (category == Log::Networking)		_GetEngineLogger()->logFunc("[Engine::Networking]   " msg, ##__VA_ARGS__);	\
		else if constexpr (category == Log::Scripting)		_GetEngineLogger()->logFunc("[Engine::Scripting]    " msg, ##__VA_ARGS__);	\
		else if constexpr (category == Log::ECS)			_GetEngineLogger()->logFunc("[Engine::ECS]          " msg, ##__VA_ARGS__);	\
		else if constexpr (category == Log::Phyisics)		_GetEngineLogger()->logFunc("[Engine::Phyisics]     " msg, ##__VA_ARGS__);	\
		else if constexpr (category == Log::Rendering)		_GetEngineLogger()->logFunc("[Engine::Rendering]    " msg, ##__VA_ARGS__);	\
		else if constexpr (category == Log::UI)				_GetEngineLogger()->logFunc("[Engine::UI]           " msg, ##__VA_ARGS__);	\
	}

#define PIT_ENGINE_TRACE(category, msg, ...)	PIT_ENGINE_BASE_LOG(Trace, Log::category, msg, ##__VA_ARGS__)
#define PIT_ENGINE_INFO(category, msg, ...)		PIT_ENGINE_BASE_LOG(Info, Log::category, msg, ##__VA_ARGS__)
#define PIT_ENGINE_WARN(category, msg, ...)		PIT_ENGINE_BASE_LOG(Warn, Log::category, msg, ##__VA_ARGS__)
#define PIT_ENGINE_ERR(category, msg, ...)		PIT_ENGINE_BASE_LOG(Error, Log::category, msg,  ##__VA_ARGS__)
#define PIT_ENGINE_FATAL(category, msg, ...)	{ PIT_ENGINE_ERR(category, msg, ##__VA_ARGS__) Pit::MessagePrompts::ErrorMessage(L"PitEngine::" L#category, L#msg); CrashHandler::OnProcessCrashed(SIGABRT); PIT_DEBUGBREAK(); }
#define PIT_ENGINE_ASSERT(category, result, msg, ...)	if (!(result)) { PIT_ENGINE_FATAL(category, "Assertion Failed: " msg, ##__VA_ARGS__); }
#else
#define PIT_ENGINE_TRACE(category, msg, ...) {}
#define PIT_ENGINE_INFO(category, msg, ...)	 {}
#define PIT_ENGINE_WARN(category, msg, ...)	 {}
#define PIT_ENGINE_ERR(category, msg, ...)	 {}
#define PIT_ENGINE_FATAL(category, msg, ...) { Pit::MessagePrompts::ErrorMessage(L"PitEngine::" L#category, L#msg); abort(); }
#define PIT_ENGINE_ASSERT(category, result, msg, ...) if (!(result)) { Pit::MessagePrompts::ErrorMessage(L"PitEngine::" L#category, L"Assertion Failed: " L#msg); }
#endif

// Game logging
#if DEBUG || RELEASE
#define _GetGameLogger() \
	Pit::Debug::Logging::GetGameLogger();

#define PIT_BASE_LOG(logFunc, category, msg, ...)	\
	if (Pit::Debug::Logging::LoggerInitialized) {																		\
		if constexpr (category == Log::General)			_GetEngineLogger()->logFunc("[Game::General]    " msg, ##__VA_ARGS__);	\
		else if constexpr (category == Log::Audio)		_GetEngineLogger()->logFunc("[Game::Audio]      " msg, ##__VA_ARGS__);	\
		else if constexpr (category == Log::Networking) _GetEngineLogger()->logFunc("[Game::Networking] " msg, ##__VA_ARGS__);	\
		else if constexpr (category == Log::ECS)		_GetEngineLogger()->logFunc("[Game::ECS]        " msg, ##__VA_ARGS__);	\
		else if constexpr (category == Log::Phyisics)	_GetEngineLogger()->logFunc("[Game::Phyisics]   " msg, ##__VA_ARGS__);	\
		else if constexpr (category == Log::Rendering)	_GetEngineLogger()->logFunc("[Game::Rendering]  " msg, ##__VA_ARGS__);	\
	}

#define PIT_TRACE(msg, ...)			PIT_BASE_LOG(Trace, msg, ##__VA_ARGS__)
#define PIT_INFO(msg, ...)			PIT_BASE_LOG(Info, msg, ##__VA_ARGS__)
#define PIT_WARN(msg, ...)			PIT_BASE_LOG(Warn, msg, ##__VA_ARGS__)
#define PIT_ERR(msg, ...)			PIT_BASE_LOG(Error, msg, ##__VA_ARGS__)
#define PIT_FATAL(msg, ...)			{ PIT_ERR(msg, ##__VA_ARGS__); Pit::MessagePrompts::ErrorMessage(L"Sandbox", L#msg); CrashHandler::OnProcessCrashed(SIGABRT); PIT_DEBUGBREAK(); }
#else
#define PIT_TRACE(msg, ...)	{}
#define PIT_INFO(msg, ...)	{}
#define PIT_WARN(msg, ...)	{}
#define PIT_ERR(msg, ...)	{}
#define PIT_FATAL(msg, ...)	{ Pit::MessagePrompts::ErrorMessage(L"Sandbox", L#msg); CrashHandler::OnProcessCrashed(SIGABRT); PIT_DEBUGBREAK(); }
#endif