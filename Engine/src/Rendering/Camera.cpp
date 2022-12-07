#include "pch.hpp"
#include "Camera.hpp"

namespace Pit::Rendering {

    Camera::Camera(glm::vec3 position)
        : Rotation(-90, 0, 0), Fov(90) {

        Position = position;

        _UpdateCameraVectors();
    }

    glm::mat4 Camera::GetViewMatrix() {
        glm::vec3 front;
        front.x = cos(glm::radians(Rotation.x)) * cos(glm::radians(Rotation.y));
        front.y = sin(glm::radians(Rotation.y));
        front.z = sin(glm::radians(Rotation.x)) * cos(glm::radians(Rotation.y));
        front = glm::normalize(front);
        return glm::lookAt(Position, Position + front, glm::vec3(0, 1, 0));
    }

    void Camera::_UpdateCameraVectors() {
        /*glm::vec3 front;
        front.x = cos(glm::radians(Rotation.x)) * cos(glm::radians(Rotation.y));
        front.y = sin(glm::radians(Rotation.y));
        front.z = sin(glm::radians(Rotation.x)) * cos(glm::radians(Rotation.y));
        Front = glm::normalize(front);
        Right = glm::normalize(glm::cross(Front, WorldUp));
        Up = glm::normalize(glm::cross(Right, Front));*/
    }
}