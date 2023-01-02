#include "pch.hpp"
#include "Logging.hpp"

namespace Pit::Debug {

	ConsoleLoggerMT* Logging::s_EngineLogger;
	ConsoleLoggerMT* Logging::s_GameLogger;

	std::ofstream Logging::s_LogFile;
	std::string Logging::s_LastErrorMsg;

	bool Logging::LoggerInitialized = false;

	void Logging::Init() {
#if DEBUG || RELEASE
		// Destroy the 10th saved log file if over 10
		size filecount = std::distance(std::filesystem::directory_iterator("Logs"), std::filesystem::directory_iterator{});
		if (filecount >= 10) {
			for (auto& directoryEntry : std::filesystem::directory_iterator("Logs")) {
				if (filecount > 10 && !directoryEntry.is_directory()) {
					std::filesystem::remove(directoryEntry.path());
					filecount--;
				}
			}
		}
		Pit::String filepath = Pit::String("Logs/") + CurrentTimeToString() + Pit::String("_Log.log");
		s_LogFile.open(filepath);
		PIT_ENGINE_ASSERT(General, s_LogFile.is_open(), "Failed to open new Logfile!");

		s_EngineLogger = new ConsoleLoggerMT("Engine", ConsoleColor::Green);
		s_GameLogger = new ConsoleLoggerMT("Game", ConsoleColor::Green);

		auto logToFileFunc = []([[maybe_unused]] LogVerbosity verbosity, const String& msg) { if (s_LogFile.is_open()) s_LogFile << msg << '\n'; };
		auto copyLastErrorMsgFunc = [](LogVerbosity verbosity, const String& msg) { if (verbosity >= LogVerbosity::Error) s_LastErrorMsg = msg; };
		s_EngineLogger->OnLogEvent += logToFileFunc;
		s_EngineLogger->OnLogEvent += copyLastErrorMsgFunc;
		s_GameLogger->OnLogEvent += logToFileFunc;
		s_GameLogger->OnLogEvent += copyLastErrorMsgFunc;

		LoggerInitialized = true;
#endif
	}

	void Logging::Shutdown() {
#if DEBUG || RELEASE
		if (s_LogFile.is_open()) s_LogFile.close();

		s_EngineLogger->SetColor(ConsoleColor::White);
		s_GameLogger->SetColor(ConsoleColor::White);

		delete s_EngineLogger;
		delete s_GameLogger;

		LoggerInitialized = false;
#endif
	}
	
	ConsoleLoggerMT* Logging::GetEngineLogger() { return s_EngineLogger; }
	ConsoleLoggerMT* Logging::GetGameLogger() { return s_GameLogger; }
}