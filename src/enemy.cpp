#include "enemy.hpp"

int Enemy::MonsterCount{};
int Enemy::MonsterDeaths{};

std::unordered_map<EnemyType, int> Enemy::MonsterCounter
{
    {EnemyType::BEAR, 0}, {EnemyType::BEHOLDER, 0}, 
    {EnemyType::CREATURE, 0}, {EnemyType::GHOST, 0}, 
    {EnemyType::IMP, 0}, {EnemyType::MUSHROOM, 0},
    {EnemyType::NECROMANCER, 0}, {EnemyType::SHADOW, 0}, 
    {EnemyType::SPIDER, 0}, {EnemyType::TOAD, 0},
    {EnemyType::BOSS, 0} 
};

Enemy::Enemy(const Sprite& Idle,
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
             const GameAudio& Audio,
             Randomizer& RandomEngine,
             const int Health,
             const float Scale, 
             const EnemyType BossSpawner)
    : Race{Race},
      Type{Type},
      WorldPos{WorldPos},
      Screen{Screen},
      World{World},
      GameTextures{GameTextures},
      Audio{Audio},
      RandomEngine{RandomEngine},
      BossSpawner{BossSpawner},
      Health{Health},
      MaxHP{Health},
      Scale{Scale}
{
    // Fill vector<Sprite*> with Sprite objects to easily loop through and call Sprite::Tick()
    Sprites.emplace_back(Idle);
    Sprites.emplace_back(Walk);
    Sprites.emplace_back(Attack);
    Sprites.emplace_back(Hurt);
    Sprites.emplace_back(Death);
    Sprites.emplace_back(Projectile);

    CurrentSpriteIndex = static_cast<int>(Monster::IDLE);
    ShootingSpriteIndex = static_cast<int>(Monster::PROJECTILE);

    // Static variable to count how many enemies on the field
    if (Type == EnemyType::NORMAL) {
        MonsterCounter[Race] += 1;
    }
    else if (Type == EnemyType::BOSS) {
        MonsterCounter[EnemyType::BOSS] += 1;
        Summoned = false;
    }
    else {
        Summoned = false;
    }

    if (BossSpawner == EnemyType::DEFAULT) {
        this->BossSpawner = Race;
    }

    if (Race == EnemyType::BEHOLDER || Race == EnemyType::NECROMANCER || Race == EnemyType::IMP) {
        Ranged = true;
    }

    MonsterCount += 1;

    // Generate RNG for current object used for randomizing AI movement
    ActionIdleTime = static_cast<float>(RandomEngine.Randomize(RandomIdleTime));
    MoveXRange = RandomEngine.Randomize(RandomRange);
    MoveYRange = RandomEngine.Randomize(RandomRange);
    LeftOrRight = RandomEngine.Randomize(RandomLeftRight);
}

Enemy::Enemy(const Sprite& NpcIdle,
             const Sprite& NpcIdleTwo,
             const Sprite& NpcWalk,
             const Sprite& NpcMisc,
             const Sprite& NpcSleep,
             const EnemyType Race,
             const Vector2 WorldPos,
             const Window& Screen,
             Background& World,
             const GameTexture& GameTextures,
             const GameAudio& Audio,
             Randomizer& RandomEngine,
             const float Scale)
    : Race{Race},
      Type{EnemyType::NPC},
      WorldPos{WorldPos},
      Screen{Screen},
      World{World},
      GameTextures{GameTextures},
      Audio{Audio},
      RandomEngine{RandomEngine},
      Scale{Scale}
{
    Sprites.emplace_back(NpcIdle);
    Sprites.emplace_back(NpcIdleTwo);
    Sprites.emplace_back(NpcWalk);
    Sprites.emplace_back(NpcMisc);
    Sprites.emplace_back(NpcSleep);

    CurrentSpriteIndex = static_cast<int>(NPC::IDLE);

    ActionIdleTime = static_cast<float>(RandomEngine.Randomize(RandomIdleTime));
    MoveXRange = RandomEngine.Randomize(RandomRange);
    MoveYRange = RandomEngine.Randomize(RandomRange);
    LeftOrRight = RandomEngine.Randomize(RandomLeftRight);
    ActionState = RandomEngine.Randomize(RandomActionState);
}

void Enemy::Tick(float DeltaTime, Props& Props, const Vector2 HeroWorldPos, const Vector2 HeroScreenPos, std::vector<Enemy>& Enemies, std::vector<Prop>& Trees)
{   
    UpdateScreenPos(HeroWorldPos);
            
    if (Visible) {
        if (Type != EnemyType::NPC) {
            if (Alive && Summoned) {
                SpriteTick(DeltaTime);
                CheckDirection();
                NeutralAction();
                TakeDamage();
                CheckAlive(DeltaTime);
            }
        }
        else {
            SpriteTick(DeltaTime);
            CheckDirection();
            NeutralAction();
        }

        if (Summoned) {
            CheckMovement(Props, HeroWorldPos, HeroScreenPos, Enemies, Trees);
        }
    }
    else {
        if (Type == EnemyType::BOSS || Type == EnemyType::FINALBOSS) {
            CheckBossSummon(HeroWorldPos);
        }
    }

    CheckSpawnChest(Props.Over);
}

