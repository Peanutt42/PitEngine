#include "pch.hpp"
#include "PhysicsSubmodule.hpp"

using namespace Pit;

void PhysicsSubmodule::Init() {
	PIT_PROFILE_FUNCTION();

}

void PhysicsSubmodule::Shutdown() {
	PIT_PROFILE_FUNCTION();

}

void PhysicsSubmodule::Update() {
	PIT_PROFILE_FUNCTION();

	using namespace std::chrono;
	static time_point<high_resolution_clock> currentTime = high_resolution_clock::now();
	static float accumulator = 0;

	time_point<high_resolution_clock> newTime = high_resolution_clock::now();
	auto frameTime = duration_cast<nanoseconds>(newTime - currentTime).count() * 0.000000001f;
	currentTime = newTime;

	accumulator += frameTime;

	while (accumulator >= m_FixedTimestep) {
		PIT_PROFILE_FUNCTION("Pit::PhysicsSubmodule::Update::Step");

		// Update Physics
		bool sleep = true;
		auto start = std::chrono::system_clock::now();
		while (sleep) {
			auto now = std::chrono::system_clock::now();
			auto elapsed = duration_cast<nanoseconds>(now - start) * 0.000000001f;
			if (elapsed.count() > 0.001f/*1ms*/)
				sleep = false;
		}

		accumulator -= m_FixedTimestep;
	}
}