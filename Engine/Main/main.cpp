#include "Engineloop.hpp"

using namespace Pit;

int main() {
	FileSystem::Init();
	Debug::Logging::Init();

	Engineloop engineloop;
	int result = engineloop.Run();
	return result;
}