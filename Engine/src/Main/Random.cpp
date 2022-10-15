#include "Main/CoreInclude.hpp"
#include "Random.hpp"

using namespace Pit;

std::mt19937 Random::s_RandomEngine{ Cast<uint>(std::random_device()()) };
std::uniform_int_distribution<std::mt19937::result_type> Random::s_Distribution;