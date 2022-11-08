#include <PitTest.hpp>
#include <thread>
#include "C:\dev\cpp\PitEngine\vendor\optick\include\optick.h"

static void Thread() {
	OPTICK_THREAD("TestThread");

	while (true) {
		OPTICK_EVENT("TestThread::Update");
		std::this_thread::sleep_for(std::chrono::milliseconds(10));

	}

}

PIT_TEST(SomeTestGroup,
	while (true) {
		OPTICK_FRAME("Test");
		std::this_thread::sleep_for(std::chrono::seconds(1));
		for (int i = 0; i < 10; i++) {
			std::thread tread(Thread);
			tread.detach();
		}
	}
);