#include "pch.hpp"
#include "Time.hpp"
#include <chrono>

namespace Pit {
	float Time::s_DeltaTime = 0;
	int Time::s_Frame = 0;
	
	const float Time::DeltaTime() { return s_DeltaTime; }
	void Time::SetDeltaTime(float deltaTime) { s_DeltaTime = deltaTime; }

	const int Time::Frame() { return s_Frame; }
	void Time::SetFrame(int frame) { s_Frame = frame; }

	void Time::Sleep(uint64_t milliseconds) {
		using namespace std::chrono;
		time_point<high_resolution_clock> start = high_resolution_clock::now();
		while ((uint64_t)(duration_cast<std::chrono::milliseconds>(high_resolution_clock::now() - start).count()) < milliseconds) {}
	}
	
	void Time::MicroSleep(uint64_t microseconds) {
		using namespace std::chrono;
		time_point<high_resolution_clock> start = high_resolution_clock::now();
		while ((uint64_t)(duration_cast<std::chrono::microseconds>(high_resolution_clock::now() - start).count()) < microseconds) {}
	}
}