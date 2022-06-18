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
             Randomizer& RandomEngine,
             const int Health,
             const float Scale) 
    : Race{Race},
      Type{Type},
      WorldPos{WorldPos},
      Screen{Screen},
      World{World},
      GameTextures{GameTextures},
      RandomEngine{RandomEngine},
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
    else {
        MonsterCounter[EnemyType::BOSS] += 1;
        Summoned = false;
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
             Randomizer& RandomEngine,
             const float Scale)
    : Race{Race},
      Type{EnemyType::NPC},
      WorldPos{WorldPos},
      Screen{Screen},
      World{World},
      GameTextures{GameTextures},
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
        if (Type == EnemyType::BOSS) {
            CheckBossSummon(HeroWorldPos);
        }
    }
}

void Enemy::Draw(const Vector2 HeroWorldPos)
{
    if (WithinScreen(HeroWorldPos)) {
        Visible = true;

        if (!OOB) {
            if (Type == EnemyType::NORMAL || Type == EnemyType::NPC || (Type == EnemyType::BOSS && Summoned)) {
                if (Hurting) {
                    DrawTexturePro(Sprites.at(CurrentSpriteIndex).Texture, Sprites.at(CurrentSpriteIndex).GetSourceRec(), Sprites.at(CurrentSpriteIndex).GetPosRec(ScreenPos,Scale), Vector2{},0.f, RED);
                }
                else {
                    DrawTexturePro(Sprites.at(CurrentSpriteIndex).Texture, Sprites.at(CurrentSpriteIndex).GetSourceRec(), Sprites.at(CurrentSpriteIndex).GetPosRec(ScreenPos,Scale), Vector2{},0.f, WHITE);
                }

                // Draw Ranged projectile
                if (Ranged && Attacking && !Dying && !Hurting) {
                    DrawTexturePro(Sprites.at(ShootingSpriteIndex).Texture, Sprites.at(ShootingSpriteIndex).GetSourceRec(), Sprites.at(CurrentSpriteIndex).GetPosRec(UpdateProjectile(),Scale), Vector2{},0.f, WHITE);
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
    
    EnemyAI();

    if (Type != EnemyType::NPC) {
        EnemyAggro(HeroScreenPos);
    }

    CheckOutOfBounds();

    if (Alive) {
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
    if (WorldPos.x < 0.f ||
        WorldPos.y < 0.f ||
        WorldPos.x > World.GetMapSize().x * World.GetScale() - (Sprites.at(CurrentSpriteIndex).Texture.width/Sprites.at(CurrentSpriteIndex).MaxFramesX)*Scale ||
        WorldPos.y > World.GetMapSize().y * World.GetScale() - (Sprites.at(CurrentSpriteIndex).Texture.height/Sprites.at(CurrentSpriteIndex).MaxFramesY)*Scale) 
    {
        UndoMovement();
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
        if (this != &Enemy && !Dying && Enemy.IsAlive() && !Enemy.IsDying()) {
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
        CurrentSpriteIndex = static_cast<int>(Monster::DEATH);
        // Amount of time needed for death animation to complete beginning to end
        float EndTime{1.35f};
        IsAttacked = false;
        Dying = true;

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
    Vector2 ToTarget {Vector2Scale(Vector2Normalize(Vector2Subtract(Vector2Add(HeroScreenPos,{50.f,50.f}), ScreenPos)), Speed)}; 
    
    if (Alive && Summoned && !Stopped && !Invulnerable && !Blocked && !Dying) {
        Vector2 RadiusAroundEnemy{};

        if (Ranged) {
            RadiusAroundEnemy = {10.f,10.f};
            MaxRange = 200.f;
            MinRange = 120.f;
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
                if (std::abs(ToTarget.x) > std::abs(ToTarget.y)) {Face = Direction::RIGHT;}
                else {Face = Direction::UP;}
            }
            else if (ToTarget.x >= 0.f && ToTarget.y > 0.f)  {
                if (std::abs(ToTarget.x) > std::abs(ToTarget.y)) {Face = Direction::RIGHT;}
                else {Face = Direction::DOWN;}
            }
            else if (ToTarget.x <= 0.f && ToTarget.y <= 0.f)  {
                if (std::abs(ToTarget.x) > std::abs(ToTarget.y)) {Face = Direction::LEFT;}
                else {Face = Direction::UP;}
            }
            else if (ToTarget.x <= 0.f && ToTarget.y > 0.f)  {
                if (std::abs(ToTarget.x) > std::abs(ToTarget.y)) {Face = Direction::LEFT;}
                else {Face = Direction::DOWN;}
            }
        }
        else if (Aggro <= MinRange) {
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
    if (!WithinScreen(HeroWorldPos) && (Type == EnemyType::BOSS) && (MonsterCounter[Race] <= 0) && !Summoned) {
        Summoned = true;
    }
}

Rectangle Enemy::GetCollisionRec()
{
    switch (Race)
    {   
        case EnemyType::FOX:
        {
            return Rectangle
            {
                ScreenPos.x + ((Sprites.at(CurrentSpriteIndex).Texture.width/Sprites.at(CurrentSpriteIndex).MaxFramesX) * Scale) * .31f,
                ScreenPos.y + ((Sprites.at(CurrentSpriteIndex).Texture.height/Sprites.at(CurrentSpriteIndex).MaxFramesY) * Scale) * .40f,
                ((Sprites.at(CurrentSpriteIndex).Texture.width/Sprites.at(CurrentSpriteIndex).MaxFramesX) * Scale) * .40f,
                ((Sprites.at(CurrentSpriteIndex).Texture.height/Sprites.at(CurrentSpriteIndex).MaxFramesY) * Scale) * .30f
            };
        }
        case EnemyType::CROW:
        {
            return Rectangle
            {
                ScreenPos.x + ((Sprites.at(CurrentSpriteIndex).Texture.width/Sprites.at(CurrentSpriteIndex).MaxFramesX) * Scale) * .31f,
                ScreenPos.y + ((Sprites.at(CurrentSpriteIndex).Texture.height/Sprites.at(CurrentSpriteIndex).MaxFramesY) * Scale) * .31f,
                ((Sprites.at(CurrentSpriteIndex).Texture.width/Sprites.at(CurrentSpriteIndex).MaxFramesX) * Scale) * .40f,
                ((Sprites.at(CurrentSpriteIndex).Texture.height/Sprites.at(CurrentSpriteIndex).MaxFramesY) * Scale) * .30f
            };
        }
        case EnemyType::SQUIRREL:
        {
            return Rectangle
            {
                ScreenPos.x + ((Sprites.at(CurrentSpriteIndex).Texture.width/Sprites.at(CurrentSpriteIndex).MaxFramesX) * Scale) * .35f,
                ScreenPos.y + ((Sprites.at(CurrentSpriteIndex).Texture.height/Sprites.at(CurrentSpriteIndex).MaxFramesY) * Scale) * .35f,
                ((Sprites.at(CurrentSpriteIndex).Texture.width/Sprites.at(CurrentSpriteIndex).MaxFramesX) * Scale) * .20f,
                ((Sprites.at(CurrentSpriteIndex).Texture.height/Sprites.at(CurrentSpriteIndex).MaxFramesY) * Scale) * .30f
            };
        }
        default:
        {
            return Rectangle 
            {
                ScreenPos.x + ((Sprites.at(CurrentSpriteIndex).Texture.width/Sprites.at(CurrentSpriteIndex).MaxFramesX) * Scale) * .31f,
                ScreenPos.y + ((Sprites.at(CurrentSpriteIndex).Texture.height/Sprites.at(CurrentSpriteIndex).MaxFramesY) * Scale) * .31f,
                ((Sprites.at(CurrentSpriteIndex).Texture.width/Sprites.at(CurrentSpriteIndex).MaxFramesX) * Scale) * .40f,
                ((Sprites.at(CurrentSpriteIndex).Texture.height/Sprites.at(CurrentSpriteIndex).MaxFramesY) * Scale) * .60f
            };
        }
    }
}

Rectangle Enemy::GetAttackRec()
{
    if (Ranged) {
        switch (Face) {
            case Direction::DOWN:
                return Rectangle
                {
                    UpdateProjectile().x + Sprites.at(ShootingSpriteIndex).Texture.width/Sprites.at(ShootingSpriteIndex).MaxFramesX/6.f,
                    UpdateProjectile().y + ((Sprites.at(ShootingSpriteIndex).Texture.height/Sprites.at(ShootingSpriteIndex).MaxFramesY)/2.f * Scale),
                    ((Sprites.at(ShootingSpriteIndex).Texture.width/Sprites.at(ShootingSpriteIndex).MaxFramesX/2.f) * Scale - (Sprites.at(ShootingSpriteIndex).Texture.width/Sprites.at(ShootingSpriteIndex).MaxFramesX)/1.5f) * Scale,
                    ((Sprites.at(ShootingSpriteIndex).Texture.height/Sprites.at(ShootingSpriteIndex).MaxFramesY/3.f) * Scale - (Sprites.at(ShootingSpriteIndex).Texture.height/Sprites.at(ShootingSpriteIndex).MaxFramesY)/1.5f)  * Scale
                };
            case Direction::LEFT:
                return Rectangle
                {
                    UpdateProjectile().x + (Sprites.at(ShootingSpriteIndex).Texture.height/Sprites.at(ShootingSpriteIndex).MaxFramesY/2.f),
                    UpdateProjectile().y + (Sprites.at(ShootingSpriteIndex).Texture.height/Sprites.at(ShootingSpriteIndex).MaxFramesY/6.f),
                    ((Sprites.at(ShootingSpriteIndex).Texture.width/Sprites.at(ShootingSpriteIndex).MaxFramesX/3.f) * Scale - (Sprites.at(ShootingSpriteIndex).Texture.width/Sprites.at(ShootingSpriteIndex).MaxFramesX)/1.5f) * Scale,
                    ((Sprites.at(ShootingSpriteIndex).Texture.height/Sprites.at(ShootingSpriteIndex).MaxFramesY/2.f) * Scale - (Sprites.at(ShootingSpriteIndex).Texture.height/Sprites.at(ShootingSpriteIndex).MaxFramesY)/1.5f)  * Scale
                };
            case Direction::RIGHT:
                return Rectangle
                {
                    UpdateProjectile().x + (Sprites.at(ShootingSpriteIndex).Texture.width/Sprites.at(ShootingSpriteIndex).MaxFramesX * Scale)/2.5f,
                    UpdateProjectile().y + (Sprites.at(ShootingSpriteIndex).Texture.height/Sprites.at(ShootingSpriteIndex).MaxFramesY/6.f),
                    ((Sprites.at(ShootingSpriteIndex).Texture.width/Sprites.at(ShootingSpriteIndex).MaxFramesX/3.f) * Scale - (Sprites.at(ShootingSpriteIndex).Texture.width/Sprites.at(ShootingSpriteIndex).MaxFramesX)/1.5f) * Scale,
                    ((Sprites.at(ShootingSpriteIndex).Texture.height/Sprites.at(ShootingSpriteIndex).MaxFramesY/2.f) * Scale - (Sprites.at(ShootingSpriteIndex).Texture.height/Sprites.at(ShootingSpriteIndex).MaxFramesY)/1.5f)  * Scale
                };
            case Direction::UP:
                return Rectangle
                {
                    UpdateProjectile().x + Sprites.at(ShootingSpriteIndex).Texture.width/Sprites.at(ShootingSpriteIndex).MaxFramesX/6.f,
                    UpdateProjectile().y + Sprites.at(ShootingSpriteIndex).Texture.height/Sprites.at(ShootingSpriteIndex).MaxFramesY/2.f,
                    ((Sprites.at(ShootingSpriteIndex).Texture.width/Sprites.at(ShootingSpriteIndex).MaxFramesX/2.f) * Scale - (Sprites.at(ShootingSpriteIndex).Texture.width/Sprites.at(ShootingSpriteIndex).MaxFramesX)/1.5f) * Scale,
                    ((Sprites.at(ShootingSpriteIndex).Texture.height/Sprites.at(ShootingSpriteIndex).MaxFramesY/3.f) * Scale - (Sprites.at(ShootingSpriteIndex).Texture.height/Sprites.at(ShootingSpriteIndex).MaxFramesY)/1.5f)  * Scale
                };
            default:
                return Rectangle
                {
                    
                };
        }
    }
    else {
        switch (Face)
        {
            case Direction::DOWN:
                return Rectangle
                {
                    ScreenPos.x + Sprites.at(CurrentSpriteIndex).Texture.width/Sprites.at(CurrentSpriteIndex).MaxFramesX/6.f,
                    ScreenPos.y + (Sprites.at(CurrentSpriteIndex).Texture.height/Sprites.at(CurrentSpriteIndex).MaxFramesY * Scale),
                    ((Sprites.at(CurrentSpriteIndex).Texture.width/Sprites.at(CurrentSpriteIndex).MaxFramesX/2.f) * Scale - (Sprites.at(CurrentSpriteIndex).Texture.width/Sprites.at(CurrentSpriteIndex).MaxFramesX)/1.5f) * Scale,
                    ((Sprites.at(CurrentSpriteIndex).Texture.height/Sprites.at(CurrentSpriteIndex).MaxFramesY/3.f) * Scale - (Sprites.at(CurrentSpriteIndex).Texture.height/Sprites.at(CurrentSpriteIndex).MaxFramesY)/1.5f)  * Scale
                }; 
            case Direction::LEFT: 
                return Rectangle
                {
                    ScreenPos.x - (Sprites.at(CurrentSpriteIndex).Texture.height/Sprites.at(CurrentSpriteIndex).MaxFramesY/1.8f),
                    ScreenPos.y + (Sprites.at(CurrentSpriteIndex).Texture.height/Sprites.at(CurrentSpriteIndex).MaxFramesY/6.f),
                    ((Sprites.at(CurrentSpriteIndex).Texture.width/Sprites.at(CurrentSpriteIndex).MaxFramesX/3.f) * Scale - (Sprites.at(CurrentSpriteIndex).Texture.width/Sprites.at(CurrentSpriteIndex).MaxFramesX)/1.5f) * Scale,
                    ((Sprites.at(CurrentSpriteIndex).Texture.height/Sprites.at(CurrentSpriteIndex).MaxFramesY/2.f) * Scale - (Sprites.at(CurrentSpriteIndex).Texture.height/Sprites.at(CurrentSpriteIndex).MaxFramesY)/1.5f)  * Scale
                }; 
            case Direction::RIGHT:
                return Rectangle
                {
                    ScreenPos.x + (Sprites.at(CurrentSpriteIndex).Texture.width/Sprites.at(CurrentSpriteIndex).MaxFramesX * Scale)/1.2f,
                    ScreenPos.y + (Sprites.at(CurrentSpriteIndex).Texture.height/Sprites.at(CurrentSpriteIndex).MaxFramesY/6.f),
                    ((Sprites.at(CurrentSpriteIndex).Texture.width/Sprites.at(CurrentSpriteIndex).MaxFramesX/3.f) * Scale - (Sprites.at(CurrentSpriteIndex).Texture.width/Sprites.at(CurrentSpriteIndex).MaxFramesX)/1.5f) * Scale,
                    ((Sprites.at(CurrentSpriteIndex).Texture.height/Sprites.at(CurrentSpriteIndex).MaxFramesY/2.f) * Scale - (Sprites.at(CurrentSpriteIndex).Texture.height/Sprites.at(CurrentSpriteIndex).MaxFramesY)/1.5f)  * Scale
                }; 
            case Direction::UP:
                return Rectangle
                {
                    ScreenPos.x + Sprites.at(CurrentSpriteIndex).Texture.width/Sprites.at(CurrentSpriteIndex).MaxFramesX/6.f,
                    ScreenPos.y - Sprites.at(CurrentSpriteIndex).Texture.height/Sprites.at(CurrentSpriteIndex).MaxFramesY/1.5f,
                    ((Sprites.at(CurrentSpriteIndex).Texture.width/Sprites.at(CurrentSpriteIndex).MaxFramesX/2.f) * Scale - (Sprites.at(CurrentSpriteIndex).Texture.width/Sprites.at(CurrentSpriteIndex).MaxFramesX)/1.5f) * Scale,
                    ((Sprites.at(CurrentSpriteIndex).Texture.height/Sprites.at(CurrentSpriteIndex).MaxFramesY/3.f) * Scale - (Sprites.at(CurrentSpriteIndex).Texture.height/Sprites.at(CurrentSpriteIndex).MaxFramesY)/1.5f)  * Scale
                };
            default:
                return Rectangle
                {
                    
                };
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
