#include "pch.hpp"
#include "Singleton.hpp"

namespace Pit::DesignPatterns {
	template<typename Self>
	Self* Singleton<Self>::s_Instance = nullptr;

	template<typename Self>
	Singleton<Self>::Singleton() {
		if (s_Instance) {
			PIT_ENGINE_WARN(General, "Trying to Instanciate a " + typeid(Self).name() + "Singleton, even though there already is a instance!");
			delete this;
		}
		else s_Instance = this;
	}

	template<typename Self>
	Self* Singleton<Self>::Inst() {
		if (s_Instance)
			return s_Instance;
		else {
			s_Instance = new Self();
			return s_Instance;
		}
	}
}