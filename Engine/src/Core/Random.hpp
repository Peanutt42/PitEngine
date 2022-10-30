#pragma once

#include "Core/CoreInclude.hpp"
#include <random>
#include <glm/glm.hpp>

namespace Pit {
	class Random {
	public:
		static int Int() {
			return Cast<int>(UInt() - (MAX_UINT32 / 2));
		}

		static uint32 UInt() {
			return s_Distribution(s_RandomEngine);
		}

		static uint32 UInt(uint32 min, uint32 max) {
			return min + (s_Distribution(s_RandomEngine) % (max - min + 1));
		}

		static float Float() {
			return (float)s_Distribution(s_RandomEngine) / (float)MAX_UINT32;
		}

		static glm::vec3 Vec3() {
			return glm::vec3(Float(), Float(), Float());
		}

		static glm::vec3 Vec3(float min, float max) {
			return glm::vec3(Float() * (max - min) + min, Float() * (max - min) + min, Float() * (max - min) + min);
		}

		static glm::vec3 InUnitSphere() {
			return glm::normalize(Vec3(-1.0f, 1.0f));
		}

	private:
		static std::mt19937 s_RandomEngine;
		static std::uniform_int_distribution<std::mt19937::result_type> s_Distribution;
	};
}