void Enemy::Draw(const Vector2 HeroWorldPos)
{
    if (WithinScreen(HeroWorldPos)) {
        Visible = true;

        if (!OOB) {
            if (Type == EnemyType::NORMAL || Type == EnemyType::NPC || (Type == EnemyType::BOSS && Summoned) || (Type == EnemyType::FINALBOSS && Summoned)) {
                if (Hurting) {
                    DrawTexturePro(Sprites.at(CurrentSpriteIndex).Texture, Sprites.at(CurrentSpriteIndex).GetSourceRec(), Sprites.at(CurrentSpriteIndex).GetPosRec(ScreenPos,Scale), Vector2{},0.f, RED);
                }
                else {
                    DrawTexturePro(Sprites.at(CurrentSpriteIndex).Texture, Sprites.at(CurrentSpriteIndex).GetSourceRec(), Sprites.at(CurrentSpriteIndex).GetPosRec(ScreenPos,Scale), Vector2{},0.f, WHITE);
                }

                // Draw Ranged projectile
                if (Ranged && Attacking && !Dying && !Hurting) {
                    if (Type == EnemyType::NORMAL) {
                        DrawTexturePro(Sprites.at(ShootingSpriteIndex).Texture, Sprites.at(ShootingSpriteIndex).GetSourceRec(), Sprites.at(CurrentSpriteIndex).GetPosRec(UpdateProjectile(),Scale), Vector2{},0.f, WHITE);
                    }
                    else if (Type == EnemyType::BOSS || Type == EnemyType::FINALBOSS) {
                        DrawTexturePro(Sprites.at(ShootingSpriteIndex).Texture, Sprites.at(ShootingSpriteIndex).GetSourceRec(), Sprites.at(CurrentSpriteIndex).GetPosRec(UpdateMultiProjectile().at(0),Scale), Vector2{},0.f, WHITE);
                        DrawTexturePro(Sprites.at(ShootingSpriteIndex).Texture, Sprites.at(ShootingSpriteIndex).GetSourceRec(), Sprites.at(CurrentSpriteIndex).GetPosRec(UpdateMultiProjectile().at(1),Scale), Vector2{},0.f, WHITE);
                        DrawTexturePro(Sprites.at(ShootingSpriteIndex).Texture, Sprites.at(ShootingSpriteIndex).GetSourceRec(), Sprites.at(CurrentSpriteIndex).GetPosRec(UpdateMultiProjectile().at(2),Scale), Vector2{},0.f, WHITE);
                    }
                }
            }
        }
        // OOB is used for debugging purposes only
        else {
            DrawTexturePro(Sprites.at(CurrentSpriteIndex).Texture, Sprites.at(CurrentSpriteIndex).GetSourceRec(), Sprites.at(CurrentSpriteIndex).GetPosRec(ScreenPos,Scale), Vector2{},0.f, PURPLE);
        }

        if (Alive && Summoned && (Type != EnemyType::NPC)) {
            DrawHP();
        }
    }
    else {
        Visible = false;
    }
}

void Enemy::SpriteTick(float DeltaTime)
{
    for (auto& Sprite:Sprites) {   
        if (Type != EnemyType::NPC) {
            // Don't call Sprite::Tick() on the death sprite UNTIL Dying==true so that it will start from FrameX=0 when it does need to play
            if (&Sprite != &Sprites.at(4)) {
                Sprite.Tick(DeltaTime);
            }
            else {
                if (Dying) {
                    Sprite.Tick(DeltaTime);
                }
            }
        }
        else {
            Sprite.Tick(DeltaTime);
        }
    }
}

void Enemy::UpdateScreenPos(const Vector2 HeroWorldPos)
{
    ScreenPos = Vector2Subtract(WorldPos, HeroWorldPos);
}

