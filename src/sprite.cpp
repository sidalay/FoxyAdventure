#include "headers/sprite.hpp"

Sprite::Sprite(const Texture2D& Texture, float MaxFramesX, float MaxFramesY, float UpdateSpeed) 
    : Texture{Texture}, MaxFramesX{MaxFramesX}, MaxFramesY{MaxFramesY}, UpdateTime{UpdateSpeed}
{

}

void Sprite::Tick(float DeltaTime)
{
    // update animation frame
    RunningTime += DeltaTime;
    if (RunningTime >= UpdateTime)
    {
        PreviousFrame = FrameX;
        ++FrameX;
        RunningTime = 0.f;
        if (FrameX > MaxFramesX) FrameX = 0;
    }
}

Rectangle Sprite::GetSourceRec()
{
    // source rectangle that shows what to draw
    Rectangle Source {
        FrameX * Texture.width / MaxFramesX,
        FrameY * Texture.height / MaxFramesY,
        Texture.width / MaxFramesX,
        Texture.height / MaxFramesY
    };

    return Source;
}

Rectangle Sprite::GetPosRec(const Vector2& ScreenPos, float Scale)
{
    // destination rectangle that shows where to draw
    Rectangle Destination {
        ScreenPos.x,
        ScreenPos.y,
        (Texture.width/MaxFramesX) * Scale,
        (Texture.height/MaxFramesY) * Scale
    };

    return Destination;
}