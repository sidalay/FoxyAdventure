#include "headers/sprite.hpp"

Sprite::Sprite()
{

}

Sprite::Sprite(const char* TexturePath, float MaxFramesX, float MaxFramesY) 
    : Texture{LoadTexture(TexturePath)}, MaxFramesX{MaxFramesX}, MaxFramesY{MaxFramesY}
{

}

Sprite::~Sprite()
{
    // UnloadTexture(Texture);
}

// Update X position to draw next sprite
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