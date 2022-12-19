#pragma once

#include "Core/CoreInclude.hpp"

namespace Pit::DesignPatterns {
	template<typename Self>
	class Singleton {
	public:
		Singleton() {
			if (s_Instance) {
				PIT_ENGINE_WARN(General, "Trying to Instanciate a " + typeid(Self).name() + "Singleton, even though there already is a instance!");
				delete this;
			}
			else s_Instance = this;
		}
		virtual ~Singleton() = 0;

		static Self* Inst() {
			if (s_Instance)
				return s_Instance;
			else {
				s_Instance = new Self();
				return s_Instance;
			}
		}

	private:
		static Self* s_Instance;
	};
}