void Enemy::CheckDirection()
{   
    // NPC only has left and right orientation sprites
    if (Type == EnemyType::NPC) {
        if (AIX < 0.f) {
            Face = Direction::LEFT;
        }
        if (AIX > 0.f) {
            Face = Direction::RIGHT;
        }
    }
    else {
        if (!Chasing) {
            if (AIY < 0.f) {
                Face = Direction::UP;
            }
            if (AIX < 0.f) {
                Face = Direction::LEFT;
            }
            if (AIY > 0.f) {
                Face = Direction::DOWN;
            }
            if (AIX > 0.f) {
                Face = Direction::RIGHT;
            }
        }
    }

    if (Type != EnemyType::NPC) {
        switch (Face)
        {
            case Direction::DOWN: 
                Sprites.at(CurrentSpriteIndex).FrameY = 0;
                break;
            case Direction::LEFT: 
                Sprites.at(CurrentSpriteIndex).FrameY = 1;
                break;
            case Direction::RIGHT:
                Sprites.at(CurrentSpriteIndex).FrameY = 2;
                break;
            case Direction::UP: 
                Sprites.at(CurrentSpriteIndex).FrameY = 3;
                break;
        }
    }
    else {
        switch (Face)
        {
            case Direction::DOWN: 
                Sprites.at(CurrentSpriteIndex).FrameY = 0;
                break;
            case Direction::LEFT: 
                Sprites.at(CurrentSpriteIndex).FrameY = 1;
                break;
            case Direction::RIGHT:
                Sprites.at(CurrentSpriteIndex).FrameY = 0;
                break;
            case Direction::UP: 
                Sprites.at(CurrentSpriteIndex).FrameY = 1;
                break;
        }
    }

    if (Ranged) {
        switch (Face)
        {
            case Direction::DOWN: 
                Sprites.at(ShootingSpriteIndex).FrameY = 0;
                break;
            case Direction::LEFT: 
                Sprites.at(ShootingSpriteIndex).FrameY = 1;
                break;
            case Direction::RIGHT:
                Sprites.at(ShootingSpriteIndex).FrameY = 2;
                break;
            case Direction::UP: 
                Sprites.at(ShootingSpriteIndex).FrameY = 3;
                break;
        }
    }
}

void Enemy::CheckMovement(Props& Props, const Vector2 HeroWorldPos, const Vector2 HeroScreenPos, std::vector<Enemy>& Enemies, std::vector<Prop>& Trees)
{
    PrevWorldPos = WorldPos;
    
    if (Race != EnemyType::MUSHROOM) {
        EnemyAI();
    }


    if (Type != EnemyType::NPC) {
        EnemyAggro(HeroScreenPos);
    }

    CheckOutOfBounds();

    if (Alive && World.GetArea() == Area::FOREST) {
        CheckCollision(Props.Under, HeroWorldPos, Enemies, Trees);
        CheckCollision(Props.Over, HeroWorldPos, Enemies, Trees);
    }
}

void Enemy::UndoMovement()
{
    WorldPos = PrevWorldPos;
}

void Enemy::CheckOutOfBounds()
{
    float CurrentSpriteWidth{static_cast<float>((Sprites.at(CurrentSpriteIndex).Texture.width/Sprites.at(CurrentSpriteIndex).MaxFramesX) * Scale)};
    float CurrentSpriteHeight{static_cast<float>((Sprites.at(CurrentSpriteIndex).Texture.height/Sprites.at(CurrentSpriteIndex).MaxFramesY) * Scale)};

    if (World.GetArea() == Area::FOREST) {
        if (WorldPos.x < 0.f ||
            WorldPos.y < 0.f ||
            WorldPos.x > World.GetForestMapSize().x - CurrentSpriteWidth ||
            WorldPos.y > World.GetForestMapSize().y - CurrentSpriteHeight)
        {
            UndoMovement();
        }
    }
    else if (World.GetArea() == Area::DUNGEON) {
        if (WorldPos.x < 64.f ||
            WorldPos.y < 128.f ||
            WorldPos.x > World.GetDungeonMapSize().x - CurrentSpriteWidth - 64.f ||
            WorldPos.y > World.GetDungeonMapSize().y - CurrentSpriteHeight - 54.f) 
        {
            UndoMovement();
        }
    }
}

void Enemy::NeutralAction()
{
    if (Chasing || Walking) {
        if (Type == EnemyType::NPC) {
            CurrentSpriteIndex = static_cast<int>(NPC::WALK);
        }
        else {
            CurrentSpriteIndex = static_cast<int>(Monster::WALK);
        }
    }
    else if (MiscAction && Type == EnemyType::NPC) {
        CurrentSpriteIndex = static_cast<int>(NPC::MISC);
    }
    else if (Sleeping && Type == EnemyType::NPC) {
        CurrentSpriteIndex = static_cast<int>(NPC::SLEEP);
    }
    else {
        if (Type == EnemyType::NPC) {
            if (IdleTwo) {
                CurrentSpriteIndex = static_cast<int>(NPC::IDLETWO);
            }
            else {
                CurrentSpriteIndex = static_cast<int>(NPC::IDLE);
            }
        }
        else {
            CurrentSpriteIndex = static_cast<int>(Monster::IDLE);
        }
    }
}

