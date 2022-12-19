#include "pch.hpp"
#include "Singleton.hpp"

namespace Pit::DesignPatterns {
	template<typename Self>
	Self* Singleton<Self>::s_Instance = nullptr;
}