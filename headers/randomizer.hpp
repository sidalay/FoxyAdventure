#ifndef RANDOMIZER_HPP
#define RANDOMIZER_HPP

#include <random>

class Randomizer
{
public:
    constexpr Randomizer() = default;
    
    constexpr int Randomize(const std::uniform_int_distribution<int>& Range) {return Range(RandomEngine);}
private:
    std::mt19937 RandomEngine{std::mt19937{std::random_device}};
};

#endif // RANDOMIZER_HPP