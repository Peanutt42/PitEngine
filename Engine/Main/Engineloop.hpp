#pragma once

#include "Rendering/RenderingInclude.hpp"
#include "Rendering/Renderer.hpp"
#include "Rendering/Window.hpp"
#include "ECS/ECSSubsystem.hpp"

namespace Pit {

	using namespace Rendering;
	using namespace Debug;
	using namespace ECS;

	class Engineloop {
		Renderer* m_Renderer;
		ECSSubsystem* m_ECSSubsystem;

	public:
		Engineloop() {
			FileSystem::Init();

			Logging::Init();

			m_ECSSubsystem = new ECSSubsystem();
			m_ECSSubsystem->Initialize();

			m_Renderer = new Renderer();

			m_Renderer->SetSpecs({ true });
		}

		~Engineloop() {
			delete m_Renderer;

			m_ECSSubsystem->Deinitialize();
			delete m_ECSSubsystem;

			Logging::Shutdown();
		}
		


		int Run() {
			while (!m_Renderer->ShouldClose()) {
				//QUICK_SCOPE_PROFILE("Main thread update");

				m_ECSSubsystem->Tick(0.16f);

				m_Renderer->Render();
			}

			return 0;
		}
	};
}