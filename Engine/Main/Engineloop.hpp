#pragma once

#include "Rendering/RenderingInclude.hpp"
#include "Rendering/Renderer.hpp"
#include "Rendering/Window.hpp"
#include "Rendering/SimpleRenderSystem.hpp"
#include "Rendering/Camera.hpp"
#include "Audio/AudioEngine.hpp"
#include "Audio/MusicBuffer.hpp"
#include "ECS/ECSSubsystem.hpp"
#include "ECS/Examples/ECSExampleMovement.hpp"

namespace Pit {
	class Engineloop {
	public:
		Rendering::Window* Window;
		Rendering::Device* Device;
		ECS::ECSSubsystem* ECSSubsystem;
		Rendering::Renderer* Renderer;
		Rendering::SimpleRenderSystem* SimpleRenderSystem;
		Rendering::Camera* Camera;
		Audio::MusicBuffer* sound1;

		Engineloop();

		~Engineloop();

		int Run();

		void Update(); // Only for window callbacks (ex. Resizing)

		static Engineloop* Instance;

	private:
		void _LoadExampleEntities();
	};
}