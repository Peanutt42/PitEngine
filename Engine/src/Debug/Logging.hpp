#pragma once

#include <memory>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

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


// Engine logging
using Log = Pit::Debug::Category;

#define _GetEngineLogger() \
	Pit::Debug::Logging::GetEngineLogger()


#define PIT_ENGINE_TRACE(category, msg, ...)	if (PIT_ENGINE_LOGGING && Pit::Debug::Logging::LoggerInitialized) SPDLOG_LOGGER_TRACE(_GetEngineLogger(), msg, ##__VA_ARGS__)
#define PIT_ENGINE_INFO(category, msg, ...)		if (PIT_ENGINE_LOGGING && Pit::Debug::Logging::LoggerInitialized) SPDLOG_LOGGER_INFO(_GetEngineLogger(), msg, ##__VA_ARGS__)
#define PIT_ENGINE_WARN(category, msg, ...)		if (PIT_ENGINE_LOGGING && Pit::Debug::Logging::LoggerInitialized) SPDLOG_LOGGER_WARN(_GetEngineLogger(), msg, ##__VA_ARGS__)
#define PIT_ENGINE_ERR(category, msg, ...)		if (PIT_ENGINE_LOGGING && Pit::Debug::Logging::LoggerInitialized) SPDLOG_LOGGER_ERROR(_GetEngineLogger(), msg, ##__VA_ARGS__)
#define PIT_ENGINE_FATAL(category, msg, ...)	if (PIT_ENGINE_LOGGING && Pit::Debug::Logging::LoggerInitialized) { PIT_ENGINE_ERR(category, msg, ##__VA_ARGS__); throw std::runtime_error(msg); while (true){} }

// Game logging
#define PIT_TRACE(msg, ...)			SPDLOG_LOGGER_TRACE(Pit::Debug::Logging::GetGameLogger(), msg, ##__VA_ARGS__)
#define PIT_INFO(msg, ...)			SPDLOG_LOGGER_INFO(Pit::Debug::Logging::GetGameLogger(), msg, ##__VA_ARGS__)
#define PIT_WARN(msg, ...)			SPDLOG_LOGGER_WARN(Pit::Debug::Logging::GetGameLogger(), msg, ##__VA_ARGS__)
#define PIT_ERR(msg, ...)			SPDLOG_LOGGER_ERROR(Pit::Debug::Logging::GetGameLogger(), msg, ##__VA_ARGS__)
#define PIT_FATAL(msg, ...)			{ PIT_ERR(msg, ##__VA_ARGS__); throw std::runtime_error(msg); while (true){} }