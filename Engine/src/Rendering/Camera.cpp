#include "Core/CoreInclude.hpp"
#include "Camera.hpp"

using namespace Pit::Rendering;

void Camera::SetProjection() {
	if (ProjectionMode == Projection::Perspective) {
		if (glm::abs(AspectRatio) <= 0.0f) PIT_ENGINE_ERR(Log::Rendering, "Camera-aspect should be above 0");
		float FOV_rad = glm::radians(FOV);
		const float tanHalfFovy = tan(FOV_rad / 2.f);
		m_ProjectionMatrix = glm::mat4{ 0.0f };
		m_ProjectionMatrix[0][0] = 1.f / (AspectRatio * tanHalfFovy);
		m_ProjectionMatrix[1][1] = 1.f / (tanHalfFovy);
		m_ProjectionMatrix[2][2] = FarClip / (FarClip - NearClip);
		m_ProjectionMatrix[2][3] = 1.f;
		m_ProjectionMatrix[3][2] = -(FarClip * NearClip) / (FarClip - NearClip);
	}
	else {
		m_ProjectionMatrix = glm::mat4{ 1.0f };
		m_ProjectionMatrix[0][0] = 2.f / (Right - Left);
		m_ProjectionMatrix[1][1] = 2.f / (Bottom - Top);
		m_ProjectionMatrix[2][2] = 1.f / (FarClip - NearClip);
		m_ProjectionMatrix[3][0] = -(Right + Left) / (Right - Left);
		m_ProjectionMatrix[3][1] = -(Bottom + Top) / (Bottom - Top);
		m_ProjectionMatrix[3][2] = -NearClip / (FarClip - NearClip);
	}
}

void Camera::SetViewDirection(const glm::vec3& position, const glm::vec3& direction, const glm::vec3& up) {
	const glm::vec3 w{ glm::normalize(direction) };
	const glm::vec3 u{ glm::normalize(glm::cross(w, up)) };
	const glm::vec3 v{ glm::cross(w, u) };

	m_ViewMatrix = glm::mat4{ 1.f };
	m_ViewMatrix[0][0] = u.x;
	m_ViewMatrix[1][0] = u.y;
	m_ViewMatrix[2][0] = u.z;
	m_ViewMatrix[0][1] = v.x;
	m_ViewMatrix[1][1] = v.y;
	m_ViewMatrix[2][1] = v.z;
	m_ViewMatrix[0][2] = w.x;
	m_ViewMatrix[1][2] = w.y;
	m_ViewMatrix[2][2] = w.z;
	m_ViewMatrix[3][0] = -glm::dot(u, position);
	m_ViewMatrix[3][1] = -glm::dot(v, position);
	m_ViewMatrix[3][2] = -glm::dot(w, position);
}

void Camera::SetViewTarget(const glm::vec3& position, const glm::vec3& target, const glm::vec3& up) {
	SetViewDirection(position, target - position, up);
}

void Camera::SetViewYXZ(const glm::vec3& position, const glm::vec3& rotation) {
	const float c3 = glm::cos(rotation.z);
	const float s3 = glm::sin(rotation.z);
	const float c2 = glm::cos(rotation.x);
	const float s2 = glm::sin(rotation.x);
	const float c1 = glm::cos(rotation.y);
	const float s1 = glm::sin(rotation.y);
	const glm::vec3 u{ (c1 * c3 + s1 * s2 * s3), (c2 * s3), (c1 * s2 * s3 - c3 * s1) };
	const glm::vec3 v{ (c3 * s1 * s2 - c1 * s3), (c2 * c3), (c1 * c3 * s2 + s1 * s3) };
	const glm::vec3 w{ (c2 * s1), (-s2), (c1 * c2) };
	m_ViewMatrix = glm::mat4{ 1.f };
	m_ViewMatrix[0][0] = u.x;
	m_ViewMatrix[1][0] = u.y;
	m_ViewMatrix[2][0] = u.z;
	m_ViewMatrix[0][1] = v.x;
	m_ViewMatrix[1][1] = v.y;
	m_ViewMatrix[2][1] = v.z;
	m_ViewMatrix[0][2] = w.x;
	m_ViewMatrix[1][2] = w.y;
	m_ViewMatrix[2][2] = w.z;
	m_ViewMatrix[3][0] = -glm::dot(u, position);
	m_ViewMatrix[3][1] = -glm::dot(v, position);
	m_ViewMatrix[3][2] = -glm::dot(w, position);
}