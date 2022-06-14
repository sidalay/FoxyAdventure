#ifndef SPRITE_HPP
#define SPRITE_HPP

#include <raylib.h>

struct Sprite
{
    Sprite() = default;
    explicit Sprite(const Texture2D& Texture, const int MaxFramesX = 1, const int MaxFramesY = 1, const float UpdateSpeed = 1.f/8.f);
    void Tick(float DeltaTime);
    Rectangle GetSourceRec();
    Rectangle GetPosRec(const Vector2& ScreenPos, const float Scale);

    const Texture2D Texture{};
    int MaxFramesX{};
    int MaxFramesY{};
    int FrameX{};
    int FrameY{};
    int PreviousFrame{};
    float UpdateTime{1.f/8.f};
    float RunningTime{};
};

#endif // SPRITE_HPP