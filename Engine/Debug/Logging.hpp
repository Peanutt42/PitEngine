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

		static std::shared_ptr<spdlog::logger>& GetEngineGeneralLogger() { return s_EngineLogger_General; }
		static std::shared_ptr<spdlog::logger>& GetEngineAudioLogger() { return s_EngineLogger_Audio; }
		static std::shared_ptr<spdlog::logger>& GetEngineNetworkingLogger() { return s_EngineLogger_Networking; }
		static std::shared_ptr<spdlog::logger>& GetEngineECSLogger() { return s_EngineLogger_ECS; }
		static std::shared_ptr<spdlog::logger>& GetEnginePhysicsLogger() { return s_EngineLogger_Physics; }
		static std::shared_ptr<spdlog::logger>& GetEngineRenderingLogger() { return s_EngineLogger_Rendering; }
		static std::shared_ptr<spdlog::logger>& GetGameLogger() { return s_GameLogger; }

		static bool LoggerInitialized;
	private:
		// Engine
		static std::shared_ptr<spdlog::logger> s_EngineLogger_General;
		static std::shared_ptr<spdlog::logger> s_EngineLogger_Audio;
		static std::shared_ptr<spdlog::logger> s_EngineLogger_Networking;
		static std::shared_ptr<spdlog::logger> s_EngineLogger_ECS;
		static std::shared_ptr<spdlog::logger> s_EngineLogger_Physics;
		static std::shared_ptr<spdlog::logger> s_EngineLogger_Rendering;
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
static std::shared_ptr<spdlog::logger> _GetEngineLogger(Log category) {
	switch (category) {
	default: return Pit::Debug::Logging::GetEngineGeneralLogger();
	case Log::Audio: return Pit::Debug::Logging::GetEngineAudioLogger();
	case Log::Networking: return Pit::Debug::Logging::GetEngineNetworkingLogger();
	case Log::ECS: return Pit::Debug::Logging::GetEngineECSLogger();
	case Log::Phyisics: return Pit::Debug::Logging::GetEnginePhysicsLogger();
	case Log::Rendering: return Pit::Debug::Logging::GetEngineRenderingLogger();
	}
}

#define PIT_ENGINE_TRACE(category, msg, ...)	if (PIT_ENGINE_LOGGING && Pit::Debug::Logging::LoggerInitialized) SPDLOG_LOGGER_TRACE(_GetEngineLogger(category), msg, ##__VA_ARGS__)
#define PIT_ENGINE_INFO(category, msg, ...)		if (PIT_ENGINE_LOGGING && Pit::Debug::Logging::LoggerInitialized) SPDLOG_LOGGER_INFO(_GetEngineLogger(category), msg, ##__VA_ARGS__)
#define PIT_ENGINE_WARN(category, msg, ...)		if (PIT_ENGINE_LOGGING && Pit::Debug::Logging::LoggerInitialized) SPDLOG_LOGGER_WARN(_GetEngineLogger(category), msg, ##__VA_ARGS__)
#define PIT_ENGINE_ERR(category, msg, ...)		if (PIT_ENGINE_LOGGING && Pit::Debug::Logging::LoggerInitialized) SPDLOG_LOGGER_ERROR(_GetEngineLogger(category), msg, ##__VA_ARGS__)
#define PIT_ENGINE_FATAL(category, msg, ...)	if (PIT_ENGINE_LOGGING && Pit::Debug::Logging::LoggerInitialized) { PIT_ENGINE_ERR(category, msg, ##__VA_ARGS__); throw std::runtime_error(msg); while (true){} }

// Game logging
#define PIT_TRACE(msg, ...)			SPDLOG_LOGGER_TRACE(Pit::Debug::Logging::GetGameLogger(), msg, ##__VA_ARGS__)
#define PIT_INFO(msg, ...)			SPDLOG_LOGGER_INFO(Pit::Debug::Logging::GetGameLogger(), msg, ##__VA_ARGS__)
#define PIT_WARN(msg, ...)			SPDLOG_LOGGER_WARN(Pit::Debug::Logging::GetGameLogger(), msg, ##__VA_ARGS__)
#define PIT_ERR(msg, ...)			SPDLOG_LOGGER_ERROR(Pit::Debug::Logging::GetGameLogger(), msg, ##__VA_ARGS__)
#define PIT_FATAL(msg, ...)			{ PIT_ERR(msg, ##__VA_ARGS__); throw std::runtime_error(msg); while (true){} }