#pragma once

#include "Core/CoreInclude.hpp"
#include <entt.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Pit::ECS {
    struct NameComponent {
        String Name;
    };

    struct UUIDComponent {
        UUID Id;
    };

    struct TransformComponent {
        glm::vec3 Position{};
        glm::vec3 Rotation{};
        glm::vec3 Scale{ 1.f, 1.f, 1.f };

        glm::mat4 GetTransform();
    };

    struct CSharpComponent {
        String ClassName;
    };



	[[maybe_unused]] static void RegisterComponents(entt::registry* reg) {
		entt::entity registerEntity = reg->create();
	#define PIT_REGISTER_COMPONENT(component) reg->emplace<component>(registerEntity); reg->remove<component>(registerEntity)
		PIT_REGISTER_COMPONENT(NameComponent);
		PIT_REGISTER_COMPONENT(UUIDComponent);
		PIT_REGISTER_COMPONENT(TransformComponent);
		PIT_REGISTER_COMPONENT(CSharpComponent);
		reg->destroy(registerEntity);
	}
}