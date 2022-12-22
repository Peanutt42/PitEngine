#pragma once

#include "Core/CoreInclude.hpp"

namespace Pit::DesignPatterns {
	template<typename Self>
	class Singleton {
	public:
		Singleton();
		virtual ~Singleton() = 0;

		static Self* Inst();

	private:
		static Self* s_Instance;
	};
}