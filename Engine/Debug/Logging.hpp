#pragma once

#include <memory>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#define PIT_ENGINE_LOGGING true

namespace Pit::Debug {
	class Logging {
	public:
		static void Init();

		static void Shutdown();

		static std::shared_ptr<spdlog::logger>& GetEngineLogger() { return s_EngineLogger; }
		static std::shared_ptr<spdlog::logger>& GetGameLogger() { return s_GameLogger; }

		static bool LoggerInitialized;
	private:
		static std::shared_ptr<spdlog::logger> s_EngineLogger;
		static std::shared_ptr<spdlog::logger> s_GameLogger;
	};
}

// Engine logging
#define PIT_ENGINE_TRACE(msg, ...)	if (PIT_ENGINE_LOGGING && Pit::Debug::Logging::LoggerInitialized) Pit::Debug::Logging::GetEngineLogger()->trace(msg, ##__VA_ARGS__)
#define PIT_ENGINE_INFO(msg, ...)	if (PIT_ENGINE_LOGGING && Pit::Debug::Logging::LoggerInitialized) Pit::Debug::Logging::GetEngineLogger()->info(msg, ##__VA_ARGS__)
#define PIT_ENGINE_WARN(msg, ...)	if (PIT_ENGINE_LOGGING && Pit::Debug::Logging::LoggerInitialized) Pit::Debug::Logging::GetEngineLogger()->warn(msg, ##__VA_ARGS__)
#define PIT_ENGINE_ERR(msg, ...)	if (PIT_ENGINE_LOGGING && Pit::Debug::Logging::LoggerInitialized) { Pit::Debug::Logging::GetEngineLogger()->error(msg, ##__VA_ARGS__); throw std::runtime_error(msg); }

// Game logging
#define PIT_TRACE(msg, ...)			Pit::Debug::Logging::GetGameLogger()->trace(msg, ##__VA_ARGS__)
#define PIT_INFO(msg, ...)			Pit::Debug::Logging::GetGameLogger()->info(msg, ##__VA_ARGS__)
#define PIT_WARN(msg, ...)			Pit::Debug::Logging::GetGameLogger()->warn(msg, ##__VA_ARGS__)
#define PIT_ERR(msg, ...)			Pit::Debug::Logging::GetGameLogger()->error(msg, ##__VA_ARGS__)
