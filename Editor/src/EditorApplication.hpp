#pragma once

#include "Engine/Main/Engine.hpp"

namespace Pit::Editor {
	class EditorApplication {
	public:
		EditorApplication(Engine& engine);
		~EditorApplication();

		void Init(), Shutdown();

		void Update();

	private:
		Engine& m_Engine;
	};
}