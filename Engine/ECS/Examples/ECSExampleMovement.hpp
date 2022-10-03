#pragma once

#include "ECS/ECSInclude.hpp"
#include "ECS/ECSSystem.hpp"
#include "ECS/ECSWorld.hpp"
#include "ECS/Commons/ECSTransformComponent.hpp"

namespace Pit::ECS {
	class ExampleMovementSystem {
	public:
		static SystemTopic GetTopic() { return SystemTopic::General; }
		static int32_t GetExecuteOrder() { return 0; }
		static void Update(World& world) {
			PIT_ENGINE_INFO("Example Update");

			auto view = world.View<TransformComponent>();
			for (auto e : view) {
				auto& transform = view.get<TransformComponent>(e);

			}
		}
	};
}