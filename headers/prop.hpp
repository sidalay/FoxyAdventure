#ifndef PROP_HPP
#define PROP_HPP

#include <raylib.h>
#include <raymath.h>
#include "headers/sprite.hpp"

enum class PropType
{
    TREE, ROCK, GRASS, FLOWER, FENCE, WALL
};

class Prop 
{
private:
    Sprite Object{};
    PropType Type{};
    Vector2 WorldPos{};     // Where the prop is located in the world
    float Scale {4.f};
public:
    // Constructor for inanimate props
    Prop(const char* TexturePath, Vector2 Pos, PropType Type, float Scale = 4.f);
    // Constructor for animated props
    Prop(Sprite Object, Vector2 Pos, PropType Type, float Scale = 4.f);
    // Tick function for animated Props               
    void Tick(float DeltaTime);
    void Draw(Vector2 CharacterWorldPos);
    Rectangle GetCollisionRec(Vector2 CharacterWorldPos);
};

#endif