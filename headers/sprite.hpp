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

    Sprite() = default;
    Sprite(const Texture2D& Texture, float MaxFramesX = 1, float MaxFramesY = 1, float UpdateSpeed = 1.f/8.f);
    void Tick(float DeltaTime);
    Rectangle GetSourceRec();
    Rectangle GetPosRec(const Vector2& ScreenPos, float Scale);
};

#endif // SPRITE_HPP