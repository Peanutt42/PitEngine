#pragma once

#include "Core/CoreInclude.hpp"
#include "ECS/ECSComponent.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace Pit::ECS {
    struct NameComponent : public Component {
        String Name;
    };

    struct UUIDComponent : public Component {
        UUID Id;
    };

    struct TransformComponent : public Component {
        glm::vec3 position{};
        glm::vec3 rotation{};
        glm::vec3 scale{ 1.f, 1.f, 1.f };

        glm::mat4 mat4() {
            // Simplified but slower:
            // auto transform = glm::translate(glm::mat4(1), position);

            // transform = glm::rotate(transform, rotation.y, {0, 1, 0});
            // transform = glm::rotate(transform, rotation.x, {1, 0, 0});
            // transform = glm::rotate(transform, rotation.z, {0, 0, 1});

            // transform = glm::scale(transform, scale);
            // return transform;
            
            const float c3 = glm::cos(rotation.z);
            const float s3 = glm::sin(rotation.z);
            const float c2 = glm::cos(rotation.x);
            const float s2 = glm::sin(rotation.x);
            const float c1 = glm::cos(rotation.y);
            const float s1 = glm::sin(rotation.y);
            return glm::mat4{
                {
                    scale.x * (c1 * c3 + s1 * s2 * s3),
                    scale.x * (c2 * s3),
                    scale.x * (c1 * s2 * s3 - c3 * s1),
                    0,
                },
                {
                    scale.y * (c3 * s1 * s2 - c1 * s3),
                    scale.y * (c2 * c3),
                    scale.y * (c1 * c3 * s2 + s1 * s3),
                    0,
                },
                {
                    scale.z * (c2 * s1),
                    scale.z * (-s2),
                    scale.z * (c1 * c2),
                    0,
                },
                {position.x, position.y, position.z, 1} };
        }

        glm::mat3 normalMatrix() {
            const float c3 = glm::cos(rotation.z);
            const float s3 = glm::sin(rotation.z);
            const float c2 = glm::cos(rotation.x);
            const float s2 = glm::sin(rotation.x);
            const float c1 = glm::cos(rotation.y);
            const float s1 = glm::sin(rotation.y);
            const glm::vec3 invScale = 1.f / scale;
            return glm::mat3{
                {
                    invScale.x * (c1 * c3 + s1 * s2 * s3),
                    invScale.x * (c2 * s3),
                    invScale.x * (c1 * s2 * s3 - c3 * s1),
                },
                {
                    invScale.y * (c3 * s1 * s2 - c1 * s3),
                    invScale.y * (c2 * c3),
                    invScale.y * (c1 * c3 * s2 + s1 * s3),
                },
                {
                    invScale.z * (c2 * s1),
                    invScale.z * (-s2),
                    invScale.z * (c1 * c2),
                } };
        }
    };
}