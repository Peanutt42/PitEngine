#pragma once

#include "Audio/AudioEngine.hpp"
#include "Audio/MusicBuffer.hpp"
#include "ECS/ECSSubsystem.hpp"
#include "ECS/Examples/ECSExampleMovement.hpp"
#include "Rendering/Renderer.hpp"
#include "Rendering/UI/UIRenderer.hpp"

namespace Pit {
	struct GlobalUbo {
		glm::mat4 projectionView{ 1.f };
		glm::vec3 lightDirection = glm::normalize(glm::vec3{ 1.f, -3.f, -1.f });
	};

	class Engine {
	public:
		ECS::ECSSubsystem* ECSSubsystem = nullptr;
		Audio::AudioEngine* AudioEngine = nullptr;
		uint sound1Id = 0; Audio::MusicBuffer* sound1 = nullptr;
		Rendering::Renderer* Renderer = nullptr;
		Rendering::UI::Renderer* UIRenderer = nullptr;

		Engine();
		~Engine();
		
		void Init(), Shutdown();
		void ForceShutdown() { m_ForceShutdown = true; }
		bool ShouldClose();
		void Update();

		static Engine* Instance;
		static Rendering::UI::LayerManager* LayerManager() { return Instance->UIRenderer->GetUILayerManager(); }

	private:
		bool m_ForceShutdown = false;
	};
}