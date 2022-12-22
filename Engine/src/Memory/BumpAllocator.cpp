#include "pch.hpp"
#include "Core/CoreInclude.hpp"
#include "BumpAllocator.hpp"

namespace Pit::Memory {
	BumpAllocator* BumpAllocator::s_Instance = nullptr;


	BumpAllocator::BumpAllocator(size_t size) : m_Size(size) {
		s_Instance = this;
		m_Start = (std::byte*)malloc(m_Size);
		if (m_Start == nullptr) PIT_ENGINE_ERR(Memory, "Failed to initialize BumpAllocator with {} bytes!", m_Size);
		m_Next = m_Start;
	}

	BumpAllocator::~BumpAllocator() {
		if (s_Instance == this) s_Instance = nullptr;
		if (m_Start) {
			free((void*)m_Start);
			m_Start = nullptr;
		}
	}

	void BumpAllocator::Reset() {
		if (_Check()) {
			m_Next = m_Start;
			m_UsedSize = 0;
		}
	}
	
	void BumpAllocator::Resize(size_t size) {
		if (!_Check()) return;

		std::byte* newStart = (std::byte*)realloc(m_Start, size);
		if (!m_Start) {
			PIT_ENGINE_ERR(Memory, "BumpAllocator failed to resize -> failed realloc()!");
			return;
		}
		m_Start = newStart;
		m_Size = size;
	}
	
	void* BumpAllocator::Allocate(size_t size) {
		if (!_Check()) return nullptr;

		if ((size_t)(m_Size - (m_Next - m_Start)) < size) {
			PIT_ENGINE_ERR(Memory, "Trying to allocate {0} bytes to a BumpAllocator that only has {1} bytes left!", size, m_Size - (m_Next - m_Start));
			PIT_ENGINE_WARN(Memory, "Now resizing BumpAllocator to 1.5 times its size");
			Resize((size_t)(m_Size * 1.5f));
			return nullptr;
		}
		void* output = m_Next;
		m_Next = m_Next + size;
		m_UsedSize += size;
		return output;
	}

	const size_t BumpAllocator::GetSize() const {
		return m_Size;
	}

	const size_t BumpAllocator::GetUsage() const {
		return m_UsedSize;
	}

	BumpAllocator* BumpAllocator::GetInstance() { return s_Instance; }

	bool BumpAllocator::_Check() {
		if (!m_Start) {
			PIT_ENGINE_ERR(Memory, "Trying to allocate to a BumpAllocator that isn't setup right (m_Start == nullptr)!");
			return false;
		}
		return true;
	}
}