#pragma once
#include <glad/glad.h>

#include "Core/CoreInclude.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Pit::Rendering {
    class Camera {
    public:
        glm::vec3 Position;
        glm::vec3 Front;
        glm::vec3 Up;
        glm::vec3 Right;
        glm::vec3 WorldUp;

        float Yaw;
        float Pitch;

        float MovementSpeed;
        float MouseSensitivity;
        float Fov;

        Camera(glm::vec3 position = glm::vec3(0, 0, 0), glm::vec3 up = glm::vec3(0, 1, 0), float yaw = -90.0f, float pitch = 0.f);

        glm::mat4 GetViewMatrix();

        void ProcessKeyboard(const glm::vec3& direction);

        void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);

        void ProcessMouseScroll(float yoffset);

    private:
        void _UpdateCameraVectors();
    };
}