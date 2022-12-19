#include <PitTest.hpp>
#include "../../Engine/src/Core/SmartPointers.hpp"

using namespace Pit;

#define _CRTDBG_MAP_ALLOC
#define _CRTDBG_MAP_ALLOC_NEW
#include <stdlib.h>
#include <crtdbg.h>

struct TestStruct {
	int i;

	TestStruct(int i = 0) : i(i) {}
};

PIT_TEST(SetupDebugMemoryBreak,
//_CrtSetBreakAlloc(Object Number);
);

PIT_TEST(SmartPointers_Ref,

Ref<TestStruct> myTest = new TestStruct();
myTest->i = 0;
{
	Ref<TestStruct> myCopiedTest = myTest;
	myTest->i = 5;
	PIT_TEST_EQUALS("Copied pointer should share data", myCopiedTest->i, myTest->i);
	PIT_TEST_EQUALS("Copied pointer should share their reference counters with the pointer they copy from", myTest.GetReferences(), myCopiedTest.GetReferences());
}
PIT_TEST_EQUALS("Pointers that shared their data one time ago must own their data even if sharers have been deleted", myTest->i, 5);
myTest->i = 10;
PIT_TEST_EQUALS("Regular changing some data inside the pointer", myTest->i, 10);
PIT_TEST_TRY_LAMDA("Pointers should reserve data if other pointers share references", myTest->i++;);

);

PIT_TEST(Cpp_UniquePtr,

		 std::unique_ptr<TestStruct> myPtr = std::make_unique<TestStruct>();
myPtr->i = 5;

PIT_TEST_EQUALS("test", myPtr->i, 5);

);

PIT_TEST(Smartpointer_ScopeRef,

		 ScopeRef<TestStruct> myPtr = new TestStruct(5);
PIT_TEST_EQUALS("test", myPtr->i, 5);
PIT_TEST_TRUE("Existing pointers are true if data still exists", myPtr);
myPtr.Release();
PIT_TEST_TRUE("Deleted Pointers should release their data", !myPtr);
);


PIT_TEST(Cpp_SharedPtr,

		 std::shared_ptr<TestStruct> myPtr = std::make_shared<TestStruct>();
myPtr->i = 5;
{
	std::shared_ptr<TestStruct> myCopiedPtr = myPtr;
	myCopiedPtr->i = 10;
}
PIT_TEST_EQUALS("test", myPtr->i, 10);

);

PIT_TEST(Smartpointers_Refs,

Ref<TestStruct> myPtr = new TestStruct();
myPtr->i = 5;
{
	Ref<TestStruct> myCopiedPtr = myPtr;
	myCopiedPtr->i = 10;
}
PIT_TEST_EQUALS("test", myPtr->i, 10);

);


PIT_TEST(ReportMemoryLeaks,
		 _CrtDumpMemoryLeaks();
);