void Enemy::CheckCollision(std::vector<std::vector<Prop>>& Props, const Vector2 HeroWorldPos, std::vector<Enemy>& Enemies, std::vector<Prop>& Trees)
{
    // Crows should not be blocked by anything
    if (Race != EnemyType::CROW) {

        // Prop collision handling
        for (auto& PropType:Props) {
            for (auto& Prop:PropType) {
                if (Prop.HasCollision() && Prop.IsSpawned()) { 
                    // move away from props
                    Vector2 PropScreenPos{Vector2{Prop.GetCollisionRec(HeroWorldPos).x, Prop.GetCollisionRec(HeroWorldPos).y}}; // Grab the collision rectangle screen position
                    Vector2 RadiusAroundEnemy{5.f,5.f};
                    Vector2 ToTarget {Vector2Scale(Vector2Normalize(Vector2Subtract(Vector2Add(PropScreenPos, RadiusAroundEnemy), ScreenPos)), Speed)}; // Calculate the distance from Enemy to Prop
                    float AvoidProp{Vector2Length(Vector2Subtract(Vector2Add(PropScreenPos, RadiusAroundEnemy), ScreenPos))};
                    
                    if (AvoidProp <= MinRange && Prop.GetType() != PropType::GRASS) {
                        WorldPos = Vector2Subtract(WorldPos, ToTarget);
                    }

                    // activate grass animation
                    if (CheckCollisionRecs(this->GetCollisionRec(), Prop.GetCollisionRec(HeroWorldPos)) && Prop.GetType() == PropType::GRASS && Alive) {   
                        Prop.SetActive(true);
                    }
                }
            }
        }
        
        // Tree collision handling
        for (auto& Tree:Trees) {
            if (Tree.HasCollision() && Tree.IsSpawned()) {
                Vector2 TreeScreenPos{Vector2{Tree.GetCollisionRec(HeroWorldPos).x, Tree.GetCollisionRec(HeroWorldPos).y}}; // Grab the collision rectangle screen position
                Vector2 RadiusAroundEnemy{5.f,5.f};
                Vector2 ToTarget {Vector2Scale(Vector2Normalize(Vector2Subtract(Vector2Add(TreeScreenPos, RadiusAroundEnemy), ScreenPos)), Speed)}; // Calculate the distance from Enemy to Tree
                float AvoidTree{Vector2Length(Vector2Subtract(Vector2Add(TreeScreenPos, RadiusAroundEnemy), ScreenPos))};
                
                if (AvoidTree <= MinRange) {
                    WorldPos = Vector2Subtract(WorldPos, ToTarget);
                }
            }
        }
    }

    // Enemy collision handling
    for (auto& Enemy:Enemies) {
        if (this != &Enemy && (Type != EnemyType::BOSS) && (Enemy.GetType() != EnemyType::BOSS) && !Dying) {
            Vector2 RadiusAroundEnemy{5.f,5.f};
            Vector2 ToTarget{Vector2Scale(Vector2Normalize(Vector2Subtract(Vector2Add(Enemy.GetEnemyPos(), RadiusAroundEnemy), ScreenPos)), Speed)}; // Calculate the distance from this->Enemy to Enemy
            float AvoidEnemy{Vector2Length(Vector2Subtract(Vector2Add(Enemy.GetEnemyPos(), RadiusAroundEnemy), ScreenPos))};

            if (AvoidEnemy <= MinRange) {
                WorldPos = Vector2Subtract(WorldPos, ToTarget);
            }
        }
    }
}

void Enemy::CheckAttack()
{
    if (Attacking) {
        CurrentSpriteIndex = static_cast<int>(Monster::ATTACK);

        if (Ranged) {
            ShootingSpriteIndex = static_cast<int>(Monster::PROJECTILE);
        }
    }
    else {
        Trajectory = 1;
    }
}

Vector2 Enemy::UpdateProjectile()
{   
    if (Trajectory >= 190.f) {
        Trajectory = 1.f;
    }

    Vector2 ProjectilePath{ScreenPos}; 
    switch (Face)
    {
        case Direction::UP: 
            ProjectilePath.y -= Trajectory;
            break;
        case Direction::DOWN:
            ProjectilePath.y += Trajectory;
            break;
        case Direction::LEFT:
            ProjectilePath.x -= Trajectory;
            break;
        case Direction::RIGHT:
            ProjectilePath.x += Trajectory;
            break;
    }
    return ProjectilePath;
}

