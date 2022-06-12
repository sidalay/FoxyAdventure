#ifndef BASECHARACTER_HPP
#define BASECHARACTER_HPP

#include <raylib.h>

enum class Direction
{
    UP, DOWN, LEFT, RIGHT
};

class BaseCharacter
{
public:
    BaseCharacter() = default;
    
protected:
    bool Alive{true};
    int CurrentFrame{};
    int PreviousFrame{};
    int MaxFrames{4};
    float UpdateTime{1.f/8.f};
    float Width{};
    float Height{};
};

#endif // BASECHARACTER_HPP