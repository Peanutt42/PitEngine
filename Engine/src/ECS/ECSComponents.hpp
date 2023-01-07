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
}