std::array<Vector2,3> Enemy::UpdateMultiProjectile()
{
    if (Trajectory >= 190.f) {
        Trajectory = 1.f;
    }
 
    std::array<Vector2,3> ProjectilePaths{Vector2{ScreenPos},Vector2{ScreenPos},Vector2{ScreenPos}};
 
    switch (Face)
    {
        case Direction::UP:
        {
            ProjectilePaths.at(0).x -= Trajectory;
            ProjectilePaths.at(0).y -= Trajectory;

            ProjectilePaths.at(1).x += 0.f;
            ProjectilePaths.at(1).y -= Trajectory;

            ProjectilePaths.at(2).x += Trajectory;
            ProjectilePaths.at(2).y -= Trajectory;
            break;
        } 
        case Direction::DOWN:
        {
            ProjectilePaths.at(0).x -= Trajectory;
            ProjectilePaths.at(0).y += Trajectory;

            ProjectilePaths.at(1).x += 0.f;
            ProjectilePaths.at(1).y += Trajectory;

            ProjectilePaths.at(2).x += Trajectory;
            ProjectilePaths.at(2).y += Trajectory;
            break;
        }
        case Direction::LEFT:
        {
            ProjectilePaths.at(0).x -= Trajectory;
            ProjectilePaths.at(0).y += Trajectory;

            ProjectilePaths.at(1).x -= Trajectory;
            ProjectilePaths.at(1).y += 0.f;

            ProjectilePaths.at(2).x -= Trajectory;
            ProjectilePaths.at(2).y -= Trajectory;
            break;
        }
        case Direction::RIGHT:
        {
            ProjectilePaths.at(0).x += Trajectory;
            ProjectilePaths.at(0).y -= Trajectory;

            ProjectilePaths.at(1).x += Trajectory;
            ProjectilePaths.at(1).y += 0.f;

            ProjectilePaths.at(2).x += Trajectory;
            ProjectilePaths.at(2).y += Trajectory;
            break;
        }
    }
    return ProjectilePaths;
}

void Enemy::TakeDamage()
{
    DamageTime += GetFrameTime();

    if (IsAttacked) {
        float UpdateTime {0.7f};
        float HurtUpdateTime{0.6f};
        
        // Knock enemy back a few units while hurt
        if (DamageTime <= HurtUpdateTime) {
            switch (Face) 
            {
                case Direction::UP:
                    WorldPos.y += 0.5f;
                    break;
                case Direction::DOWN:
                    WorldPos.y -= 0.5f;
                    break;
                case Direction::LEFT:
                    WorldPos.x += 0.5f;
                    break;
                case Direction::RIGHT:
                    WorldPos.x -= 0.5f;
                    break;
            }
        }

        // Make enemy unable to move
        Stopped = true;

        if (Health <= 0) {
            Invulnerable = true;
        }

        if (DamageTime <= HurtUpdateTime) {
            CurrentSpriteIndex = static_cast<int>(Monster::HURT);
            Hurting = true;
            DamageAudio();
            Trajectory = 1.f;
        }

        // This is where the enemy takes damage
        if (DamageTime >= UpdateTime) {
            if (Health > 0) {
                Health -= 1;
            }
            DamageTime = 0.f;
        }
    }
    else {
        Stopped = false;
        Hurting = false;
    }
}

void Enemy::CheckAlive(float DeltaTime) 
{
    if (Health <= 0) {
        DeathAudio();
        CurrentSpriteIndex = static_cast<int>(Monster::DEATH);
        // Amount of time needed for death animation to complete beginning to end
        float EndTime{1.35f};
        IsAttacked = false;
        Dying = true;
        Walking = false;
        Chasing = false;

        // Allow time for death animation to finish before setting alive=false which turns off SpriteTick()
        StopTime += DeltaTime;
        if (StopTime >= EndTime) {
            MonsterDeaths += 1;
            Alive = false;
            if (Type != EnemyType::BOSS) {
                MonsterCounter[Race] -= 1;
            }
            else {
                MonsterCounter[EnemyType::BOSS] -= 1;
            }
        }
    }
}

void Enemy::EnemyAI()
{
    // Randomize which direction enemy will move first
    if (!InitializedAI) {
        InitializeAI();
    } 

    if (!Chasing && !Blocked) {
        ActionTime += GetFrameTime();

        if (ActionTime >= ActionIdleTime) {
            ActionTime = 0.0f;
            IdleTwo = !IdleTwo;
            MiscAction = false;
            Sleeping = false;
            Walking = false;
            ActionState = RandomEngine.Randomize(RandomActionState);
        }

        if (Type == EnemyType::NPC) {
            if (ActionState >= 1 && ActionState <= 3) {
                Walking = false;
            }
            else if ((ActionState >= 4 && ActionState <= 6) && (ActionTime <= ActionIdleTime)) {
                CheckMovementAI();
                Walking = true;
                WorldPos.x += AIX;
                WorldPos.y += AIY;
            }
            else if (ActionState == 7 && (ActionTime <= ActionIdleTime)) {
                MiscAction = true;
            }
            else if ((ActionState >= 8 && ActionState <= 10) && (ActionTime <= ActionIdleTime)) {
                Sleeping = true;
            }
        }
        // all other enemies
        else {
            CheckMovementAI();  
            if (ActionTime <= ActionIdleTime/2) {
                Walking = true;
                WorldPos.x += AIX;
                WorldPos.y += AIY;
            }
            else {
                Walking = false;
            }
        }
    }
}

