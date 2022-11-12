#include "pch.hpp"
#include "Core/Engine.hpp"
#include "RenderingSubmodule.hpp"
#include "Renderer.hpp"
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace Pit;
using namespace Rendering;

float vertices[] = {
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};

glm::vec3 cubePositions[] = {
	glm::vec3(0.0f,  0.0f,  0.0f),
	glm::vec3(2.0f,  5.0f, -15.0f),
	glm::vec3(-1.5f, -2.2f, -2.5f),
	glm::vec3(-3.8f, -2.0f, -12.3f),
	glm::vec3(2.4f, -0.4f, -3.5f),
	glm::vec3(-1.7f,  3.0f, -7.5f),
	glm::vec3(1.3f, -2.0f, -2.5f),
	glm::vec3(1.5f,  2.0f, -2.5f),
	glm::vec3(1.5f,  0.2f, -1.5f),
	glm::vec3(-1.3f,  1.0f, -1.5f)
};

static void APIENTRY GlMessageCallback(GLenum source, GLenum type, unsigned int id, GLenum severity,
										   GLsizei length, const char* message, const void* userParam) {

	// Ignore non-significant error/warning codes
	if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

	switch (severity) {
	case GL_DEBUG_SEVERITY_HIGH:         PIT_ENGINE_FATAL(Rendering, "[Glad]: {}", message); return;
	case GL_DEBUG_SEVERITY_MEDIUM:       PIT_ENGINE_ERR(Rendering, "[Glad]: {}", message); return;
	case GL_DEBUG_SEVERITY_LOW:          PIT_ENGINE_WARN(Rendering, "[Glad]: {}", message); return;
	case GL_DEBUG_SEVERITY_NOTIFICATION: PIT_ENGINE_TRACE(Rendering, "[Glad]: {}", message); return;
	}

	PIT_ENGINE_TRACE(Rendering, "[OpenGL]: Unknown severity level!");
}
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

Renderer::Renderer() {
	PIT_PROFILE_FUNCTION();

	Engine::Rendering()->Window->SetIcon(FileSystem::GetEngineDir() + "assets/Icons/PitEngineLogo.png");

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);

	int flags; glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
	if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
		//glEnable(GL_DEBUG_OUTPUT);
		//glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		//glDebugMessageCallback(GlMessageCallback, nullptr);
		//glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
	}

	glfwSetCursorPosCallback(Engine::Rendering()->Window->GetWindowHandle(), mouse_callback);
	glfwSetScrollCallback(Engine::Rendering()->Window->GetWindowHandle(), scroll_callback);

	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_VBO);
	//glGenBuffers(1, &m_EBO);

	glBindVertexArray(m_VAO);
	
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glPolygonMode(GL_FRONT, GL_FILL);
	
	m_Shader.Use();

	m_Shader.SetInt("texture1", 0);
	m_Shader.SetInt("texture2", 1);

	PIT_ENGINE_INFO(Rendering, "OpenGL Info:");
	PIT_ENGINE_INFO(Rendering, " - Vendor: {}", (const char*)glGetString(GL_VENDOR));
	PIT_ENGINE_INFO(Rendering, " - Renderer: {}", (const char*)glGetString(GL_RENDERER));
	PIT_ENGINE_INFO(Rendering, " - Version: {}", (const char*)glGetString(GL_VERSION));
}

Renderer::~Renderer() {
	PIT_PROFILE_FUNCTION();

}


void Renderer::Update() {
	PIT_PROFILE_FUNCTION();

	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (Input::IsKeyDown(Key_W))
		m_Camera.ProcessKeyboard({0, 1, 0});
	if (Input::IsKeyDown(Key_S))
		m_Camera.ProcessKeyboard({ 0, -1, 0 });
	if (Input::IsKeyDown(Key_A))
		m_Camera.ProcessKeyboard({ 1, 0, 0 });
	if (Input::IsKeyDown(Key_D))
		m_Camera.ProcessKeyboard({ -1, 0, 0 });

	m_Texture.Bind(0);
	m_Texture.Bind(1);

	m_Shader.Use();
	
	glm::mat4 projection = glm::perspective(glm::radians(m_Camera.Zoom), (float)Engine::Rendering()->Window->GetWidth() / (float)Engine::Rendering()->Window->GetHeight(), 0.1f, 100.0f);
	glm::mat4 view = m_Camera.GetViewMatrix();

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));


	unsigned int modelLoc = glGetUniformLocation(m_Shader.GetID(), "model");
	unsigned int viewLoc = glGetUniformLocation(m_Shader.GetID(), "view");

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);

	m_Shader.SetMat4("projection", projection);

	glBindVertexArray(m_VAO);
	for (unsigned int i = 0; i < 10; i++) {
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, cubePositions[i]);
		float angle = 20.0f * i;
		model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
		m_Shader.SetMat4("model", model);

		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
	//glDrawArrays(GL_TRIANGLES, 0, 36);//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

static void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
	static float lastX = Engine::Rendering()->Window->GetWidth() / 2.0f;
	static float lastY = Engine::Rendering()->Window->GetHeight() / 2.0f;
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	Engine::Rendering()->Renderer->m_Camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	Engine::Rendering()->Renderer->m_Camera.ProcessMouseScroll(static_cast<float>(yoffset));
}