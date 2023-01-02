#pragma once

#include "Core/CoreInclude.hpp"
#include "Core/Submodule.hpp"

namespace Pit {
	namespace Memory {
		class BumpAllocator;
	}

	/// <summary>
	/// Submodule for handeling Networking
	/// </summary>
	class MemorySubmodule : public Submodule {
	public:
		virtual void Init();

		virtual void Shutdown();

		virtual void Update();

		void ToggleFrameAllocator(bool on);

		const size_t GetFrameAllocatorUsage() const;
		const size_t GetFrameAllocatorSize() const;

	private:
		Memory::BumpAllocator* m_FrameAllocator;
	};
}