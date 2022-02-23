#ifndef ENEMY_HPP
#define ENEMY_HPP

#include <raylib.h>
#include <vector>
#include "headers/prop.hpp"
#include "headers/sprite.hpp"
#include "headers/window.hpp"
#include "headers/background.hpp"
#include "headers/basecharacter.hpp"

struct Enemies;

class Enemy : public BaseCharacter
{
private:
    Sprite Idle{};
    Sprite Walk{};
    Sprite Attack{};
    Sprite Hurt{};
    Sprite Death{};
    Sprite* CurrentSprite{&Idle};
    std::vector<Sprite*> Sprites{};

    Vector2 CharacterPos{};              // Where the character is on the screen
    Vector2 WorldPos{};                  // Where the character is in the world
    Vector2 PrevWorldPos{};
    Rectangle Source{};
    Rectangle Destination{};
    Window* Screen{};
    Background* World{};
    
    int Health{10};
    float Scale{3.2f};
    float Speed{1.0f};
    float RunningTime{};
    bool Colliding{false};
    bool Attacking{false};

    Direction Face{Direction::DOWN};
public:
    Enemy(Sprite Idle,
          Sprite Walk,
          Sprite Attack,
          Sprite Hurt,
          Sprite Death,
          Vector2 WorldPos,
          Window* Screen,
          Background* World);
    ~Enemy();

    void Tick(float DeltaTime, Props& Props, Vector2 HeroWorldPos);
    void Draw(Vector2 HeroWorldPos);
    void SpriteTick(float DeltaTime);
    void CheckDirection();
    void CheckMovement(Props& Props, Vector2 HeroWorldPos);
    void UndoMovement();
    void CheckCollision(std::vector<std::vector<Prop>>* Props, Vector2 Direction, Vector2 HeroWorldPos);
    void CheckAttack(Props& Props, Vector2 HeroWorldPos);
    void UpdateSource();

    int GetHealth() {return Health;}
    Vector2 GetWorldPos() {return WorldPos;}
    Vector2 GetPrevWorldPos() {return PrevWorldPos;}
    Vector2 GetCharPos() {return CharacterPos;}
    Rectangle GetCollisionRec(Vector2 HeroWorldPos);
};

struct Enemies
{
    std::vector<std::vector<Enemy>>* Enemies{};
};

#endif