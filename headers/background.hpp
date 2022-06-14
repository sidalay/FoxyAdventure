#ifndef BACKGROUND_HPP
#define BACKGROUND_HPP

#include "gametextures.hpp"
#include <raymath.h>

enum class Direction
{
    UP, DOWN, LEFT, RIGHT
};

class Background
{
public:
    explicit Background(const GameTexture& GameTextures);
    ~Background() = default;
    constexpr Background(const Background&) = delete;
    constexpr Background(Background&&) = default;
    constexpr Background& operator=(const Background&) = delete;
    constexpr Background& operator=(Background&&) = default;

    void Tick(Vector2 WorldPos);
    void Draw();
    void DrawMiniMap(Vector2 CharWorldPos);
    Vector2 GetMapSize();
    constexpr Vector2 GetMapPos() const {return MapPos;}
    constexpr bool IsMiniMapOn() const {return MiniMapOpen;}
    constexpr float GetScale() const {return Scale;}
    
private:
    const GameTexture& GameTextures;
    Vector2 MapPos{0.f, 0.f};
    float Scale{4.f};
    bool MiniMapOpen{false};
};

#endif // BACKGROUND_HPP