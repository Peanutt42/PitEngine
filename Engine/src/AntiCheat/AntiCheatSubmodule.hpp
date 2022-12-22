#pragma once

#include "Core/CoreInclude.hpp"
#include "Core/Submodule.hpp"

namespace Pit {
	/// <summary>
	/// Submodule for handeling Networking
	/// </summary>
	class AntiCheatSubmodule : public Submodule {
	public:
		virtual void Init();

		virtual void Shutdown();

		virtual void Update();

	private:
		static void _AntiCheatThreadMain(AntiCheatSubmodule* antiCheat);

		std::thread m_AntiCheatThread;
		bool m_AntiCheatThreadQuit;
	};
}