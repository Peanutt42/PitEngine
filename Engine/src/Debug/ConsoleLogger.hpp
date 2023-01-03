#pragma once

#include "Core\CoreInclude.hpp"
#include "Core\Event.hpp"
#include "fmt\fmt.h"

namespace Pit::Debug {
	enum class ConsoleColor {
		Red = 12,
		Green = 10,
		Yellow = 14,
		Blue = 9,
		Magenta = 13,
		Cyan = 11,
		White = 15
	};

	enum class LogVerbosity {
		Trace,
		Info,
		Warning,
		Error,
		Critical
	};

	struct OnLogEvent : public Event<LogVerbosity, const std::string&> {
		virtual void Invoke(LogVerbosity verbosity, const std::string& msg) {
			PIT_PROFILE_FUNCTION();

			for (auto& callback : m_Listeners)
				callback(verbosity, msg);
		}
	};

	class ConsoleLoggerMT {
	public:
		ConsoleLoggerMT(const std::string& name, ConsoleColor consoleColor = ConsoleColor::White, LogVerbosity verbosity = LogVerbosity::Trace);

		void SetColor(ConsoleColor consoleColor);

		void SetLevel(LogVerbosity newVerbosity);

		template<typename... T>
		void Log(LogVerbosity verbosity, fmt::format_string<T...> msg, T... args) {
			if (verbosity >= m_Verbosity) {
				std::scoped_lock lock(m_LogMutex);
				switch (verbosity) {
				default:
				case LogVerbosity::Trace: SetColor(ConsoleColor::White); break;
				case LogVerbosity::Info: SetColor(ConsoleColor::Green); break;
				case LogVerbosity::Warning: SetColor(ConsoleColor::Yellow); break;
				case LogVerbosity::Error:
				case LogVerbosity::Critical: SetColor(ConsoleColor::Red); break;
				}
				std::string formatted = fmt::format(msg, std::forward<T>(args)...);
				std::cout << formatted << '\n';
				OnLogEvent.Invoke(verbosity, formatted);
			}
		}

		template<typename... T>
		void Trace(fmt::format_string<T...> msg, T... args) {
			Log(LogVerbosity::Trace, msg, std::forward<T>(args)...);
		}
		template<typename... T>
		void Info(fmt::format_string<T...> msg, T... args) {
			Log(LogVerbosity::Info, msg, std::forward<T>(args)...);
		}
		template<typename... T>
		void Warn(fmt::format_string<T...> msg, T... args) {
			Log(LogVerbosity::Warning, msg, std::forward<T>(args)...);
		}
		template<typename... T>
		void Error(fmt::format_string<T...> msg, T... args) {
			Log(LogVerbosity::Error, msg, std::forward<T>(args)...);
		}
		template<typename... T>
		void Critical(fmt::format_string<T...> msg, T... args) {
			Log(LogVerbosity::Critical, msg, std::forward<T>(args)...);
		}

		OnLogEvent OnLogEvent;

	private:
		std::string m_Name;
		LogVerbosity m_Verbosity;
		std::mutex m_LogMutex;
		void* m_Console;
	};

	class ConsoleLoggerST {
	public:
		ConsoleLoggerST(const std::string& name, ConsoleColor consoleColor = ConsoleColor::White, LogVerbosity verbosity = LogVerbosity::Trace);

		void SetColor(ConsoleColor consoleColor);

		void SetLevel(LogVerbosity newVerbosity);

		template<typename... T>
		void Log(LogVerbosity verbosity, [[maybe_unused]] fmt::format_string<T...> msg, T... args) {
			if (verbosity >= m_Verbosity) {
				switch (verbosity) {
				default:
				case LogVerbosity::Trace: SetColor(ConsoleColor::White);
				case LogVerbosity::Info: SetColor(ConsoleColor::Green);
				case LogVerbosity::Warning: SetColor(ConsoleColor::Magenta);
				case LogVerbosity::Error:
				case LogVerbosity::Critical: SetColor(ConsoleColor::Red);
				}
				std::string formatted = fmt::format(msg, std::forward<T>(args)...);
				std::cout << formatted << '\n';
				OnLogEvent.Invoke(verbosity, formatted);
			}
		}

		template<typename... T>
		void Trace(fmt::format_string<T...> msg, T... args) {
			Log(LogVerbosity::Trace, msg, std::forward<T>(args)...);
		}
		template<typename... T>
		void Info(fmt::format_string<T...> msg, T... args) {
			Log(LogVerbosity::Info, msg, std::forward<T>(args)...);
		}
		template<typename... T>
		void Warn(fmt::format_string<T...> msg, T... args) {
			Log(LogVerbosity::Warning, msg, std::forward<T>(args)...);
		}
		template<typename... T>
		void Error(fmt::format_string<T...> msg, T... args) {
			Log(LogVerbosity::Error, msg, std::forward<T>(args)...);
		}
		template<typename... T>
		void Critical(fmt::format_string<T...> msg, T... args) {
			Log(LogVerbosity::Critical, msg, std::forward<T>(args)...);
		}

		OnLogEvent OnLogEvent;

	private:
		std::string m_Name;
		LogVerbosity m_Verbosity;
		void* m_Console;
	};
}