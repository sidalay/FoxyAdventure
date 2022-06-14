#include "randomizer.hpp"

int Randomizer::Randomize(std::uniform_int_distribution<int>& Range)
{
    return Range(RandomEngine);
}