#ifndef ENEMY_HPP
#define ENEMY_HPP

#include <random>
#include <unordered_map>
#include "prop.hpp"
#include "window.hpp"
#include "randomizer.hpp"

enum class EnemyType 
{
    BEAR, BEHOLDER, CREATURE, GHOST, 
    IMP, MUSHROOM, NECROMANCER, SHADOW, 
    SPIDER, TOAD, NORMAL, BOSS, NPC,
    FOX, CROW, SQUIRREL, DEFAULT
};

enum class Monster 
{
    IDLE, WALK, ATTACK, HURT, DEATH, PROJECTILE
}; 

enum class NPC 
{
    IDLE, IDLETWO, WALK, MISC, SLEEP
};

struct Enemies;

class Enemy
{
public:
    // Enemy constructor
    Enemy(const Sprite& Idle,
          const Sprite& Walk,
          const Sprite& Attack,
          const Sprite& Hurt,
          const Sprite& Death,
          const Sprite& Projectile,
          const EnemyType Race,
          const EnemyType Type,
          const Vector2 WorldPos,
          const Window& Screen,
          Background& World,
          const GameTexture& GameTextures,
          Randomizer& RandomEngine,
          const int Health = 3,
          const float Scale = 3.2f,
          const EnemyType BossSpawner = EnemyType::DEFAULT);
    
    // Wildlife NPC constructor
    Enemy(const Sprite& NpcIdle,
          const Sprite& NpcIdleTwo,
          const Sprite& NpcWalk,
          const Sprite& NpcMisc,
          const Sprite& NpcSleep,
          const EnemyType Race,
          const Vector2 WorldPos,
          const Window& Screen,
          Background& World,
          const GameTexture& GameTextures,
          Randomizer& RandomEngine,
          const float Scale = 3.2f);

    void Tick(float DeltaTime, Props& Props, const Vector2 HeroWorldPos, const Vector2 HeroScreenPos, std::vector<Enemy>& Enemies, std::vector<Prop>& Trees);
    void Draw(const Vector2 HeroWorldPos);
    void SpriteTick(float DeltaTime);
    void UpdateScreenPos(Vector2 HeroWorldPos);
    void CheckDirection();
    void CheckMovement(Props& Props, const Vector2 HeroWorldPos, const Vector2 HeroScreenPos, std::vector<Enemy>& Enemies, std::vector<Prop>& Trees);
    void NeutralAction();
    void UndoMovement();
    void CheckOutOfBounds();
    void CheckCollision(std::vector<std::vector<Prop>>& Props, const Vector2 HeroWorldPos, std::vector<Enemy>& Enemies, std::vector<Prop>& Trees);
    void CheckAttack();
    void UpdateSource();
    void TakeDamage();
    void CheckAlive(float DeltaTime);
    void EnemyAI();
    void EnemyAggro(const Vector2 HeroScreenPos);
    void CheckMovementAI();
    void InitializeAI();
    void DrawHP();
    void CheckBossSummon(const Vector2 HeroWorldPos);

    constexpr void Damaged(const bool Attacked) {IsAttacked = Attacked;}
    constexpr bool IsBlocked() const {return Blocked;}
    constexpr bool IsAlive() const {return Alive;}
    constexpr bool IsSummoned() const {return Summoned;}
    constexpr bool IsInvulnerable() const {return Invulnerable;}
    constexpr bool IsAttacking() const {return Attacking;}
    constexpr bool IsDying() const {return Dying;}
    constexpr int GetHealth() const {return Health;}
    constexpr int GetMaxHP() const {return MaxHP;}
    int GetMonstersKilled() {return MonsterDeaths;}
    int GetTotalMonsters() {return MonsterCount;}
    constexpr Vector2 GetWorldPos() const {return WorldPos;}
    constexpr Vector2 GetPrevWorldPos() const {return PrevWorldPos;}
    constexpr Vector2 GetEnemyPos() const {return ScreenPos;}
    constexpr EnemyType GetType() const {return Type;}
    constexpr EnemyType GetRace() const {return Race;}
    Rectangle GetCollisionRec();
    Rectangle GetAttackRec();
    Vector2 UpdateProjectile();
    std::array<Vector2,3> UpdateMultiProjectile();
    bool WithinScreen(const Vector2 HeroWorldPos);
    int GetMonsterCount(const EnemyType Type) {return MonsterCounter.at(Type);}
    
private:
    std::vector<Sprite> Sprites{};

    const EnemyType Race{};
    const EnemyType Type{};           // If the Enemy is NORMAL, BOSS, or NPC type

    Vector2 ScreenPos{};                  
    Vector2 WorldPos{};                  
    Vector2 PrevWorldPos{};
    Vector2 Movement{};
    Rectangle Source{};
    Rectangle Destination{};
    const Window& Screen;
    Background& World;
    const GameTexture& GameTextures;
    Randomizer& RandomEngine;
    EnemyType BossSpawner{};    // Spawn Boss after this EnemyType is all killed
    
    static int MonsterDeaths;
    static int MonsterCount;
    int CurrentSpriteIndex{};
    int ShootingSpriteIndex{};
    int Health{};
    int MaxHP{};
    int LeftOrRight{};
    int ActionState{};
    float Scale{3.2f};
    float Speed{1.4f};
    float Radius{25.f};                 
    float MaxRange{150.f};
    float MinRange{60.f};
    float MinCollisionRange{0.5f};
    float RunningTime{};
    float DamageTime{};
    float StopTime{};
    float ActionTime{};
    float ActionIdleTime{};
    float MoveXRange{};
    float MoveYRange{};
    float AIX{0.6f};
    float AIY{0.6f};
    float Trajectory{1.f};
    bool Alive{true};
    bool Colliding{false};
    bool Attacking{false};
    bool Invulnerable{false};
    bool Chasing{false};
    bool Walking{false};
    bool IsAttacked{false};
    bool Stopped{false};
    bool Dying{false};
    bool Blocked{false};            // Whether enemy is colliding with an object or not
    bool Hurting{false};
    bool InitializedAI{false};
    bool OOB{false};
    bool Ranged{false};
    bool Summoned{true};
    bool Visible{false};

    // Wildlife NPC
    bool IdleTwo{false};
    bool Sleeping{false};
    bool MiscAction{false};
    
    Direction Face{Direction::DOWN};

    std::uniform_int_distribution<int> RandomRange{60,80};
    std::uniform_int_distribution<int> RandomIdleTime{3,6};
    std::uniform_int_distribution<int> RandomLeftRight{1,10};
    std::uniform_int_distribution<int> RandomActionState{1,10};

    static std::unordered_map<EnemyType, int> MonsterCounter;
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
        [x] fix OutOfBounds if check parameters
        [x] fix prop collision not being registered correctly sometimes
        [x] add HP bars
        [x] fix death animation not ticking correctly
        [x] add tombstone for death animation
        [x] add and implement enemy types
        [x] add different enemy attack types ranged/melee
        [x] add monster counter by EnemyType
        [x] add 'boss' after defeating all of one enemy type
        [x] fix enemy collision rec
        [x] wildlife npcs
        [x] refactor crow collision and movement ai
*/