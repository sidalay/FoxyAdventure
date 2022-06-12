#ifndef BACKGROUND_HPP
#define BACKGROUND_HPP

#include <raylib.h>
#include <raymath.h>
#include "window.hpp"
#include "gametextures.hpp"

class Background
{
public:
    explicit Background(GameTexture& GameTextures);
    ~Background() = default;
    Background(const Background&) = delete;
    Background(Background&&) = default;
    Background& operator=(const Background&) = delete;
    Background& operator=(Background&&) = default;

    void Tick(Vector2 WorldPos);
    void Draw();
    void DrawMiniMap(Vector2 CharWorldPos);
    Vector2 GetMapSize();
    Vector2 GetMapPos() {return MapPos;}
    bool IsMiniMapOn() {return MiniMapOpen;}
    float GetScale() {return Scale;}
    
private:
    GameTexture* GameTextures{};
    Vector2 MapPos{0.f, 0.f};
    float Scale{4.f};
    bool MiniMapOpen{false};
};

#endif // BACKGROUND_HPP