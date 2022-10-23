#include "pch.hpp"
#include "Logging.hpp"
#if PIT_ENGINE_DEBUG || PIT_ENGINE_RELEASE
#include "ProfileStats.hpp"
#endif

using namespace Pit::Debug;

std::shared_ptr<spdlog::sinks::basic_file_sink_mt> Logging::s_FileSink;

std::shared_ptr<spdlog::logger> Logging::s_EngineLogger;
std::shared_ptr<spdlog::logger> Logging::s_GameLogger;

bool Logging::LoggerInitialized = false;

static std::ofstream PerformanceReportFile;

void Logging::Init() {
#if PIT_ENGINE_DEBUG || PIT_ENGINE_RELEASE
	constexpr const char* pattern = "%^%v%$";
	spdlog::set_pattern(pattern);

	// Destroy the 10th saved log file if over 10
	size_t filecount = std::distance(std::filesystem::directory_iterator("Logs"), std::filesystem::directory_iterator{});
	if (filecount >= 10) {
		for (auto& directoryEntry : std::filesystem::directory_iterator("Logs")) {
			if (filecount > 10) {
				std::filesystem::remove(directoryEntry.path());
				filecount--;
			}
		}
	}
	std::string filepath = std::string("Logs/") + CurrentTimeToString() + std::string("_Log.log");

	s_FileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(filepath);
	s_FileSink->set_pattern(pattern);
	s_FileSink->set_level(spdlog::level::trace);

	s_EngineLogger = spdlog::stdout_color_mt("Engine");
	s_EngineLogger->set_level(spdlog::level::trace);
	s_EngineLogger->sinks().push_back(s_FileSink);


	s_GameLogger = spdlog::stdout_color_mt("Game");
	s_GameLogger->set_level(spdlog::level::trace);
	s_GameLogger->sinks().push_back(s_FileSink);

	LoggerInitialized = true;

	PerformanceReportFile = std::ofstream(std::string("Logs/") + CurrentTimeToString() + std::string("_ProfilingReport.log"));
#endif
}

void Logging::Shutdown() {
#if PIT_ENGINE_DEBUG || PIT_ENGINE_RELEASE
	spdlog::shutdown();

	s_FileSink.reset();

	s_EngineLogger.reset();
	s_GameLogger.reset();
	
	LoggerInitialized = false;

	PerformanceReportFile << ProfileStatGroups::InfoToString();
	PerformanceReportFile.close();
#endif
}
