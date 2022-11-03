#include <PitTest.hpp>


PIT_TEST(SomeTestGroup,
	PIT_TEST_TRY_LAMDA(SomeShortTest, Sleep(400););
	PIT_TEST_TRY_LAMDA(SomeLongTest, Sleep(2000););
	PIT_TEST_TRY_LAMDA(SomeSmallTest, Sleep(10););
	PIT_TEST_TRUE(SomeEasyTest, 1 + 1 == 2);
);

PIT_TEST(SomeTestGroup2,
	PIT_TEST_TRY_LAMDA(SomeShortTest, Sleep(400););
	PIT_TEST_TRY_LAMDA(SomeLongTest, Sleep(2000););
	PIT_TEST_TRY_LAMDA(SomeSmallTest, Sleep(10););
	PIT_TEST_TRUE(SomeEasyTest, 1 + 1 == 2);
);