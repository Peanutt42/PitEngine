#pragma once

#include "Core/CoreInclude.hpp"
#include "Core/Submodule.hpp"

namespace Pit {
	/// <summary>
	/// Submodule for handling Networking
	/// </summary>
	class AntiCheatSubmodule : public Submodule {
	public:
		virtual void Init();

		virtual void Shutdown();

		virtual void Update();

	private:
		static void _AntiCheatThreadMain();

		std::thread m_AntiCheatThread;
	};
}