#include <PitTest.hpp>

#include "../../Engine/src/Memory/BumpAllocator.hpp"

using namespace Pit;
using namespace Memory;

static BumpAllocator* s_alc = nullptr;

struct TestObj {
	int number, m1, m2;

	TestObj(int num = 0) : number(num) { std::cout << "Constructor\n"; }
	~TestObj() { std::cout << "Destructor\n"; }
};

void* operator new(size_t size) {
	if (s_alc)
		return s_alc->Allocate(size);
	else
		return malloc(size);
}

void operator delete(void* p) {
	if (s_alc) {}
	else free(p);
}

PIT_TEST(BumpAllocatorTest,
Sleep(1000);
BumpAllocator alc(KB(1));
s_alc = &alc;
void* ptr1 = alc.Allocate(512);
void* ptr2 = alc.Allocate(256);
TestObj* obj = new TestObj(1);
TestObj* objs[2];
objs[0] = new TestObj(1);
objs[1] = new TestObj(2);
PIT_TEST_TRUE("Allocated structs are placed correctly", objs[0]->number == 1 && objs[1]->number == 2);
delete obj;
delete objs[0];
delete objs[1];
PIT_TEST_TRUE("Allocating returns correct pointer", ptr1 != nullptr && ptr2 != nullptr && ptr1 != ptr2);
Sleep(1000);
s_alc = nullptr;
		 );

PIT_TEST(RawMalloc,
Sleep(1000);
//void* ptr = malloc(MB(1));
TestObj* objs = new TestObj[2];
objs[0].number = 1;
objs[1].number = 2;
PIT_TEST_TRUE("Allocated structs are placed correctly", objs[0].number == 1 && objs[1].number == 2);
delete[] objs;
//free(ptr);
//ptr = nullptr;
		 );