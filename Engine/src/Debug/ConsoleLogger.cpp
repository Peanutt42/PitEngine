#include "pch.hpp"
#include "ConsoleLogger.hpp"
#if PIT_WINDOWS
#include <windows.h>
#endif

namespace Pit::Debug {
	ConsoleLoggerMT::ConsoleLoggerMT(const std::string& name, ConsoleColor consoleColor, LogVerbosity verbosity)
		: m_Name(name), m_Verbosity(verbosity), m_Console(GetStdHandle(STD_OUTPUT_HANDLE)) {

		SetColor(consoleColor);
	}

	void ConsoleLoggerMT::SetColor(ConsoleColor consoleColor) {
		#if PIT_WINDOWS
		SetConsoleTextAttribute(m_Console, Cast<WORD>(consoleColor));
		#endif
	}
	
	void ConsoleLoggerMT::SetLevel(LogVerbosity newVerbosity) {
		m_Verbosity = newVerbosity;
	}
	
	
	ConsoleLoggerST::ConsoleLoggerST(const std::string& name, ConsoleColor consoleColor, LogVerbosity verbosity) 
		: m_Name(name), m_Verbosity(verbosity), m_Console(GetStdHandle(STD_OUTPUT_HANDLE)) {

		SetColor(consoleColor);
	}

	void ConsoleLoggerST::SetColor(ConsoleColor consoleColor) {
		#if PIT_WINDOWS
		SetConsoleTextAttribute(m_Console, Cast<WORD>(consoleColor));
		#endif
	}

	void ConsoleLoggerST::SetLevel(LogVerbosity newVerbosity) {
		m_Verbosity = newVerbosity;
	}
}