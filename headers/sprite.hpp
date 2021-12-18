#ifndef SPRITE_HPP
#define SPRITE_HPP

#include <raylib.h>

struct Sprite
{
    Texture2D Texture{};
    float MaxFramesX{};
    float MaxFramesY{};
    float FrameX{};
    float FrameY{};
    int PreviousFrame{};
    float UpdateTime{1.f/8.f};
    float RunningTime{};

    Sprite();
    Sprite(const char* TexturePath, float MaxFramesX, float MaxFramesY);
    ~Sprite();
    void Tick(float DeltaTime);
};

#endif