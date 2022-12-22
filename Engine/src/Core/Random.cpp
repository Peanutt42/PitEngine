#include "pch.hpp"
#include "Random.hpp"

namespace Pit {
	std::mt19937 Random::s_RandomEngine{ Cast<uint>(std::random_device()()) };
	std::uniform_int_distribution<std::mt19937::result_type> Random::s_Distribution;
	
	int Random::Int() {
		return Cast<int>(UInt() - (MAX_UINT32 / 2));
	}

	uint32 Random::UInt() {
		return s_Distribution(s_RandomEngine);
	}
	
	uint32 Random::UInt(uint32 min, uint32 max) {
		return min + (s_Distribution(s_RandomEngine) % (max - min + 1));
	}

	float Random::Float() {
		return (float)s_Distribution(s_RandomEngine) / (float)MAX_UINT32;
	}
	
	glm::vec3 Random::Vec3() {
		return glm::vec3(Float(), Float(), Float());
	}
	
	glm::vec3 Random::Vec3(float min, float max) {
		return glm::vec3(Float() * (max - min) + min, Float() * (max - min) + min, Float() * (max - min) + min);
	}
	
	glm::vec3 Random::InUnitSphere() {
		return glm::normalize(Vec3(-1.0f, 1.0f));
	}
}