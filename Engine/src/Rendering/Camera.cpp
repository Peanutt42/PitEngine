#include "pch.hpp"
#include "Camera.hpp"

namespace Pit::Rendering {

    Camera::Camera(glm::vec3 position, glm::vec3 up)
        : Front(glm::vec3(0, 0, -1)), Rotation(-90, 0, 0), MovementSpeed(2.5f), MouseSensitivity(.1f), Fov(90) {

        Position = position;
        WorldUp = up;

        _UpdateCameraVectors();
    }

    glm::mat4 Camera::GetViewMatrix() {
        return glm::lookAt(Position, Position + Front, Up);
    }

    void Camera::ProcessKeyboard(const glm::vec3& direction) {
        float velocity = MovementSpeed * Time::DeltaTime();
        if (direction == glm::vec3(0, 1, 0))
            Position += Front * velocity;
        if (direction == glm::vec3(0, -1, 0))
            Position -= Front * velocity;
        if (direction == glm::vec3(1, 0, 0))
            Position -= Right * velocity;
        if (direction == glm::vec3(-1, 0, 0))
            Position += Right * velocity;
    }

    void Camera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch) {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Rotation.x += xoffset;
        Rotation.y += yoffset;

        if (constrainPitch)
            Rotation.y = Math::Clamp(Rotation.y, -90, 90);

        _UpdateCameraVectors();
    }

    void Camera::ProcessMouseScroll(float yoffset) {
        Fov -= (float)yoffset;
        if (Fov < 1)
            Fov = 1;
        if (Fov > 180)
            Fov = 180;
    }

    void Camera::_UpdateCameraVectors() {
        glm::vec3 front;
        front.x = cos(glm::radians(Rotation.x)) * cos(glm::radians(Rotation.y));
        front.y = sin(glm::radians(Rotation.y));
        front.z = sin(glm::radians(Rotation.x)) * cos(glm::radians(Rotation.y));
        Front = glm::normalize(front);
        Right = glm::normalize(glm::cross(Front, WorldUp));
        Up = glm::normalize(glm::cross(Right, Front));
    }
}