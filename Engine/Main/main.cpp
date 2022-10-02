#include "Engineloop.hpp"
#include <iostream>

using namespace Pit;

int main() {
	Logging::Init();
	
	Engineloop engineloop;

	engineloop.Run();

	Logging::Shutdown();

	return 0;
}