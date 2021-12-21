#ifndef PROP_HPP
#define PROP_HPP

#include <raylib.h>
#include <raymath.h>
#include <vector>
#include "headers/sprite.hpp"

enum class PropType
{
    TREE, ROCK, BOULDER, GRASS, FLOWER, FENCE, WALL, BUSH, STUMP
};

class Prop 
{
private:
    Sprite Object{};
    PropType Type{};
    Vector2 WorldPos{};     // Where the prop is located in the world
    float Scale {4.f};
    bool Collidable{false};
    bool Interactable{};

public:
    // Constructor for inanimate props
    Prop(const char* TexturePath, Vector2 Pos, PropType Type, float Scale = 4.f, bool Interactable = false);
    // Constructor for animated props
    Prop(Sprite Object, Vector2 Pos, PropType Type, float Scale = 4.f, bool Interactable = false);              
    void Tick(float DeltaTime);
    void Draw(Vector2 CharacterWorldPos);
    bool HasCollision() {return Collidable;}
    bool IsInteractable() {return Interactable;}
    Vector2 GetWorldPos() {return WorldPos;}
    PropType GetType() {return Type;}
    Rectangle GetCollisionRec(Vector2 CharacterWorldPos);
    void SetWorldPos(Vector2 Direction);
};

struct Props
{
    std::vector<std::vector<Prop>>* Under{};
    std::vector<std::vector<Prop>>* Over{};

    Props(std::vector<std::vector<Prop>>* Under, std::vector<std::vector<Prop>>* Over);
};

#endif