#include "pch.hpp"
#include "RenderingSystem.hpp"
#include "Core/Engine.hpp"
#include "RenderingSubmodule.hpp"
#include "Camera.hpp"
#include "ECS/ECSSubmodule.hpp"
#include "ECS/ECSWorld.hpp"
#include "ECS/ECSComponents.hpp"
#include <imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>

namespace Pit::Rendering {
	float vertices[] = {
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
	};
	void RenderingSystem::Setup() {
		// first, configure the cube's VAO (and VBO)
		
		glGenVertexArrays(1, &cubeVAO);
		glGenBuffers(1, &VBO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glBindVertexArray(cubeVAO);

		// position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		// normal attribute
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);


		// second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
		
		glGenVertexArrays(1, &lightCubeVAO);
		glBindVertexArray(lightCubeVAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		// note that we update the lamp's position attribute's stride to reflect the updated buffer data
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		Engine::UIRenderEvent += [this]() {
			ImGui::Begin("Test");
			ImGui::ColorPicker3("LightColor", glm::value_ptr(lightColor));
			ImGui::DragFloat3("LightPos", glm::value_ptr(lightPos));
			ImGui::End();
		};
	}

	void RenderingSystem::Render() {
		// be sure to activate shader when setting uniforms/drawing objects
		lightingShader.Use();
		lightingShader.SetVec3("objectColor", 1.0f, 0.5f, 0.31f);
		lightingShader.SetVec3("lightColor", lightColor);
		lightingShader.SetVec3("lightPos", lightPos);
		lightingShader.SetVec3("viewPos", Engine::Rendering()->Camera->Position);

		// view/projection transformations
		Camera* cam = Engine::Rendering()->Camera;
		glm::mat4 projection = glm::perspective(glm::radians(cam->Fov), (float)Engine::Rendering()->Window->GetWidth() /
												(float)Engine::Rendering()->Window->GetHeight(), cam->NearPlane, cam->FarPlane);
		glm::mat4 view = cam->GetViewMatrix();
		lightingShader.SetMat4("projection", projection);
		lightingShader.SetMat4("view", view);

		// world transformation
		glm::mat4 model = glm::mat4(1.0f);

		// render the cube
		glBindVertexArray(cubeVAO);
		auto entityview = Engine::ECS()->GetEcsWorld().View<ECS::TransformComponent>();
		for (auto e : entityview) {
			ECS::TransformComponent& transform = entityview.get<ECS::TransformComponent>(e);
			model = transform.mat4();
			lightingShader.SetMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}


		// also draw the lamp object
		lightCubeShader.Use();
		lightCubeShader.SetMat4("projection", projection);
		lightCubeShader.SetMat4("view", view);
		model = glm::mat4(1.0f);
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
		lightCubeShader.SetMat4("model", model);

		glBindVertexArray(lightCubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
}