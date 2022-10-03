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
			auto view = world.View<TransformComponent>();
			for (auto e : view) {
				auto& transform = view.get<TransformComponent>(e);

				transform.rotation.y = glm::mod(transform.rotation.y + Time::DeltaTime * 1, glm::two_pi<float>());
				transform.rotation.x = glm::mod(transform.rotation.x + Time::DeltaTime * 1, glm::two_pi<float>());
			}
		}
	};
}