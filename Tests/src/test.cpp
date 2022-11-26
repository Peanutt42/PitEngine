#include <PitTest.hpp>
#include "../../Engine/src/Core/SmartPointers.hpp"

using namespace Pit;

struct TestStruct {
	int i;
};

PIT_TEST(SmartPointers_Ref,

Ref<TestStruct> myTest = new TestStruct();
myTest->i = 0;
{
	Ref<TestStruct> myCopiedTest = myTest;
	myTest->i = 5;
	PIT_TEST_EQUALS(Copied pointer should share data, myCopiedTest->i, myTest->i);
	PIT_TEST_EQUALS(Copied pointer should share their reference counters with the pointer they copy from, myTest.GetReferences(), myCopiedTest.GetReferences());
}
PIT_TEST_EQUALS(Pointers that shared their data one time ago must own their data even if sharers have been deleted, myTest->i, 5);
myTest->i = 10;
PIT_TEST_EQUALS(Regular changing some data inside the pointer, myTest->i, 10);
PIT_TEST_TRY_LAMDA(Pointers should reserve data if other pointers share references, myTest->i++;);
);