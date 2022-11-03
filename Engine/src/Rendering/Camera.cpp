#include "pch.hpp"
#include "Camera.hpp"

using namespace Pit;
using namespace Rendering;

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
    : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(2.5f), MouseSensitivity(.1f), Zoom(70.f) {

    Position = position;
    WorldUp = up;
    Yaw = yaw;
    Pitch = pitch;

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

    Yaw += xoffset;
    Pitch += yoffset;

    if (constrainPitch)
        Pitch = Math::Clamp(Pitch, -90.f, 90.f);

    _UpdateCameraVectors();
}

void Camera::ProcessMouseScroll(float yoffset) {
    Zoom -= (float)yoffset;
    if (Zoom < 1.0f)
        Zoom = 1.0f;
    if (Zoom > 45.0f)
        Zoom = 45.0f;
}

void Camera::_UpdateCameraVectors() {
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Front = glm::normalize(front);
    Right = glm::normalize(glm::cross(Front, WorldUp));
    Up = glm::normalize(glm::cross(Right, Front));
}