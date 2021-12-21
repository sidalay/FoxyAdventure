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
    Sprite(const char* TexturePath, float MaxFramesX = 1, float MaxFramesY = 1);
    ~Sprite();
    void Tick(float DeltaTime);
    // Returns a source rectangle that dictates what sprite to draw
    Rectangle GetSourceRec();
    // Returns destination rectangle that shows where to draw on screen
    Rectangle GetPosRec(const Vector2& ScreenPos, float Scale);
};

#endif