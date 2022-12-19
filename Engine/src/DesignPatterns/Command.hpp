#pragma once

#include "Core/CoreInclude.hpp"

namespace Pit::DesignPatterns {
	class Command {
	public:
		virtual ~Command() {}
		
		virtual void Execute() = 0;
		virtual void Undo() = 0;
	};
}