void Enemy::EnemyAggro(const Vector2 HeroScreenPos)
{
    // Calculate the distance from Enemy to Player
    Vector2 ToTarget {Vector2Scale(Vector2Normalize(Vector2Subtract(Vector2Add(HeroScreenPos, {50.f, 50.f}), ScreenPos)), Speed)}; 
    
    if (Alive && Summoned && !Stopped && !Invulnerable && !Blocked && !Dying) {
        Vector2 RadiusAroundEnemy{};

        if (Ranged) {
            RadiusAroundEnemy = {10.f,10.f}; 
            switch (Face)
            {
                case Direction::DOWN:
                {
                    MinRange = 80.f;
                    MaxRange = 260.f;
                    break;
                }
                case Direction::LEFT:
                {
                    MinRange = 140.f;
                    MaxRange = 200.f;
                    break;
                }
                case Direction::RIGHT:
                {
                    MinRange = 80.f;
                    MaxRange = 260.f;
                    break;
                }
                case Direction::UP:
                {
                    MinRange = 140.f;
                    MaxRange = 200.f;
                    break;
                }
            }
        }
        else {
            RadiusAroundEnemy = {50.f,50.f};
        }

        float Aggro{Vector2Length(Vector2Subtract(Vector2Add(HeroScreenPos, RadiusAroundEnemy), ScreenPos))};

        // Chasing: Only move enemy towards Player if within a certain range
        if (Aggro > MaxRange) {
            ToTarget = {0.f,0.f};
            Chasing = false;
            Attacking = false;
            // AIY = 0.f;
        }
        else if ((Aggro > MinRange) && (Aggro < MaxRange)) {
            WorldPos = Vector2Add(WorldPos, ToTarget);
            Chasing = true;
            Attacking = false;

            // Control what direction the enemy will face when chasing
            if (ToTarget.x >= 0.f && ToTarget.y <= 0.f) {
                if (std::abs(ToTarget.x) > std::abs(ToTarget.y)) {
                    Face = Direction::RIGHT;
                }
                else {
                    Face = Direction::UP;
                }
            }
            else if (ToTarget.x >= 0.f && ToTarget.y > 0.f)  {
                if (std::abs(ToTarget.x) > std::abs(ToTarget.y)) {
                    Face = Direction::RIGHT;
                }
                else {
                    Face = Direction::DOWN;
                }
            }
            else if (ToTarget.x <= 0.f && ToTarget.y <= 0.f)  {
                if (std::abs(ToTarget.x) > std::abs(ToTarget.y)) {
                    Face = Direction::LEFT;
                }
                else {
                    Face = Direction::UP;
                }
            }
            else if (ToTarget.x <= 0.f && ToTarget.y > 0.f)  {
                if (std::abs(ToTarget.x) > std::abs(ToTarget.y)) {
                    Face = Direction::LEFT;
                }
                else {
                    Face = Direction::DOWN;
                }
            }
        }
        else if (Aggro <= MinRange) {
            AttackAudio();
            Attacking = true;
            Trajectory += 1.3f;
        }

        CheckAttack();
    }
}

void Enemy::CheckMovementAI()
{
    Movement.x += AIX;
    Movement.y += AIY;

    if (Movement.x <= -MoveXRange || Movement.x >= MoveXRange) {
        AIX = -AIX;
    }
    if (Movement.y <= -MoveYRange || Movement.y >= MoveYRange) {
        AIY = -AIY;
    }  
}

void Enemy::InitializeAI()
{
    if (LeftOrRight <= 5) {
        AIX = -AIX;
        AIY = -AIY;
        IdleTwo = true;
    }
    InitializedAI = true;
}

