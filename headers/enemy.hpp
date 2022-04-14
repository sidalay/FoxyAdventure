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
    Sprite* PreviousSprite{};
    std::vector<Sprite*> Sprites{};

    Vector2 EnemyPos{};                  // Where the character is on the screen
    Vector2 WorldPos{};                  // Where the character is in the world
    Vector2 PrevWorldPos{};
    Rectangle Source{};
    Rectangle Destination{};
    Window* Screen{};
    Background* World{};
    
    int Health{10};
    float Scale{3.2f};
    float Speed{1.4f};
    float Radius{25.f};                 
    float Range{150.f};
    float RunningTime{};
    float DamageTime{};
    float StopTime{};
    bool Colliding{false};
    bool Attacking{false};
    bool Moving{false};
    bool IsAttacked{false};
    bool Stopped{false};            // Whether or not the enemy should be able to move

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

    void Tick(float DeltaTime, Props& Props, Vector2 HeroWorldPos, Vector2 HeroScreenPos);
    void Draw(Vector2 HeroWorldPos);
    void SpriteTick(float DeltaTime);
    void CheckDirection();
    void CheckMovement(Props& Props, Vector2 HeroWorldPos, Vector2 HeroScreenPos);
    void WalkOrRun();
    void UndoMovement();
    void CheckCollision(std::vector<std::vector<Prop>>* Props, Vector2 HeroScreenPos);
    void CheckAttack();
    void UpdateSource();
    void TakingDamage();
    void Damaged(bool Attacked) {IsAttacked = Attacked;}
    void CheckAlive();

    bool IsAlive() {return Alive;}
    int GetHealth() {return Health;}
    Vector2 GetWorldPos() {return WorldPos;}
    Vector2 GetPrevWorldPos() {return PrevWorldPos;}
    Vector2 GetCharPos() {return EnemyPos;}
    Rectangle GetCollisionRec();
};

#endif