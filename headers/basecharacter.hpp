#ifndef BASECHARACTER_HPP
#define BASECHARACTER_HPP

#include <raylib.h>

enum class Direction
{
    UP, DOWN, LEFT, RIGHT
};

class BaseCharacter
{
protected:
    bool Alive{true};
    int CurrentFrame{};
    int PreviousFrame{};
    int MaxFrames{4};
    float UpdateTime{1.f/8.f};
    float RunningTime{};
    float Health {};
    float Speed {4};
    float Width {};
    float Height {};
    float Scale {1.f};

public:
    BaseCharacter();
    // void undoMovement();
    // Rectangle getCollisionRec();
    virtual void Tick(float DeltaTime);
    float getHealth() const {return Health;}
};

#endif