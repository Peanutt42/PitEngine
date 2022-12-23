#include "pch.hpp"
#include "Core/Engine.hpp"
#include "RenderingSubmodule.hpp"
#include "Renderer.hpp"
#include <GLFW/glfw3.h>
#include <stb_image.h>
#pragma warning(push)
#pragma warning(disable: 4201)
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#pragma warning(pop)

namespace Pit::Rendering {

	Renderer::Renderer() {
		PIT_PROFILE_FUNCTION();

		Engine::Rendering()->Window->SetIcon(FileSystem::GetEngineDir() + "assets/Icons/PitEngineLogo.png");

		glEnable(GL_DEPTH_TEST);
		glPolygonMode(GL_FRONT, GL_FILL);
		
		m_RenderingSystem.Setup();
		
		// Create Screen Quad
		glGenVertexArrays(1, &m_ScreenQuadVAO);
		glGenBuffers(1, &m_ScreenQuadVBO);
		glBindVertexArray(m_ScreenQuadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, m_ScreenQuadVBO);

		constexpr float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
			// positions   // texCoords
			-1.0f,  1.0f,  0.0f, 1.0f,
			-1.0f, -1.0f,  0.0f, 0.0f,
			 1.0f, -1.0f,  1.0f, 0.0f,

			-1.0f,  1.0f,  0.0f, 1.0f,
			 1.0f, -1.0f,  1.0f, 0.0f,
			 1.0f,  1.0f,  1.0f, 1.0f
		};
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));


		if (Engine::GetSettings().AntiAliasing != 0) {
			// configure MSAA framebuffer
			// --------------------------
			glGenFramebuffers(1, &m_ScreenFramebuffer);
			glBindFramebuffer(GL_FRAMEBUFFER, m_ScreenFramebuffer);
			// create a multisampled color attachment texture
			unsigned int textureColorBufferMultiSampled;
			glGenTextures(1, &textureColorBufferMultiSampled);
			glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, textureColorBufferMultiSampled);
			glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, GLsizei(Engine::GetSettings().AntiAliasing), GL_RGB, Engine::Rendering()->Window->GetWidth(), Engine::Rendering()->Window->GetHeight(), GL_TRUE);
			glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, textureColorBufferMultiSampled, 0);
			// create a (also multisampled) renderbuffer object for depth and stencil attachments
			unsigned int rbo;
			glGenRenderbuffers(1, &rbo);
			glBindRenderbuffer(GL_RENDERBUFFER, rbo);
			glRenderbufferStorageMultisample(GL_RENDERBUFFER, GLsizei(Engine::GetSettings().AntiAliasing), GL_DEPTH24_STENCIL8, Engine::Rendering()->Window->GetWidth(), Engine::Rendering()->Window->GetHeight());
			glBindRenderbuffer(GL_RENDERBUFFER, 0);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			// configure second post-processing framebuffer
			glGenFramebuffers(1, &m_ScreenIntermediateFramebuffer);
			glBindFramebuffer(GL_FRAMEBUFFER, m_ScreenIntermediateFramebuffer);
			// create a color attachment texture
			glGenTextures(1, &m_ScreenTexColorBuffer);
			glBindTexture(GL_TEXTURE_2D, m_ScreenTexColorBuffer);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Engine::Rendering()->Window->GetWidth(), Engine::Rendering()->Window->GetHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ScreenTexColorBuffer, 0);	// we only need a color buffer

			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				std::cout << "ERROR::FRAMEBUFFER:: Intermediate framebuffer is not complete!" << std::endl;
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

		}
		else {
			// framebuffer configuration
			// -------------------------
			glGenFramebuffers(1, &m_ScreenFramebuffer);
			glBindFramebuffer(GL_FRAMEBUFFER, m_ScreenFramebuffer);
			// create a color attachment texture
			glGenTextures(1, &m_ScreenTexColorBuffer);
			glBindTexture(GL_TEXTURE_2D, m_ScreenTexColorBuffer);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Engine::Rendering()->Window->GetWidth(), Engine::Rendering()->Window->GetHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ScreenTexColorBuffer, 0);
			// create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
			unsigned int rbo;
			glGenRenderbuffers(1, &rbo);
			glBindRenderbuffer(GL_RENDERBUFFER, rbo);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, Engine::Rendering()->Window->GetWidth(), Engine::Rendering()->Window->GetHeight()); // use a single renderbuffer object for both a depth AND stencil buffer.
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it
			// now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				PIT_ENGINE_FATAL(Rendering, "Framebuffer is not complete!");
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		m_ScreenShader.Use();
		m_ScreenShader.SetInt("screenTexture", 0);
		

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

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (Engine::GetSettings().AntiAliasing != 0) {
			// 1. draw scene as normal in multisampled buffers
			glBindFramebuffer(GL_FRAMEBUFFER, m_ScreenFramebuffer);
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glEnable(GL_DEPTH_TEST);

			m_RenderingSystem.Render();

			// 2. now blit multisampled buffer(s) to normal colorbuffer of intermediate FBO. Image is stored in screenTexture
			glBindFramebuffer(GL_READ_FRAMEBUFFER, m_ScreenFramebuffer);
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_ScreenIntermediateFramebuffer);
			glBlitFramebuffer(0, 0, Engine::Rendering()->Window->GetWidth(), Engine::Rendering()->Window->GetHeight(), 0, 0, Engine::Rendering()->Window->GetWidth(), Engine::Rendering()->Window->GetHeight(), GL_COLOR_BUFFER_BIT, GL_NEAREST);

			// 3. now render quad with scene's visuals as its texture image
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);
			glDisable(GL_DEPTH_TEST);

			// draw Screen quad
			m_ScreenShader.Use();
			glBindVertexArray(m_ScreenQuadVAO);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_ScreenTexColorBuffer); // use the now resolved color attachment as the quad's texture
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}
		else {
			// bind to framebuffer and draw scene as we normally would to color texture 
			glBindFramebuffer(GL_FRAMEBUFFER, m_ScreenFramebuffer);

			glEnable(GL_DEPTH_TEST); // enable depth testing (is disabled for rendering screen-space quad)
			//glEnable(GL_CULL_FACE);
			//glCullFace(GL_FRONT);
			//glFrontFace(GL_CCW);

			// make sure we clear the framebuffer's content
			glClearColor(.1f, .1f, .1f, 1);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			m_RenderingSystem.Render();

			// now bind back to default framebuffer and draw a quad plane with the attached framebuffer color texture
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.
			glDisable(GL_CULL_FACE);
			// clear all relevant buffers
			glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
			glClear(GL_COLOR_BUFFER_BIT);

			m_ScreenShader.Use();
			glBindVertexArray(m_ScreenQuadVAO);
			glBindTexture(GL_TEXTURE_2D, m_ScreenTexColorBuffer);	// use the color attachment texture as the texture of the quad plane
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}
	}
	
	uint Renderer::GetScreenTexColorBuffer() { return m_ScreenTexColorBuffer; }
}