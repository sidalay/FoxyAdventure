#ifndef CHARACTER_HPP
#define CHARACTER_HPP

#include "enemy.hpp"

enum class Emotion 
{
    DEFAULT, ANGRY, HAPPY, NERVOUS, SAD, SLEEPING, HURT, DEAD
};

enum class FoxState 
{
    IDLE, WALK, RUN, ATTACK, HURT, DEATH, PUSH, SLEEP, ITEMGRAB
};

class Character
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
              const GameTexture& GameTextures, 
              const Window& Screen, 
              Background& World);
    
    void Tick(float DeltaTime, Props& Props, std::vector<Enemy>& Enemies, std::vector<Prop>& Trees);
    void Draw();
    void SpriteTick(float DeltaTime);
    void UpdateScreenPos();
    void CheckDirection();
    void CheckMovement(Props& Props, std::vector<Enemy>& Enemies, std::vector<Prop>& Trees);
    void UndoMovement();
    void CheckCollision(std::vector<std::vector<Prop>>& Props, const Vector2 Direction, std::vector<Enemy>& Enemies, std::vector<Prop>& Trees);
    void WalkOrRun();
    void CheckAttack();
    void UpdateSource();
    void CheckSleep();
    void CheckEmotion();
    void CheckIfAlive();
    void CheckSecretSpot();
    void SetSleep() {Sleeping = !Sleeping;}
    void SetWorldPos(Vector2 Pos) {WorldPos = Pos;}
    void DrawIndicator();
    void TakeDamage();

    constexpr bool IsAlive() const {return Alive;}
    constexpr bool IsAttacking() const {return Attacking;}
    constexpr float GetHealth() const {return Health;}
    constexpr float GetSpeed() const {return Speed;}
    constexpr Vector2 GetOffset() const {return Offset;}
    constexpr Vector2 GetWorldPos() const {return WorldPos;}
    constexpr Vector2 GetPrevWorldPos() const {return PrevWorldPos;}
    constexpr Vector2 GetCharPos() const {return ScreenPos;}
    constexpr Emotion GetEmotion() const {return State;}
    Rectangle GetCollisionRec();
    Rectangle GetAttackRec();

    // Debug function
    void AddHealth(float HP);
    void HealOverTime(float HP, float TimeToHeal = 0.6f);
    void CheckHealing();
    void SwitchCollidable() {Collidable = !Collidable;}
    
private:
    const GameTexture& GameTextures;
    const Window& Screen;
    Background& World;
    Vector2 Offset{615.f,335.f};         // Player offset vs Enemy/Prop WorldPos
    Vector2 ScreenPos{};                 // Where the character is on the screen
    Vector2 WorldPos{436.f,3758.f};      // Where the character is in the world
    // Vector2 WorldPos{2334.f,2431};
    Vector2 PrevWorldPos{};
    Rectangle Source{};
    Rectangle Destination{};
    std::vector<Sprite> Sprites{};

    int SpriteIndex{};
    float Health{11.f};
    float Scale{1.5f};
    float Speed{1.0f};
    float RunningTime{};
    float DamageTime{};
    float AttackTime{};
    float HealTime{};
    float DamageTaken{0.5f};
    float AmountToHeal{};
    bool Alive{true};
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