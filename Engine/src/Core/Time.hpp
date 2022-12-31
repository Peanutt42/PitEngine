#pragma once

#include <iostream>
#include <string>
#include <chrono>
#include "Debug\Logging.hpp"

namespace Pit {
	/// <summary>
	/// Simple Holder to get DeltaTime/current Frame
	/// </summary>
	class Time {
	public:
		static const float DeltaTime();
		static void SetDeltaTime(float deltaTime);

		static const int Frame();
		static void SetFrame(int frame);

		// Waits the specified milliseconds on the current thread
		static void Sleep(uint64_t milliseconds);
		// Waits the specified microseconds on the current thread
		static void MicroSleep(uint64_t microseconds);

	private:
		static float s_DeltaTime;
		static int s_Frame;
	};


#define QUICK_SCOPE_PROFILE(name)	\
	ScopedTimer __scopeTimer(name);

	/// <summary>
	/// Simple Timer class that capsures time between construction and Elapsed()
	/// </summary>
	class Timer {
	public:
		Timer() {
			Reset();
		}

		inline void Reset() {
			m_Start = std::chrono::high_resolution_clock::now();
		}

		inline float Elapsed() {
			return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - m_Start).count() * .001f * .001f * .001f;
		}

		inline float ElapsedMillis() {
			return Elapsed() * 1000;
		}

		inline float ElapsedSeconds() {
			return ElapsedMillis() / 1000;
		}

	private:
		std::chrono::time_point<std::chrono::high_resolution_clock> m_Start;
	};

	/// <summary>
	/// Simple Scope Timer that prints the time it took to get out of the current scope
	/// </summary>
	class ScopedTimer {
	public:
		ScopedTimer(const std::string& name)
			: m_Name(name) {}
		~ScopedTimer() {
			[[maybe_unused]] float time = m_Timer.ElapsedMillis();
			PIT_ENGINE_INFO(General, "[TIMER] {0:3s} - {1:f}ms, {2:f}fps", m_Name, time, 1 / time * 1000);
		}
	private:
		std::string m_Name;
		Timer m_Timer;
	};
}