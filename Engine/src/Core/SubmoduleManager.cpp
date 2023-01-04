#include "pch.hpp"
#include "SubmoduleManager.hpp"
#include "Engine.hpp"

#include "Audio/AudioSubmodule.hpp"
#include "ECS/ECSSubmodule.hpp"
#include "Rendering/RenderingSubmodule.hpp"
#include "Networking/NetworkingSubmodule.hpp"
#include "Physics/PhysicsSubmodule.hpp"
#include "AssetManagment/AssetManagmentSubmodule.hpp"
#include "Memory/MemorySubmodule.hpp"
#include "AntiCheat/AntiCheatSubmodule.hpp"
#include "Scripting/ScriptingSubmodule.hpp"

namespace Pit {
	void SubmoduleManager::Init() {
		MemorySubmodule = new Pit::MemorySubmodule();
		MemorySubmodule->Init();

		AntiCheatSubmodule = new Pit::AntiCheatSubmodule();
		AntiCheatSubmodule->Init();


		if (!Engine::GetSettings().Headless) {
			AudioSubmodule = new Pit::AudioSubmodule();
			AudioSubmodule->Init();
		}

		ECSSubmodule = new Pit::ECSSubmodule();
		ECSSubmodule->Init();

		ScriptingSubmodule = new Pit::ScriptingSubmodule();
		ScriptingSubmodule->Init();

		if (!Engine::GetSettings().Headless) {
			RenderingSubmodule = new Pit::RenderingSubmodule();
			RenderingSubmodule->Init();
		}

		NetworkingSubmodule = new Pit::NetworkingSubmodule();
		NetworkingSubmodule->Init();

		PhysicsSubmodule = new Pit::PhysicsSubmodule();
		PhysicsSubmodule->Init();

		AssetManagmentSubmodule = new Pit::AssetManagmentSubmodule();
		AssetManagmentSubmodule->Init();
	}

	void SubmoduleManager::Shutdown() {
		AssetManagmentSubmodule->Shutdown();
		delete AssetManagmentSubmodule;

		if (!Engine::GetSettings().Headless) {
			RenderingSubmodule->Shutdown();
			delete RenderingSubmodule;
		}
		PhysicsSubmodule->Shutdown();
		delete PhysicsSubmodule;
		NetworkingSubmodule->Shutdown();
		delete NetworkingSubmodule;
		ScriptingSubmodule->Shutdown();
		delete ScriptingSubmodule;
		ECSSubmodule->Shutdown();
		delete ECSSubmodule;
		if (!Engine::GetSettings().Headless) {
			AudioSubmodule->Shutdown();
			delete AudioSubmodule;
		}
		AntiCheatSubmodule->Shutdown();
		delete AntiCheatSubmodule;

		MemorySubmodule->Shutdown();
		delete MemorySubmodule;
	}

	void SubmoduleManager::Update() {
		MemorySubmodule->Update();

		AntiCheatSubmodule->Update();

		NetworkingSubmodule->Update();

		PhysicsSubmodule->Update();
		
		ScriptingSubmodule->Update();

		ECSSubmodule->Update();

		if (!Engine::GetSettings().Headless) {
			RenderingSubmodule->Update();

			AudioSubmodule->Update();
		}
	}
}