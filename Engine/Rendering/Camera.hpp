#pragma once

#include "Main/CoreInclude.hpp"
#include "RenderingInclude.hpp"

namespace Pit::Rendering {
	class Camera {
	public:
		void SetPerspectiveProjection(float fov_y, float aspect, float nearClip, float farClip);
		
		void SetOrthographicProjection(float left, float right, float top, float bottom, float nearClip, float farClip);
		
		void SetViewDirection(glm::vec3 position, glm::vec3 direction, glm::vec3 up = { 0.f, -1.f, 0.f });
		void SetViewTarget(glm::vec3 position, glm::vec3 target, glm::vec3 up = { 0.f, -1.f, 0.f });
		void SetViewYXZ(glm::vec3 position, glm::vec3 rotation);

		const glm::mat4 GetProjection() const { return m_ProjectionMatrix; }
		const glm::mat4 GetView() const { return m_ViewMatrix; }

	private:
		glm::mat4 m_ProjectionMatrix{ 1.f };
		glm::mat4 m_ViewMatrix{ 1.f };
	};
}