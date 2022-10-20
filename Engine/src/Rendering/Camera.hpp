#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace Pit::Rendering {
	class Camera {
	public:
		enum Projection {
			Orthographic,
			Perspective
		};

		void SetProjection();

		void SetViewDirection(const glm::vec3& position, const glm::vec3& direction, const glm::vec3& up = { 0.f, -1.f, 0.f });
		void SetViewTarget(const glm::vec3& position, const glm::vec3& target, const glm::vec3& up = { 0.f, -1.f, 0.f });
		void SetViewYXZ(const glm::vec3& position, const glm::vec3& rotation);

		const glm::mat4& GetProjection() const { return m_ProjectionMatrix; }
		const glm::mat4& GetView() const { return m_ViewMatrix; }

		Projection ProjectionMode;
		float Left, Right, Top, Bottom;
		float FOV, AspectRatio;

		float NearClip;
		float FarClip;

	private:
		glm::mat4 m_ProjectionMatrix{ 1.f };
		glm::mat4 m_ViewMatrix{ 1.f };
	};
}