void Enemy::DrawHP()
{
    float LifeBarScale{2.f};
    float SingleBarWidth{static_cast<float>(GameTextures.LifebarLeftEmpty.width) * LifeBarScale};
    float MaxBarWidth{SingleBarWidth * MaxHP};
    float CenterLifeBar {(MaxBarWidth - (Sprites.at(CurrentSpriteIndex).Texture.width/Sprites.at(CurrentSpriteIndex).MaxFramesX)*Scale ) / 2.f};
    Vector2 LifeBarPos{};                               // Where the lifebar is positioned
    Vector2 LifeBarPosAdd{SingleBarWidth, 0.f};         // spacing between each life 'bar'

    // update lifebarpos to center of enemy sprite
    LifeBarPos = Vector2Subtract(ScreenPos, Vector2{CenterLifeBar, 20.f});

    for (auto i = 1; i <= MaxHP; ++i) {
        if (i <= Health) {
            // far left of lifebar
            if (i == 1) {
                DrawTextureEx(GameTextures.LifebarLeftFilled, LifeBarPos, 0.f, LifeBarScale, WHITE);
            }
            // far right of lifebar
            else if (i == MaxHP) {
                DrawTextureEx(GameTextures.LifebarRightFilled, LifeBarPos, 0.f, LifeBarScale, WHITE);
            }
            // middle of lifebar
            else {
                DrawTextureEx(GameTextures.LifebarMiddleFilled, LifeBarPos, 0.f, LifeBarScale, WHITE);
            }
        }
        else {
            // far left of lifebar
            if (i == 1) {
                DrawTextureEx(GameTextures.LifebarLeftEmpty, LifeBarPos, 0.f, LifeBarScale, WHITE);
            }
            // far right of lifebar
            else if (i == MaxHP) {
                DrawTextureEx(GameTextures.LifebarRightEmpty, LifeBarPos, 0.f, 2.f, WHITE);
            }
            // middle of lifebar
            else {
                DrawTextureEx(GameTextures.LifebarMiddleEmpty, LifeBarPos, 0.f, 2.f, WHITE);
            }
        }
        // add spacing between each bar
        LifeBarPos = Vector2Add(LifeBarPos, LifeBarPosAdd);
    }
}

void Enemy::CheckBossSummon(const Vector2 HeroWorldPos)
{
    if (!WithinScreen(HeroWorldPos) && Type == EnemyType::BOSS && (MonsterCounter[BossSpawner] <= 0) && !Summoned) {
        Summoned = true;
    }
    else if (Type == EnemyType::FINALBOSS && (MonsterCounter[BossSpawner] <= 0) && !Summoned && World.GetArea() == Area::DUNGEON) {
        Summoned = true;
    }
}

void Enemy::CheckSpawnChest(std::vector<std::vector<Prop>>& Props)
{
    for (auto& OuterProp:Props) {
        for (auto& Prop:OuterProp) {
            if ((Type == EnemyType::BOSS || Type == EnemyType::FINALBOSS) && Prop.GetType() == PropType::TREASURE) {
                if ((Race == EnemyType::CREATURE && !Alive && Prop.GetItemName() == "Top Right Altar Piece") || 
                    (Race == EnemyType::IMP && !Alive && Prop.GetItemName() == "Top Altar Piece") || 
                    (Race == EnemyType::BEHOLDER && !Alive && Prop.GetItemName() == "Bottom Altar Piece") ||
                    (Race == EnemyType::NECROMANCER && !Alive && Prop.GetItemName() == "Bottom Right Altar Piece")) 
                {
                    Prop.SetSpawned(true);
                }
            }
        }
    }
}

Rectangle Enemy::GetCollisionRec()
{
    float CurrentSpriteWidth{static_cast<float>((Sprites.at(CurrentSpriteIndex).Texture.width/Sprites.at(CurrentSpriteIndex).MaxFramesX) * Scale)};
    float CurrentSpriteHeight{static_cast<float>((Sprites.at(CurrentSpriteIndex).Texture.height/Sprites.at(CurrentSpriteIndex).MaxFramesY) * Scale)};

    switch (Race)
    {   
        case EnemyType::FOX:
        {
            return Rectangle
            {
                ScreenPos.x + CurrentSpriteWidth * .31f,
                ScreenPos.y + CurrentSpriteHeight * .40f,
                CurrentSpriteWidth * .40f,
                CurrentSpriteHeight * .30f
            };
        }
        case EnemyType::CROW:
        {
            return Rectangle
            {
                ScreenPos.x + CurrentSpriteWidth * .31f,
                ScreenPos.y + CurrentSpriteHeight * .31f,
                CurrentSpriteWidth * .40f,
                CurrentSpriteHeight * .30f
            };
        }
        case EnemyType::SQUIRREL:
        {
            return Rectangle
            {
                ScreenPos.x + CurrentSpriteWidth * .35f,
                ScreenPos.y + CurrentSpriteHeight * .35f,
                CurrentSpriteWidth * .20f,
                CurrentSpriteHeight * .30f
            };
        }
        default:
        {
            return Rectangle 
            {
                ScreenPos.x + CurrentSpriteWidth * .31f,
                ScreenPos.y + CurrentSpriteHeight * .31f,
                CurrentSpriteWidth * .40f,
                CurrentSpriteHeight * .60f
            };
        }
    }
}

