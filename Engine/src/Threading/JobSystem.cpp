#include "pch.hpp"
#include "JobSystem.hpp"

namespace Pit {

	uint32 JobSystem::s_NumThreads = 0;
	ThreadSafeRingBuffer<std::function<void()>, 256> JobSystem::s_JobPool;
	std::condition_variable JobSystem::s_WakeCondition;
	std::mutex JobSystem::s_WakeMutex;
	uint64 JobSystem::s_CurrentLabel;
	std::atomic<uint64> JobSystem::s_FinishedLabel;
	std::atomic<bool> JobSystem::s_ShutdownThreads;

	void JobSystem::_WorkerThread(uint32 threadID) {
		OPTICK_THREAD((String("WorkerThread_") + std::to_string(threadID + 1)).c_str());

		std::function<void()> job;

		while (!s_ShutdownThreads.load()) {
			if (s_JobPool.pop_front(job)) {
				PIT_PROFILE_FUNCTION("WorkerThread::Update");
				job();
				s_FinishedLabel.fetch_add(1);
			}
			else {
				std::unique_lock<std::mutex> lock(s_WakeMutex);
				s_WakeCondition.wait(lock);
			}
		}
	}

	void JobSystem::Initialize() {
		PIT_PROFILE_FUNCTION();

		s_FinishedLabel.store(0);

		auto numCores = std::thread::hardware_concurrency() - 6;

		if (numCores < 1) numCores = 1;
		s_NumThreads = numCores;

		PIT_ENGINE_INFO(General, "JobSystem started with {} threads.", s_NumThreads);

		for (uint32 i = 0; i < s_NumThreads; i++) {
			std::thread thread(&_WorkerThread, i);
			thread.detach();
		}
	}

	void JobSystem::Shutdown() {
		PIT_PROFILE_FUNCTION();

		Wait();
		s_ShutdownThreads.store(true);
	}

	void JobSystem::Execute(const std::function<void()>& job) {
		s_CurrentLabel += 1;

		while (!s_JobPool.push_back(job)) { _Poll(); }

		s_WakeCondition.notify_one();
	}

	void JobSystem::Dispatch(uint32 jobCount, uint32 groupSize, const std::function<void(uint32, uint32)>& job) {
		if (jobCount == 0 || groupSize == 0) return;

		const uint32 groupCount = (jobCount + groupSize - 1) / groupSize;

		s_CurrentLabel += groupCount;

		for (uint32 groupIndex = 0; groupIndex < groupCount; groupIndex++) {
			const auto& jobGroup = [jobCount, groupSize, job, groupIndex]() {
				const uint32 groupJobOffset = groupIndex * groupSize;
				const uint32 groupJobEnd = Math::Min(groupJobOffset + groupSize, jobCount);

				for (uint32 i = groupJobOffset; i < groupJobEnd; i++)
					job(groupIndex, i);
			};

			while (!s_JobPool.push_back(jobGroup)) { _Poll(); }

			s_WakeCondition.notify_one();
		}
	}

	bool JobSystem::IsBusy() {
		return s_FinishedLabel.load() < s_CurrentLabel;
	}

	void JobSystem::Wait() {
		while (IsBusy()) { _Poll(); }
	}

	void JobSystem::_Poll() {
		s_WakeCondition.notify_one();
		std::this_thread::yield();
	}
}