#pragma once

namespace Pit::Memory {
#define KB(number) number * 1024
#define MB(number) KB(number) * 1024
#define GB(number) MB(number) * 1024

	class BumpAllocator {
	public:
		BumpAllocator(size_t size);
		~BumpAllocator();

		// Resets the current m_Next ptr to m_Start, so no memory is actually beeing flushed/erased
		void Reset();

		void Resize(size_t size);

		void* Allocate(size_t size);

		const size_t GetSize() const;
		const size_t GetUsage() const;

		static BumpAllocator* GetInstance() { return s_Instance; }

	private:
		bool _Check();

		std::byte* m_Start = nullptr;
		std::byte* m_Next = nullptr;
		size_t m_Size = 0, m_UsedSize = 0;

		static BumpAllocator* s_Instance;
	};
}