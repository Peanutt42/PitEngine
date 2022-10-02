#pragma once

#include "Rendering/RenderingInclude.hpp"
#include "Rendering/Renderer.hpp"
#include "Rendering/Window.hpp"
#include "ECS/ECSSubsystem.hpp"
#include "ECS/Examples/ECSExampleMovement.hpp"

namespace Pit {

	using namespace Rendering;
	using namespace Debug;
	using namespace ECS;

	class Engineloop {
	private:
		Window m_Window{ 800, 600, "PitEngine" };
		Device m_Device{ m_Window };
		ECSSubsystem m_ECSSubsystem;
		Renderer m_Renderer{m_Device, m_Window, m_ECSSubsystem.GetEcsWorld() };

	public:
		Engineloop();

		~Engineloop();

		int Run();

	private:
		void _LoadExampleEntities();
	};
}