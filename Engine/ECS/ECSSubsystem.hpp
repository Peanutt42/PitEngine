#pragma once

#include "Main/CoreInclude.hpp"
#include "ECSInclude.hpp"
#include "ECSWorld.hpp"
#include "Examples/ECSBasicGameloop.hpp"


namespace Pit::ECS {
	class ECSSubsystem {
	public:
		ECSSubsystem();
		~ECSSubsystem();
		void Tick(float DeltaTime);
		void ResetECSWorld();

		World* GetEcsWorld() const { return ECSWorld; }
		static ECSSubsystem* GetInstance() { return m_Instance; }
	protected:

		World* ECSWorld = nullptr;
		WorldSpecs ECSWorldSpecs{
			"MainECSWorld"
		};
		Example::BasicGameloop* ECSGameloop = nullptr;

		static ECSSubsystem* m_Instance;

	private:
	};
}