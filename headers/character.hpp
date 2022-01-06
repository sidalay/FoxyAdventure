#ifndef CHARACTER_HPP
#define CHARACTER_HPP

#include <raylib.h>
#include <raymath.h>
#include <vector>
#include "headers/prop.hpp"
#include "headers/sprite.hpp"
#include "headers/window.hpp"
#include "headers/background.hpp"
#include "headers/basecharacter.hpp"

enum class Emotion 
{
    DEFAULT, ANGRY, HAPPY, NERVOUS, SAD, SLEEPING
};

class Character : BaseCharacter
{
private:
    Sprite Idle{};
    Sprite Walk{};
    Sprite Run{};
    Sprite Attack{};
    Sprite Hurt{};
    Sprite Death{};
    Sprite Push{};
    Sprite Sleep{};
    Sprite ItemGrab{};
    Sprite* CurrentSprite{&Idle};  
    std::vector<Sprite*> Sprites {};

    Window* Screen{};
    Background* World{};
    Vector2 CharacterPos{};              // Where the character is on the screen
    Vector2 WorldPos{1899,2125};         // Where the character is in the world
    Vector2 PrevWorldPos{};
    Rectangle Source{};
    Rectangle Destination{};
    
    int Health{5};
    float Scale{1.5f};
    float Speed{1.0f};
    bool Colliding{false};
    bool Walking{false};
    bool Running{false};
    bool Attacking{false};
    Emotion State{Emotion::DEFAULT};
    Direction Face{Direction::DOWN};

public:
    Character(Sprite Idle, 
              Sprite Walk, 
              Sprite Run, 
              Sprite Attack, 
              Sprite Hurt, 
              Sprite Death, 
              Sprite Push, 
              Sprite Sleep, 
              Sprite ItemGrab, 
              Window* Screen, 
              Background* World);
    ~Character();
    
    void Tick(float DeltaTime, Props& Props);
    void Draw();
    void SpriteTick(float DeltaTime);
    void UpdateCharacterPos();
    void CheckDirection();
    void CheckMovement(Props& Props);
    void UndoMovement();
    void CheckCollision(std::vector<std::vector<Prop>>* Props, Vector2 Direction);
    void WalkOrRun();
    void CheckAttack(Props& Props);
    void UpdateSource();
    void CheckEmotion();

    int GetHealth() {return Health;}
    float GetSpeed() {return Speed;}
    Vector2 GetWorldPos() {return WorldPos;}
    Vector2 GetPrevWorldPos() {return PrevWorldPos;}
    Vector2 GetCharPos() {return CharacterPos;}
    Emotion GetEmotion() {return State;}
    Rectangle GetCollisionRec();

    // Debug function
    void SetHealth(int HP);
};

#endif