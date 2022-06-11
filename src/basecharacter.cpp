#include "basecharacter.hpp"

void BaseCharacter::Tick(float DeltaTime) 
{
    // update animation frame
    RunningTime += DeltaTime;
    if (RunningTime >= UpdateTime)
    {
        PreviousFrame = CurrentFrame;
        ++CurrentFrame;
        RunningTime = 0.f;
        if (CurrentFrame > MaxFrames) CurrentFrame = 0;
    }
}