#include "Logging.hpp"

using namespace Pit::Debug;

std::shared_ptr<spdlog::logger> Logging::s_EngineLogger_General;
std::shared_ptr<spdlog::logger> Logging::s_EngineLogger_Audio;
std::shared_ptr<spdlog::logger> Logging::s_EngineLogger_Networking;
std::shared_ptr<spdlog::logger> Logging::s_EngineLogger_ECS;
std::shared_ptr<spdlog::logger> Logging::s_EngineLogger_Physics;
std::shared_ptr<spdlog::logger> Logging::s_EngineLogger_Rendering;

std::shared_ptr<spdlog::logger> Logging::s_GameLogger;
bool Logging::LoggerInitialized = false;

void Logging::Init() {
	spdlog::set_pattern("%^[%n]: %v%$");//"%^%n: %v%$");
	s_EngineLogger_General = spdlog::stdout_color_mt("Engine-General");
	s_EngineLogger_General->set_level(spdlog::level::trace);

	s_EngineLogger_Audio = spdlog::stdout_color_mt("Engine-Audio");
	s_EngineLogger_Audio->set_level(spdlog::level::trace);

	s_EngineLogger_Networking = spdlog::stdout_color_mt("Engine-Networking");
	s_EngineLogger_Networking->set_level(spdlog::level::trace);

	s_EngineLogger_ECS = spdlog::stdout_color_mt("Engine-ECS");
	s_EngineLogger_ECS->set_level(spdlog::level::trace);

	s_EngineLogger_Physics = spdlog::stdout_color_mt("Engine-Physics");
	s_EngineLogger_Physics->set_level(spdlog::level::trace);

	s_EngineLogger_Rendering = spdlog::stdout_color_mt("Engine-Rendering");
	s_EngineLogger_Rendering->set_level(spdlog::level::trace);


	s_GameLogger = spdlog::stdout_color_mt("Game");
	s_GameLogger->set_level(spdlog::level::trace);

	LoggerInitialized = true;
}

void Logging::Shutdown() {
	spdlog::shutdown();
	s_EngineLogger_General.reset();
	s_EngineLogger_Audio.reset();
	s_EngineLogger_Networking.reset();
	s_EngineLogger_ECS.reset();
	s_EngineLogger_Physics.reset();
	s_EngineLogger_Rendering.reset();
	
	s_GameLogger.reset();
	
	LoggerInitialized = false;
}
