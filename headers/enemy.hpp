#ifndef ENEMY_HPP
#define ENEMY_HPP

#include <raylib.h>
#include <vector>
#include <random>
#include "headers/prop.hpp"
#include "headers/sprite.hpp"
#include "headers/window.hpp"
#include "headers/background.hpp"
#include "headers/basecharacter.hpp"

enum class EnemyType 
{
    BEAR, BEHOLDER, CREATURE, GHOST, 
    IMP, MUSHROOM, NECROMANCER, SHADOW, 
    SPIDER, TOAD, NORMAL, BOSS
};

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
    Sprite PreviousSprite{};
    std::vector<Sprite> Sprites{};
    Texture2D LifeBarLeft_Full{LoadTexture("sprites/enemies/lifebar/round_left_filled.png")};
    Texture2D LifeBarMiddle_Full{LoadTexture("sprites/enemies/lifebar/round_middle_filled.png")};
    Texture2D LifeBarRight_Full{LoadTexture("sprites/enemies/lifebar/round_right_filled.png")};
    Texture2D LifeBarLeft_Empty{LoadTexture("sprites/enemies/lifebar/round_left_empty.png")};
    Texture2D LifeBarMiddle_Empty{LoadTexture("sprites/enemies/lifebar/round_middle_empty.png")};
    Texture2D LifeBarRight_Empty{LoadTexture("sprites/enemies/lifebar/round_right_empty.png")};

    Vector2 EnemyPos{};                  // Where the character is on the screen
    Vector2 WorldPos{};                  // Where the character is in the world
    Vector2 PrevWorldPos{};
    Vector2 Movement{};
    Rectangle Source{};
    Rectangle Destination{};
    Window* Screen{};
    Background* World{};
    
    static int MonsterDeaths;
    static int MonsterCount;
    int Health{};
    int MaxHP{};
    int LeftOrRight{};
    float Scale{3.2f};
    float Speed{1.4f};
    float Radius{25.f};                 
    float MaxRange{150.f};
    float MinRange{60.f};
    float MinCollisionRange{0.5f};
    float RunningTime{};
    float DamageTime{};
    float StopTime{};
    float WalkingTime{};
    float MovementIdleTime{};
    float MoveXRange{};
    float MoveYRange{};
    float AIX{0.6};
    float AIY{0.6};
    bool Colliding{false};
    bool Attacking{false};
    bool Invulnerable{false};
    bool Chasing{false};
    bool Walking{false};
    bool IsAttacked{false};
    bool Stopped{false};            // Whether or not the enemy should be able to move
    bool Dead{false};
    bool Blocked{false};            // Whether enemy is colliding with an object or not
    bool Hurting{false};
    bool Intro{true};
    bool OOB{false};

    Direction Face{Direction::DOWN};
public:
    Enemy(Sprite Idle,
          Sprite Walk,
          Sprite Attack,
          Sprite Hurt,
          Sprite Death,
          Vector2 WorldPos,
          Window* Screen,
          Background* World,
          int Health = 3,
          float Scale = 3.2f);
    ~Enemy();

    void Tick(float DeltaTime, Props& Props, Vector2 HeroWorldPos, Vector2 HeroScreenPos, std::vector<Enemy>& Enemies);
    void Draw(Vector2 HeroWorldPos);
    void SpriteTick(float DeltaTime);
    void CheckDirection();
    void CheckMovement(Props& Props, Vector2 HeroWorldPos, Vector2 HeroScreenPos, std::vector<Enemy>& Enemies);
    void WalkOrRun();
    void UndoMovement();
    void OutOfBounds();
    void CheckCollision(std::vector<std::vector<Prop>>* Props, Vector2 HeroWorldPos, std::vector<Enemy>& Enemies);
    void CheckAttack();
    void UpdateSource();
    void TakingDamage();
    void Damaged(bool Attacked) {IsAttacked = Attacked;}
    void CheckAlive(float DeltaTime);
    void EnemyAI();
    void EnemyAggro(Vector2 HeroScreenPos);
    void DrawHP();

    bool IsBlocked() {return Blocked;}
    bool IsAlive() {return Alive;}
    bool IsInvulnerable() {return Invulnerable;}
    bool IsAttacking() {return Attacking;}
    int GetHealth() {return Health;}
    int GetMonstersKilled() {return MonsterDeaths;}
    int GetTotalMonsters() {return MonsterCount;}
    Vector2 GetWorldPos() {return WorldPos;}
    Vector2 GetPrevWorldPos() {return PrevWorldPos;}
    Vector2 GetEnemyPos() {return EnemyPos;}
    Rectangle GetCollisionRec();
    Rectangle GetAttackRec();
};

struct Enemies
{
    std::vector<Enemy>* Monsters{};

    Enemies(std::vector<Enemy>* Monsters);
};

#endif // ENEMY_HPP

/* 
    TODO: 
        [x] figure out orientation when chasing   
        [x] add min range to enemy aggro         
        [x] implement new undo movement system                    
        [x] bounce back when being attacked      
        [x] add movement ai                      
        [x] implement attack
        [x] add randomization to movement
        [x] fix multiple enemy instances
        [x] prevent enemies from 'overlapping' and walking over each other
        [ ] fix OutOfBounds if check parameters
        [x] add HP bars
        [x] fix death animation not ticking correctly
        [x] add tombstone for death animation
        [ ] add heart loot drops
        [ ] add and implement enemy types
        [ ] add 'boss' after defeating all of one enemy type
        [x] fix enemy collision rec
*/