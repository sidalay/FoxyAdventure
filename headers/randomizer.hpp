#ifndef RANDOMIZER_HPP
#define RANDOMIZER_HPP

#include <random>

class Randomizer
{
public:
    Randomizer() = default;
    
    int Randomize(std::uniform_int_distribution<int>& Range);
private:
    std::random_device Seed;
    std::mt19937 RandomEngine{std::mt19937{Seed()}};
};

#endif // RANDOMIZER_HPP