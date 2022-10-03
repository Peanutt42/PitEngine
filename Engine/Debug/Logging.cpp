#include "Logging.hpp"

using namespace Pit::Debug;

std::shared_ptr<spdlog::logger> Logging::s_EngineLogger;
std::shared_ptr<spdlog::logger> Logging::s_GameLogger;
bool Logging::LoggerInitialized = false;

void Logging::Init() {
	spdlog::set_pattern("%^%n: %v%$");

	s_EngineLogger = spdlog::stdout_color_mt("Engine");
	s_EngineLogger->set_level(spdlog::level::trace);
	s_GameLogger = spdlog::stdout_color_mt("Game");
	s_GameLogger->set_level(spdlog::level::trace);

	LoggerInitialized = true;
}

void Logging::Shutdown() {
	spdlog::shutdown();
	LoggerInitialized = false;
}
