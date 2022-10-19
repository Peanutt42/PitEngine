#include "Main/CoreInclude.hpp"
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