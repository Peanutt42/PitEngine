#pragma once

#include "Core/CoreInclude.hpp"
#include "Core/Engine.hpp"

namespace Pit {
	class AudioSubmodule;
	class AssetManagmentSubmodule;
	class ECSSubmodule;
	class NetworkingSubmodule;
	class PhysicsSubmodule;
	class RenderingSubmodule;

	class SubmoduleManager {
	public:
		void Init();

		void Shutdown();

		void Update();


		AudioSubmodule* AudioSubmodule;
		AssetManagmentSubmodule* AssetManagmentSubmodule;
		ECSSubmodule* ECSSubmodule;
		NetworkingSubmodule* NetworkingSubmodule;
		PhysicsSubmodule* PhysicsSubmodule;
		RenderingSubmodule* RenderingSubmodule;
	};
}