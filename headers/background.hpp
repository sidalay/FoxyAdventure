#ifndef BACKGROUND_HPP
#define BACKGROUND_HPP

#include <raylib.h>
#include <raymath.h>
#include "headers/window.hpp"

class Background
{
private:
    Texture2D Map {LoadTexture("sprites/maps/CodexMap2.png")};
    Texture2D MiniMap {LoadTexture("sprites/maps/CodexMap4.png")};
    Vector2 MapPos{0.f, 0.f};
    float Scale{4.f};
    bool IsMiniMap{false};
public:
    Background();
    ~Background();
    void Tick(Vector2 WorldPos);
    void Draw();
    void DrawMiniMap(Vector2 CharWorldPos);
    Texture2D GetMap() {return Map;}
    bool IsMiniMapOn() {return IsMiniMap;}
    Vector2 GetMapPos() {return MapPos;}
    float GetScale() {return Scale;}
};

#endif