#pragma once

//#include "Core/CoreInclude.hpp"

namespace Pit::Memory {
#define KB(number) number * 1024
#define MB(number) number * 1024 * 1024
#define GB(number) number * 1024 * 1024 * 1024
	class BumpAllocator {
	public:
		BumpAllocator(size_t size) : m_Size(size) {
			m_Start = (std::byte*)malloc(m_Size);
			if (m_Start == nullptr) std::cerr << "Failed to allocate " << m_Size << " bytes!\n";//PIT_ENGINE_ERR(Memory, "Failed to allocate {} bytes!", m_Size);
			m_Next = m_Start;
		}

		~BumpAllocator() {
			if (m_Start) {
				free((void*)m_Start);
				m_Start = nullptr;
			}
		}

		// Resets the current m_Next ptr to m_Start, so no memory is actually beeing flushed/erased
		void Flush() {
			m_Next = m_Start;
		}

		void* Allocate(size_t size) {
			if (m_Start == nullptr) {
				std::cerr << "Trying to allocate to a BumpAllocator that isn't setup right (m_Start == nullptr)!\n";//PIT_ENGINE_ERR(Memory, "Trying to allocate to a BumpAllocator that isn't setup right(m_Start == nullptr)!");
				return nullptr;
			}
			if ((size_t)(m_Size - (m_Next - m_Start)) < size) {
				std::cerr << "Trying to allocate " << size << " bytes to a BumpAllocator that only has " << m_Size - (m_Next - m_Start) << " bytes left!\n";//PIT_ENGINE_ERR(Memory, "Trying to allocate{0} bytes to a BumpAllocator that only has {1} bytes left!", size, m_FreePtr - m_Start);
				return nullptr;
			}
			void* output = m_Next;
			m_Next = m_Next + size;
			std::cout << "Succesfully allocated " << size << " bytes!\n";
			return output;
		}

	private:
		std::byte* m_Start = nullptr;
		std::byte* m_Next = nullptr;
		size_t m_Size;
	};
}