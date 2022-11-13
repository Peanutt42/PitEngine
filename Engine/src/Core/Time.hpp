#pragma once

#include <iostream>
#include <string>
#include <chrono>
#include "Debug/Logging.hpp"

namespace Pit {
	class Time {
	public:
		static const float& DeltaTime() { return s_DeltaTime; }
		static void SetDeltaTime(float DeltaTime) { s_DeltaTime = DeltaTime; }

		static const int& Frame() { return s_Frame; }
		static void SetFrame(int Frame) { s_Frame = Frame; }

	private:
		static float s_DeltaTime;
		static int s_Frame;
	};


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

		inline float ElapsedSeconds() {
			return ElapsedMillis() / 1000.f;
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
			PIT_ENGINE_INFO(General, "[TIMER] {0:3s} - {1:f}s, {2:f}ms, {3:f}fps", m_Name, time / 1000.f, time, 1 / time * 1000);
		}
	private:
		std::string m_Name;
		Timer m_Timer;
	};
}