#include "sprite.hpp"

Sprite::Sprite(const Texture2D& Texture, const int MaxFramesX, const int MaxFramesY, const float UpdateSpeed) 
    : Texture{Texture}, MaxFramesX{MaxFramesX}, MaxFramesY{MaxFramesY}, UpdateTime{UpdateSpeed}
{

}

void Sprite::Tick(float DeltaTime)
{
    // update animation frame
    RunningTime += DeltaTime;
    if (RunningTime >= UpdateTime) {
        PreviousFrame = FrameX;
        ++FrameX;
        RunningTime = 0.f;
        if (FrameX > MaxFramesX) {
            FrameX = 0;
        }
    }
}

Rectangle Sprite::GetSourceRec()
{
    // source rectangle that shows what to draw
    Rectangle Source {
        static_cast<float>(FrameX * Texture.width / MaxFramesX),
        static_cast<float>(FrameY * Texture.height / MaxFramesY),
        static_cast<float>(Texture.width / MaxFramesX),
        static_cast<float>(Texture.height / MaxFramesY)
    };

    return Source;
}

Rectangle Sprite::GetPosRec(const Vector2& ScreenPos, const float Scale)
{
    // destination rectangle that shows where to draw
    Rectangle Destination {
        ScreenPos.x,
        ScreenPos.y,
        static_cast<float>(Texture.width/MaxFramesX) * Scale,
        static_cast<float>(Texture.height/MaxFramesY) * Scale
    };

    return Destination;
}