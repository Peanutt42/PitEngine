#pragma once

#include "Core/CoreInclude.hpp"
#include <random>
#include <glm/glm.hpp>

namespace Pit {
	/// <summary>
	/// Simple helpers to easily get random values
	/// </summary>
	class Random {
	public:
		static int Int();

		static uint32 UInt();

		static uint32 UInt(uint32 min, uint32 max);

		static float Float();

		static glm::vec3 Vec3();

		static glm::vec3 Vec3(float min, float max);

		static glm::vec3 InUnitSphere();

	private:
		static std::mt19937 s_RandomEngine;
		static std::uniform_int_distribution<std::mt19937::result_type> s_Distribution;
	};
}