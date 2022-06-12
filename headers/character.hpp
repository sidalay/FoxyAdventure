#ifndef CHARACTER_HPP
#define CHARACTER_HPP

#include <raylib.h>
#include <raymath.h>
#include <vector>
#include "prop.hpp"
#include "sprite.hpp"
#include "enemy.hpp"
#include "window.hpp"
#include "background.hpp"
#include "basecharacter.hpp"
#include "gametextures.hpp"

enum class Emotion 
{
    DEFAULT, ANGRY, HAPPY, NERVOUS, SAD, SLEEPING, HURT, DEAD
};

class Character : public BaseCharacter
{
public:
    Character(const Sprite& Idle, 
              const Sprite& Walk, 
              const Sprite& Run, 
              const Sprite& Attack, 
              const Sprite& Hurt, 
              const Sprite& Death, 
              const Sprite& Push, 
              const Sprite& Sleep, 
              const Sprite& ItemGrab,
              GameTexture& GameTextures, 
              Window* Screen, 
              Background* World);
    
    void Tick(float DeltaTime, Props& Props, std::vector<Enemy>& Enemies, std::vector<Prop>& Trees);
    void Draw();
    void SpriteTick(float DeltaTime);
    void UpdateScreenPos();
    void CheckDirection();
    void CheckMovement(Props& Props, std::vector<Enemy>& Enemies, std::vector<Prop>& Trees);
    void UndoMovement();
    void CheckCollision(std::vector<std::vector<Prop>>& Props, Vector2 Direction, std::vector<Enemy>& Enemies, std::vector<Prop>& Trees);
    void WalkOrRun();
    void CheckAttack();
    void UpdateSource();
    void CheckSleep();
    void CheckEmotion();
    void CheckIfAlive();
    void SetSleep() {Sleeping = !Sleeping;}
    void DrawIndicator();
    void TakeDamage();

    bool IsAlive() {return Alive;}
    bool IsAttacking() {return Attacking;}
    float GetHealth() {return Health;}
    float GetSpeed() {return Speed;}
    Vector2 GetWorldPos() {return WorldPos;}
    Vector2 GetPrevWorldPos() {return PrevWorldPos;}
    Vector2 GetCharPos() {return ScreenPos;}
    Emotion GetEmotion() {return State;}
    Rectangle GetCollisionRec();
    Rectangle GetAttackRec();

    // Debug function
    void AddHealth(float HP);
    void HealOverTime(float HP);
    void CheckHealing();
    void SwitchCollidable() {Collidable = !Collidable;}
    
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
    GameTexture& GameTextures;  

    Window* Screen{};
    Background* World{};
    Vector2 Offset{615.f,335.f};         // Player offset vs Enemy/Prop WorldPos
    Vector2 ScreenPos{};              // Where the character is on the screen
    Vector2 WorldPos{362.f,2594.f};      // Where the character is in the world
    // Vector2 WorldPos{2334.f,2431};
    Vector2 PrevWorldPos{};
    Rectangle Source{};
    Rectangle Destination{};
    float Health{11.f};
    float Scale{1.5f};
    float Speed{1.0f};
    float RunningTime{};
    float DamageTime{};
    float AttackTime{};
    float HealTime{};
    float AmountToHeal{};
    bool Colliding{false};
    bool Locked{false};
    bool Walking{false};
    bool Running{false};
    bool Attacking{false};
    bool Sleeping{false};
    bool Interacting{false};
    bool Interactable{false};
    bool Hurting{false};
    bool Healing{false};
    Emotion State{Emotion::DEFAULT};
    Direction Face{Direction::DOWN};

    // Debug controls
    bool Collidable{true};
};

#endif // CHARACTER_HPP

/*
    TODO:
        [x] add attack cooldown
        [x] add damage knockback
        [x] add half-hearts to hp
        [x] fix hurt portrait not showing up
        [x] heal fox when killing enemy
        [x] add healing animation
        [x] redo attacking and combos to feel less clunky
*/