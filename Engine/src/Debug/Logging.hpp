#pragma once

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
#if PIT_ENGINE_DEBUG || PIT_ENGINE_RELEASE
#define _GetEngineLogger() \
	Pit::Debug::Logging::GetEngineLogger()

#define PIT_ENGINE_TRACE(category, msg, ...)	if (Pit::Debug::Logging::LoggerInitialized) SPDLOG_LOGGER_TRACE(_GetEngineLogger(), msg, ##__VA_ARGS__)
#define PIT_ENGINE_INFO(category, msg, ...)		if (Pit::Debug::Logging::LoggerInitialized) SPDLOG_LOGGER_INFO(_GetEngineLogger(), msg, ##__VA_ARGS__)
#define PIT_ENGINE_WARN(category, msg, ...)		if (Pit::Debug::Logging::LoggerInitialized) SPDLOG_LOGGER_WARN(_GetEngineLogger(), msg, ##__VA_ARGS__)
#define PIT_ENGINE_ERR(category, msg, ...)		if (Pit::Debug::Logging::LoggerInitialized) SPDLOG_LOGGER_ERROR(_GetEngineLogger(), msg, ##__VA_ARGS__)
#define PIT_ENGINE_FATAL(category, msg, ...)	if (Pit::Debug::Logging::LoggerInitialized) { PIT_ENGINE_ERR(category, msg, ##__VA_ARGS__); throw std::runtime_error(msg); while (true){} }
#else
#define PIT_ENGINE_TRACE(category, msg, ...) {}
#define PIT_ENGINE_INFO(category, msg, ...)	 {}
#define PIT_ENGINE_WARN(category, msg, ...)	 {}
#define PIT_ENGINE_ERR(category, msg, ...)	 {}
#define PIT_ENGINE_FATAL(category, msg, ...) { throw std::runtime_error(msg); while (true){}; }
#endif

// Game logging
#if PIT_ENGINE_DEBUG || PIT_ENGINE_RELEASE
#define _GetGameLogger() \
	Pit::Debug::Logging::GetGameLogger();

#define PIT_TRACE(msg, ...)			if (Pit::Debug::Logging::LoggerInitialized) SPDLOG_LOGGER_TRACE(_GetGameLogger(), msg, ##__VA_ARGS__)
#define PIT_INFO(msg, ...)			if (Pit::Debug::Logging::LoggerInitialized) SPDLOG_LOGGER_INFO(_GetGameLogger(), msg, ##__VA_ARGS__)
#define PIT_WARN(msg, ...)			if (Pit::Debug::Logging::LoggerInitialized) SPDLOG_LOGGER_WARN(_GetGameLogger(), msg, ##__VA_ARGS__)
#define PIT_ERR(msg, ...)			if (Pit::Debug::Logging::LoggerInitialized) SPDLOG_LOGGER_ERROR(_GetGameLogger(), msg, ##__VA_ARGS__)
#define PIT_FATAL(msg, ...)			if (Pit::Debug::Logging::LoggerInitialized) { PIT_ERR(msg, ##__VA_ARGS__); throw std::runtime_error(msg); while (true){} }
#else
#define PIT_TRACE(msg, ...)	{}
#define PIT_INFO(msg, ...)	{}
#define PIT_WARN(msg, ...)	{}
#define PIT_ERR(msg, ...)	{}
#define PIT_FATAL(msg, ...)	{ throw std::runtime_error(msg); while (true){}; }
#endif