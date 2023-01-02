#include "pch.hpp"
#include "MemorySubmodule.hpp"
#include "BumpAllocator.hpp"
#include "Core/Engine.hpp"
#include "Platform\PlatformUtils.hpp"

static bool s_UseFrameallocator = false;

void* operator new(size_t size) {
	Pit::Memory::BumpAllocator* bumpAlloc = Pit::Memory::BumpAllocator::GetInstance();
	void* output = nullptr;
	if (s_UseFrameallocator && bumpAlloc && std::this_thread::get_id() == Pit::Thread::MainThreadId)
		output = bumpAlloc->Allocate(size);
	else
		output = malloc(size);
	PIT_ENGINE_ASSERT(Memory, output, "Failed to allocate {} bytes", size);
	return output;
}

void operator delete(void* p) {
	if (s_UseFrameallocator) {
		if (!Pit::Memory::BumpAllocator::GetInstance())
			free(p);
	}
}

namespace Pit {
	void MemorySubmodule::Init() {
		PIT_PROFILE_FUNCTION();

		m_FrameAllocator = new Memory::BumpAllocator(MB(10));
	}

	void MemorySubmodule::Shutdown() {
		PIT_PROFILE_FUNCTION();

		delete m_FrameAllocator;
	}

	void MemorySubmodule::Update() {
		PIT_PROFILE_FUNCTION();

		m_FrameAllocator->Reset();
	}

	void MemorySubmodule::ToggleFrameAllocator(bool on) { s_UseFrameallocator = on; }

	const size_t MemorySubmodule::GetFrameAllocatorSize() const {
		return m_FrameAllocator->GetSize();
	}

	const size_t MemorySubmodule::GetFrameAllocatorUsage() const {
		return m_FrameAllocator->GetUsage();
	}
}