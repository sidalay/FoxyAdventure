#ifndef CHARACTER_HPP
#define CHARACTER_HPP

#include <raylib.h>
#include <raymath.h>
#include <vector>
#include "headers/sprite.hpp"
#include "headers/window.hpp"
#include "headers/background.hpp"
#include "headers/basecharacter.hpp"

class Character : BaseCharacter
{
private:
    Sprite Idle{};
    Sprite Walk{};
    Sprite Run{};
    Sprite Attack{};
    Sprite Hurt{};
    Sprite Death{};
    Sprite* CurrentSprite{&Idle};  
    std::vector<Sprite*> Sprites {};

    Window* Screen{};
    Background* World{};
    Vector2 CharacterPos{};
    Vector2 WorldPos{};
    Vector2 PrevWorldPos{};
    Rectangle Source{};
    Rectangle Destination{};
    
    float Scale{1.5f};
    float Speed{1.5f};
    bool Walking{false};
    bool Running{false};
    bool Attacking{false};
    Direction Face{Direction::DOWN};

public:
    Character(Sprite Idle, Sprite Walk, Sprite Run, Sprite Attack, Sprite Hurt, Sprite Death, Window* Screen, Background* World);
    ~Character();
    void Tick(float DeltaTime);
    void Draw();
    void SpriteTick(float DeltaTime);
    void UpdateCharacterPos();
    void CheckDirection();
    void CheckMovement();
    void WalkOrRun();
    void CheckAttack();
    void UpdateSource();

    Vector2 GetWorldPos() {return WorldPos;}
    Vector2 GetCharPos() {return CharacterPos;}
};

#endif