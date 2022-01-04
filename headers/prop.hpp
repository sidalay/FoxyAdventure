#ifndef PROP_HPP
#define PROP_HPP

#include <raylib.h>
#include <raymath.h>
#include <vector>
#include "headers/sprite.hpp"
#include "headers/background.hpp"

enum class PropType
{
    TREE, 
    ROCK, 
    BOULDER, 
    GRASS, 
    FLOWER,
    rHOUSELEFT, rHOUSERIGHT, DOOR,
    bHOUSELEFT, bHOUSERIGHT,
    FENCE, 
    TOPWALL, LEFTSIDEWALL, RIGHTSIDEWALL, BOTTOMWALL, 
    BUSH, 
    STUMP, 
    HOLE, 
    TREASURE,
    DUNGEON, DUNGEONLEFT, DUNGEONRIGHT,
    ALTAR
};

struct Props;

class Prop 
{
private:
    Sprite Object{};
    PropType Type{};
    Vector2 WorldPos{};     // Where the prop is located in the world
    Vector2 PrevWorldPos{};
    float Scale {4.f};
    bool Active{false};
    bool Collidable{false};
    bool Interactable{false};
    bool OutOfBounds{false};

public:
    Prop(const char* TexturePath, Vector2 Pos, PropType Type, float Scale = 4.f, bool Interactable = false);
    Prop(Sprite Object, Vector2 Pos, PropType Type, float Scale = 4.f, bool Interactable = false);              
    
    void Tick(float DeltaTime, Background& Map);
    void Draw(Vector2 CharacterWorldPos);

    bool HasCollision() {return Collidable;}
    bool IsInteractable() {return Interactable;}
    bool IsOutOfBounds() {return OutOfBounds;}
    
    Vector2 GetWorldPos() {return WorldPos;}
    PropType GetType() {return Type;}
    Rectangle GetCollisionRec(Vector2 CharacterWorldPos);
    
    void SetWorldPos(Vector2 Direction);
    void SetActive(bool Activity) {Active = Activity;}
    void UndoMovement() {WorldPos = PrevWorldPos;}
    bool CheckMovement(Background& Map, Vector2 CharWorldPos, Vector2 Direction, float Speed, std::vector<std::vector<Prop>>* Props);
};

struct Props
{
    std::vector<std::vector<Prop>>* Under{};
    std::vector<std::vector<Prop>>* Over{};

    Props(std::vector<std::vector<Prop>>* Under, std::vector<std::vector<Prop>>* Over);
};

#endif