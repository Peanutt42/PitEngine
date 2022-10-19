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
		const glm::mat4& GetProjection() const { return m_ProjectionMatrix; }

		Projection ProjectionMode;
		float Left, Right, Top, Bottom;
		float FOV, AspectRatio;

		float NearClip;
		float FarClip;

	private:
		glm::mat4 m_ProjectionMatrix{ 1.f };
	};
}