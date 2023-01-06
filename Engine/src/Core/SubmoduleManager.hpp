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
	class AntiCheatSubmodule;
	class ScriptingSubmodule;

	class SubmoduleManager {
	public:
		void Init();

		void Shutdown();

		void Update();


		AudioSubmodule* AudioSubmodule;
		AntiCheatSubmodule* AntiCheatSubmodule;
		AssetManagmentSubmodule* AssetManagmentSubmodule;
		ECSSubmodule* ECSSubmodule;
		NetworkingSubmodule* NetworkingSubmodule;
		PhysicsSubmodule* PhysicsSubmodule;
		RenderingSubmodule* RenderingSubmodule;
		ScriptingSubmodule* ScriptingSubmodule;
	};
}