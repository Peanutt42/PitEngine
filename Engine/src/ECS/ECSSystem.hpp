#pragma once

#include "Main/CoreInclude.hpp"

namespace Pit::ECS {
	enum SystemTopic {
		None,
		Physic,
		Networking,
		General,
		Gameplay,
		Render
	};

	using SystemIndex = int32_t;

	static SystemIndex SystemCounter = 0;
	template<typename T>
	SystemIndex GetSystemIndex() {
		static SystemIndex systemIndex = SystemCounter++;
		return systemIndex;
	}

	template<typename BaseSystem>
	struct SystemExecuteOrderBuilder {
		int32_t Build() {

			int32_t result = 99999;
			for (auto beforeIndex : beforeSystems)
				if (beforeIndex <= result)
					result = beforeIndex - 1;

			for (auto afterIndex : afterSystems)
				if (afterIndex >= result)
					result = afterIndex + 1;
			
			return result;
		}

		template<typename... T>
		SystemExecuteOrderBuilder& ExecuteBefore() {
			([&]() {
				if (T::GetTopic() <= BaseSystem::GetTopic())
					beforeSystems.push_back(T::GetExecuteOrder());
			}(), ...);
			return *this;
		}
		template<typename... T>
		SystemExecuteOrderBuilder& ExecuteAfter() {
			([&]() {
				if (T::GetTopic() >= BaseSystem::GetTopic())
				afterSystems.push_back(T::GetExecuteOrder());
			}(), ...);
			return *this;
		}

	private:
		std::vector<int32_t> beforeSystems;
		std::vector<int32_t> afterSystems;
	};
}