Rectangle Enemy::GetAttackRec()
{
    if (Ranged) {
        float ShootingSpriteWidth{static_cast<float>(Sprites.at(ShootingSpriteIndex).Texture.width/Sprites.at(ShootingSpriteIndex).MaxFramesX) * Scale};
        float ShootingSpriteHeight{static_cast<float>(Sprites.at(ShootingSpriteIndex).Texture.height/Sprites.at(ShootingSpriteIndex).MaxFramesY) * Scale};
        
        if (Type == EnemyType::NORMAL) {
            return Rectangle
            {
                UpdateProjectile().x + (ShootingSpriteWidth * 0.3f),
                UpdateProjectile().y + (ShootingSpriteHeight * 0.3f),
                ShootingSpriteWidth * 0.45f,
                ShootingSpriteHeight * 0.45f
            };
        }
        else {
            Rectangle MultiProjectileRec
            {
                UpdateMultiProjectile().at(AttackRecIndex).x + (ShootingSpriteWidth * 0.3f),
                UpdateMultiProjectile().at(AttackRecIndex).y + (ShootingSpriteHeight * 0.3f),
                ShootingSpriteWidth * 0.45f,
                ShootingSpriteHeight * 0.45f
            };
            ++AttackRecIndex;
            if (AttackRecIndex > 2) {
                AttackRecIndex = 0;
            }
            return MultiProjectileRec;
        }
    }
    else {
        float CurrentSpriteWidth{static_cast<float>(Sprites.at(CurrentSpriteIndex).Texture.width/Sprites.at(CurrentSpriteIndex).MaxFramesX) * Scale};
        float CurrentSpriteHeight{static_cast<float>(Sprites.at(CurrentSpriteIndex).Texture.height/Sprites.at(CurrentSpriteIndex).MaxFramesY) * Scale};

        switch (Face)
        {
            case Direction::UP:
                return Rectangle
                {
                    ScreenPos.x,
                    ScreenPos.y - (CurrentSpriteHeight * 0.4f),
                    CurrentSpriteWidth,
                    CurrentSpriteHeight
                }; 
            case Direction::DOWN:
                return Rectangle
                {
                    ScreenPos.x,
                    ScreenPos.y + (CurrentSpriteHeight * 0.4f),
                    CurrentSpriteWidth,
                    CurrentSpriteHeight
                }; 
            case Direction::LEFT: 
                return Rectangle
                {
                    ScreenPos.x - (CurrentSpriteWidth * 0.4f),
                    ScreenPos.y,
                    CurrentSpriteWidth,
                    CurrentSpriteHeight
                }; 
            case Direction::RIGHT:
                return Rectangle
                {
                    ScreenPos.x + (CurrentSpriteWidth * 0.4f),
                    ScreenPos.y,
                    CurrentSpriteWidth,
                    CurrentSpriteHeight
                }; 
            default:
                return Rectangle{};
        }
    }
}

bool Enemy::WithinScreen(const Vector2 HeroWorldPos)
{
    if (
        (WorldPos.x >= (HeroWorldPos.x + 615.f) - (GetScreenWidth()/2 + (Sprites.at(CurrentSpriteIndex).Texture.width * Scale))) && 
        (WorldPos.x <= (HeroWorldPos.x + 615.f) + (GetScreenWidth()/2 + (Sprites.at(CurrentSpriteIndex).Texture.width * Scale))) &&
        (WorldPos.y >= (HeroWorldPos.y + 335.f) - (GetScreenHeight()/2 + (Sprites.at(CurrentSpriteIndex).Texture.height * Scale))) && 
        (WorldPos.y <= (HeroWorldPos.y + 335.f) + (GetScreenHeight()/2 + (Sprites.at(CurrentSpriteIndex).Texture.height * Scale)))
       ) {
        return true;
    }
    else {
        return false;
    }
}

// ------------------------- Audio ---------------------------
void Enemy::WalkingAudio()
{
    SetSoundVolume(Audio.Walking, 0.10f);
    WalkingAudioTime += GetFrameTime();

    if (Walking && WalkingAudioTime >= 1.f/3.f) {
        PlaySoundMulti(Audio.Walking);
        WalkingAudioTime = 0.f;
    }
    else if (Chasing && WalkingAudioTime >= 1.f/3.5f) {
        PlaySoundMulti(Audio.Walking);
        WalkingAudioTime = 0.f;
    }
}

void Enemy::AttackAudio()
{
    AttackAudioTime += GetFrameTime();

    if (AttackAudioTime >= 0.6f) {
        if (Ranged) {
            SetSoundVolume(Audio.MonsterRangedAttack, 0.7f);
            PlaySoundMulti(Audio.MonsterRangedAttack);
        }
        else {
            SetSoundVolume(Audio.MonsterAttack, 0.7f);
            PlaySoundMulti(Audio.MonsterAttack);
        }
        AttackAudioTime = 0.f;
    }
}

void Enemy::DamageAudio()
{
    SetSoundVolume(Audio.ImpactHeavy, 0.7f);
    PlaySound(Audio.ImpactHeavy);
}

void Enemy::DeathAudio()
{
    if (Type == EnemyType::NORMAL) {
        PlaySound(Audio.MonsterDeath);
    }
    else if (Type == EnemyType::BOSS || Type == EnemyType::FINALBOSS) {
        PlaySound(Audio.BossDeath);
    }
}