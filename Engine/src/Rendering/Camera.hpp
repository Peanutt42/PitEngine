#pragma once
#include <glad/glad.h>

#include "Core/CoreInclude.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Pit::Rendering {
    /// <summary>
    /// Manager for calculating the needed ViewMatrix
    /// </summary>
    class Camera {
    public:
        glm::vec3 Position;
        glm::vec3 Rotation;

        float Fov;
        float NearPlane = 0.1f, FarPlane = 500;

        Camera(glm::vec3 position = glm::vec3(0, 0, 0));

        glm::mat4 GetViewMatrix();

    private:
        void _UpdateCameraVectors();
    };
}