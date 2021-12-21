#ifndef BACKGROUND_HPP
#define BACKGROUND_HPP

#include <raylib.h>
#include <raymath.h>
#include "headers/window.hpp"

class Background
{
private:
    Texture2D Map {LoadTexture("sprites/maps/CodexMap2.png")};
    Vector2 MapPos{0.f, 0.f};
    Vector2 PreviousMapPos{};
    float Speed{1.5f};
    float Scale{4.f};
public:
    Background();
    ~Background();
    void Tick(Vector2 WorldPos);
    void Draw();
    Texture2D GetMap() {return Map;}
    float GetScale() {return Scale;}
};

#endif