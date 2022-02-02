#ifndef NPC_HPP
#define NPC_HPP

#include <raylib.h>
#include <raymath.h>
#include "headers/sprite.hpp"
#include "headers/basecharacter.hpp"

class NPC
{
private:
    Sprite Idle{};
    Sprite* CurrentSprite{&Idle}; 
    Vector2 WorldPos{};
    Rectangle Source{};
    Rectangle Destination{};

    float Scale{3.f};
public:
    NPC(Sprite Idle, Vector2 WorldPos);
    ~NPC();

    void Tick();
    void Draw(Vector2 CharacterWorldPos);
    void UpdateSource();
};

#endif