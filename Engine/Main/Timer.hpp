#pragma once

#include <iostream>
#include <string>
#include <chrono>
#include "Debug/Logging.hpp"

namespace Pit {

#define QUICK_SCOPE_PROFILE(name)	\
	ScopedTimer __scopeTimer(name);

	class Timer {
	public:
		Timer() {
			Reset();
		}

		inline void Reset() {
			m_Start = std::chrono::high_resolution_clock::now();
		}

		inline float Elapsed() {
			return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - m_Start).count() * 0.001f * 0.001f * 0.001f;
		}

		inline float ElapsedMillis() {
			return Elapsed() * 1000.0f;
		}

	private:
		std::chrono::time_point<std::chrono::high_resolution_clock> m_Start;
	};

	class ScopedTimer {
	public:
		ScopedTimer(const std::string& name)
			: m_Name(name) {}
		~ScopedTimer() {
			float time = m_Timer.ElapsedMillis();
			PIT_ENGINE_INFO("[TIMER] {0:s} - {1:f}ms, {2:f}fps", m_Name, time, 1 / time * 1000);
		}
	private:
		std::string m_Name;
		Timer m_Timer;
	};



}