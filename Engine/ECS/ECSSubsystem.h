#pragma once

#include "Main/CoreInclude.h"
#include "ECS.h"
#include "ECSWorld.h"
#include "Examples/ECSBasicGameloop.h"


namespace Pit::ECS {
	class ECSSubsystem {

	public:
		void Initialize();
		void Deinitialize();
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