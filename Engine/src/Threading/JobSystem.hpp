#pragma once

#include "Core/CoreInclude.hpp"

namespace Pit {
	template <typename T, size capacity>
	class ThreadSafeRingBuffer {
	public:
		inline bool push_back(const T& item) {
			bool result = false;
			lock.lock();
			size next = (head + 1) % capacity;
			if (next != tail) {
				data[head] = item;
				head = next;
				result = true;
			}
			lock.unlock();
			return result;
		}

		inline bool pop_front(T& item) {
			bool result = false;
			lock.lock();
			if (tail != head) {
				item = data[tail];
				tail = (tail + 1) % capacity;
				result = true;
			}
			lock.unlock();
			return result;
		}

	private:
		T data[capacity];
		size head = 0;
		size tail = 0;
		std::mutex lock;
	};

	class JobSystem {
	public:
		static void Initialize();
		static void Shutdown();

		static void Execute(const std::function<void()>& job);
		static void Dispatch(uint32 jobCount, uint32 groupSize, const std::function<void(uint32, uint32)>& job);
		static bool IsBusy();
		static void Wait();

	private:
		static uint32 s_NumThreads;
		static ThreadSafeRingBuffer<std::function<void()>, 256> s_JobPool;
		static std::condition_variable s_WakeCondition;
		static std::mutex s_WakeMutex;
		static uint64 s_CurrentLabel;
		static std::atomic<uint64> s_FinishedLabel;
		static std::atomic<bool> s_ShutdownThreads;

		static void _WorkerThread(uint32 threadID);
		static void _Poll();
	};
}