#include "Logging.hpp"

using namespace Pit::Debug;

std::shared_ptr<spdlog::logger> Logging::s_EngineLogger;
std::shared_ptr<spdlog::logger> Logging::s_GameLogger;

void Logging::Init() {
	spdlog::set_pattern("%^[%T] %n: %v%$");

	s_EngineLogger = spdlog::stdout_color_mt("Engine");
	s_EngineLogger->set_level(spdlog::level::trace);
	s_GameLogger = spdlog::stdout_color_mt("Game");
	s_GameLogger->set_level(spdlog::level::trace);
}

void Logging::Shutdown() {
	spdlog::shutdown();
}
