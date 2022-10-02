#pragma once

#include "ECS/ECSInclude.hpp"
#include "ECS/ECSSystem.hpp"
#include "ECS/ECSWorld.hpp"
#include "ECS/Commons/ECSTransform2DComponent.hpp"

namespace Pit::ECS {
	class ExampleMovementSystem {
	public:
		static SystemTopic GetTopic() { return SystemTopic::General; }
		static int32_t GetExecuteOrder() { return 0; }
		static void Update(World& world, float DeltaTime) {
			PIT_ENGINE_INFO("Example Update");

			auto view = world.View<Transform2DComponent>();
			for (auto e : view) {
				auto& transform = view.get<Transform2DComponent>(e);

			}
		}
	};
}