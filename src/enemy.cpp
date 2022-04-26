#include "headers/enemy.hpp"

int Enemy::MonsterCount{};
int Enemy::MonsterDeaths{};

Enemy::Enemy(Sprite Idle,
             Sprite Walk,
             Sprite Attack,
             Sprite Hurt,
             Sprite Death,
             EnemyType Race,
             EnemyType Type,
             Vector2 WorldPos,
             Window* Screen,
             Background* World,
             int Health,
             float Scale)
    : Idle{Idle},
      Walk{Walk},
      Attack{Attack},
      Hurt{Hurt},
      Death{Death},
      Race{Race},
      Type{Type},
      WorldPos{WorldPos},
      Screen{Screen},
      World{World},
      Health{Health},
      MaxHP{Health},
      Scale{Scale}
{
    // Fill vector<Sprite*> with Sprite objects to easily loop through and call Sprite::Tick()
    Sprites.emplace_back(this->Idle);
    Sprites.emplace_back(this->Walk);
    Sprites.emplace_back(this->Attack);
    Sprites.emplace_back(this->Hurt);
    Sprites.emplace_back(this->Death);

    // Static variable to count how many enemies on the field
    MonsterCount += 1;

    // Generate RNG for current object used for randomizing AI movement
    std::random_device Seed;
    std::uniform_int_distribution<int> RandomRange{60, 80};
    std::uniform_int_distribution<int> RandomIdleTime{5, 9};
    std::uniform_int_distribution<int> RandomLeftRight{1, 10};
    std::mt19937 RNG{std::mt19937{Seed()}};

    MovementIdleTime = static_cast<float>(RandomIdleTime(RNG));
    MoveXRange = RandomRange(RNG);
    MoveYRange = RandomRange(RNG);
    LeftOrRight = RandomLeftRight(RNG);
}

Enemy::~Enemy()
{
    /* 
        UnloadTexture was being called somewhere and breaking multiple instances of Enemy objects.
        Disabling it fixed having more than one Enemy's textures breaking
    */
    
    // Unload all Textures when destructing Character
    // for (auto& Sprite:Sprites)
    // {
    //     UnloadTexture(Sprite.Texture);
    // }
}

void Enemy::Tick(float DeltaTime, Props& Props, Vector2 HeroWorldPos, Vector2 HeroScreenPos, std::vector<Enemy>& Enemies)
{
    if (Alive) {

        SpriteTick(DeltaTime);

        CheckDirection();
        
        WalkOrRun();

        TakingDamage();

        CheckAlive(DeltaTime);
    }

    CheckMovement(Props, HeroWorldPos, HeroScreenPos, Enemies);
}

// Draw character animation
void Enemy::Draw(Vector2 HeroWorldPos)
{
    if ((WorldPos.x >= (HeroWorldPos.x + 615) - (GetScreenWidth()/2 + (CurrentSprite->Texture.width * Scale))) && (WorldPos.x <= (HeroWorldPos.x + 615) + (GetScreenWidth()/2 + (CurrentSprite->Texture.width * Scale))) &&
        (WorldPos.y >= (HeroWorldPos.y + 335) - (GetScreenHeight()/2 + (CurrentSprite->Texture.height * Scale))) && (WorldPos.y <= (HeroWorldPos.y + 335) + (GetScreenHeight()/2 + (CurrentSprite->Texture.height * Scale))))
    {
        if (!OOB) {
            if (Hurting) {
                DrawTexturePro(CurrentSprite->Texture, CurrentSprite->GetSourceRec(), CurrentSprite->GetPosRec(EnemyPos,Scale), Vector2{},0.f, RED);
            }
            else {
                DrawTexturePro(CurrentSprite->Texture, CurrentSprite->GetSourceRec(), CurrentSprite->GetPosRec(EnemyPos,Scale), Vector2{},0.f, WHITE);
            }
        }
        else {
            DrawTexturePro(CurrentSprite->Texture, CurrentSprite->GetSourceRec(), CurrentSprite->GetPosRec(EnemyPos,Scale), Vector2{},0.f, PURPLE);
        }

        if (Alive) {
            DrawHP();
        }
    }
}

/* 
    Loop through Sprites vector and call Sprite::Tick() to maintain sprite animations current 'spot'.
    Halfway through walk animation -> Run will start from the halfway point. Etc.
*/
void Enemy::SpriteTick(float DeltaTime)
{
    for (auto& Sprite:Sprites)
    {
        // Call Sprite::Tick() on all sprites that are NOT death sprite
        if (&Sprite != &Sprites.at(4)) {
            Sprite.Tick(DeltaTime);
        }
        // Don't call Sprite::Tick() on the death sprite until Dying=true so that it will start from FrameX=0 when it does need to play
        else {
            if (Dying) {
                Sprite.Tick(DeltaTime);
            }
        }
    }
}

// Check which orientation the character is facing
void Enemy::CheckDirection()
{
    if (!Chasing) {
        if (AIY < 0) Face = Direction::UP;
        if (AIX < 0) Face = Direction::LEFT;
        if (AIY > 0) Face = Direction::DOWN;
        if (AIX > 0) Face = Direction::RIGHT;
    }

    switch (Face)
    {
        case Direction::DOWN: 
            CurrentSprite->FrameY = 0;
            break;
        case Direction::LEFT: 
            CurrentSprite->FrameY = 1;
            break;
        case Direction::RIGHT:
            CurrentSprite->FrameY = 2;
            break;
        case Direction::UP: 
            CurrentSprite->FrameY = 3;
            break;
    }
}

// Check for movement input
void Enemy::CheckMovement(Props& Props, Vector2 HeroWorldPos, Vector2 HeroScreenPos, std::vector<Enemy>& Enemies)
{
    PrevWorldPos = WorldPos;
    EnemyPos = Vector2Subtract(WorldPos, HeroWorldPos); // Where the Enemy is drawn on the screen
    
    // Enemy movement AI
    EnemyAI();

    // Chase Player
    EnemyAggro(HeroScreenPos);

    // Check if moving OOB (THE IF CHECK NEEDS FIXING)
    OutOfBounds();

    // Check for collision against player or props
    if (Alive) {
        CheckCollision(Props.Under, HeroWorldPos, Enemies);
        CheckCollision(Props.Over, HeroWorldPos, Enemies);
    }
}

// Undo movement if walking out-of-bounds or colliding
void Enemy::UndoMovement()
{
    WorldPos = PrevWorldPos;
}

// UndoMovement if enemy is moving out of bounds
void Enemy::OutOfBounds()
{
    if (WorldPos.x + EnemyPos.x + 615 < 0.f - (CurrentSprite->Texture.width/CurrentSprite->MaxFramesX)/2.f ||
        WorldPos.y + EnemyPos.y + 335 < 0.f - (CurrentSprite->Texture.height/CurrentSprite->MaxFramesY)/2.f ||
        WorldPos.x + (Screen->x - EnemyPos.x + 615) > World->GetMap().width * World->GetScale() + (CurrentSprite->Texture.width/CurrentSprite->MaxFramesX)/2.f ||
        WorldPos.y + (Screen->y - EnemyPos.y + 335) > World->GetMap().height * World->GetScale() + (CurrentSprite->Texture.height/CurrentSprite->MaxFramesY)/2.f) 
    {
        // UndoMovement();
        OOB = true;
    }
    else {
        OOB = false;
    }
}

// Check if Enemy is moving and change sprites if needed
void Enemy::WalkOrRun()
{
    if (IsKeyDown(KEY_LEFT_SHIFT))
    {
        Speed = 1.6f;
    }
    else 
    {
        Speed = 1.4f;

    }

    if (Chasing || Walking) 
    {
        CurrentSprite = &Sprites.at(1);
        // CurrentSprite = Walk;
    }
    else 
    {
        CurrentSprite = &Sprites.at(0);
        // CurrentSprite = Idle;
    }
}

// Check if colliding with props
void Enemy::CheckCollision(std::vector<std::vector<Prop>>* Props, Vector2 HeroWorldPos, std::vector<Enemy>& Enemies)
{
    // Prop collision handling
    for (auto& PropType:*Props) {
        for (auto& Prop:PropType) {
            if (Prop.HasCollision()) {
                
                /*
                    Undo movement wasn't working so implemented a reverse aggro logic. 
                    When enemy gets within min aggro range of an obstacle it 'runs' away from it.
                    Therefore it will never collide with a prop.
                */
                Vector2 PropScreenPos{Vector2Subtract(Prop.GetWorldPos(), HeroWorldPos)};
                Vector2 RadiusAroundEnemy{5,5};
                Vector2 ToTarget {Vector2Scale(Vector2Normalize(Vector2Subtract(Vector2Add(PropScreenPos, RadiusAroundEnemy), EnemyPos)), Speed)}; // Calculate the distance from Enemy to Prop
                float AvoidProp{Vector2Length(Vector2Subtract(Vector2Add(PropScreenPos, RadiusAroundEnemy), EnemyPos))};

                // move away from moveable objects 
                if (Prop.IsMoveable()) {
                    if (Prop.GetType() == PropType::BOULDER) {
                        if (AvoidProp <= MinRange) {
                            WorldPos = Vector2Subtract(WorldPos, ToTarget);
                        }
                    }
                }
                // move away from all other objects
                else if (Prop.IsVisible()) {
                    if (AvoidProp <= MinRange) {
                        WorldPos = Vector2Subtract(WorldPos, ToTarget);
                    }
                }

                // check physical collision
                if (CheckCollisionRecs(this->GetCollisionRec(), Prop.GetCollisionRec(HeroWorldPos))) {   

                    // activate grass animation
                    if (Prop.IsMoveable()) {
                        if (Prop.GetType() == PropType::GRASS && Alive) {
                            Prop.SetActive(true);
                        }
                    }
                }
            }
        }
    }

    // Enemy collision handling
    for (auto& Enemy:Enemies) {
        if (this != &Enemy) {

            /*
                When enemy gets within min aggro range of another enemy it 'runs' away from it.
                Therefore it will never collide with a Enemy.
            */
            Vector2 RadiusAroundEnemy{5,5};
            Vector2 ToTarget {Vector2Scale(Vector2Normalize(Vector2Subtract(Vector2Add(Enemy.GetEnemyPos(), RadiusAroundEnemy), EnemyPos)), Speed)}; // Calculate the distance from this->Enemy to Enemy
            float AvoidEnemy{Vector2Length(Vector2Subtract(Vector2Add(Enemy.GetEnemyPos(), RadiusAroundEnemy), EnemyPos))};

            if ((AvoidEnemy <= MinRange) && !Dying) {
                WorldPos = Vector2Subtract(WorldPos, ToTarget);
            }
        }
    }
}

// Manage attack sprites
void Enemy::CheckAttack()
{
    if (Attacking)
    {
        CurrentSprite = &Sprites.at(2);
        // CurrentSprite = Attack;
    }
}

// Handle enemy taking damage
void Enemy::TakingDamage()
{
    DamageTime += GetFrameTime();
    float UpdateTime {1.3f/1.f};
    float HurtUpdateTime{0.6f};

    // if (PreviousSprite != &Hurt) {PreviousSprite = CurrentSprite;}

    if (IsAttacked) {
        
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

        // Make invulnerable when HP <= 0
        if (Health <= 0) {
            Invulnerable = true;
        }

        // Time between hurt animation showing
        if (DamageTime <= HurtUpdateTime) {
            CurrentSprite = &Hurt;
            Hurting = true;
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
    // else {
    //     CurrentSprite = PreviousSprite;
    // }
}

// Handle death animation
void Enemy::CheckAlive(float DeltaTime) 
{
    // Amount of time needed for death animation to complete beginning to end
    float EndTime{1.35f};

    if (Health <= 0) {
        // Set to death sprite
        CurrentSprite = &Sprites.at(4);
        
        // Turn off any TakingDamage() functionality
        IsAttacked = false;

        /* 
            Dying=true will trigger some final Enemy::Tick() functionality.
            Alive=false will turn off majority of Enemy::Tick().
        */
        Dying = true;

        // Allow time for death animation to finish before setting alive=false which turns off SpriteTick()
        StopTime += DeltaTime;
        if (StopTime >= EndTime) {
            MonsterDeaths += 1;
            Alive = false;
        }
    }
}

// Handle enemy movement AI
void Enemy::EnemyAI()
{
    // Randomize which direction enemy will move first
    if (Intro && LeftOrRight <= 5) {
        AIX = -AIX;
        AIY = -AIY;
        Intro = false;
    }

    if (!Chasing && !Blocked) {
        Movement.x += AIX;
        Movement.y += AIY;

        WalkingTime += GetFrameTime();

        if (WalkingTime <= MovementIdleTime/2) {
            Walking = true;
            WorldPos.x += AIX;
            WorldPos.y += AIY;
        }
        else if (WalkingTime >= MovementIdleTime) {
            WalkingTime = 0.0f;
        }
        else {
            Walking = false;
        }

        if (Movement.x <= -MoveXRange || Movement.x >= MoveXRange) {
            AIX = -AIX;
        }
        if (Movement.y <= -MoveYRange || Movement.y >= MoveYRange) {
            AIY = -AIY;
        }
    }
}

// Handle enemy aggro range and chasing
void Enemy::EnemyAggro(Vector2 HeroScreenPos)
{
    // Calculate the distance from Enemy to Player
    Vector2 ToTarget {Vector2Scale(Vector2Normalize(Vector2Subtract(Vector2Add(HeroScreenPos,{50,50}), EnemyPos)), Speed)}; 
    
    if (!Stopped && Alive && !Invulnerable && !Blocked) {
        Vector2 RadiusAroundEnemy{50,50};
        float Aggro{Vector2Length(Vector2Subtract(Vector2Add(HeroScreenPos, RadiusAroundEnemy), EnemyPos))};

        // Only move enemy towards Player if within a certain range
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
            if (ToTarget.x >= 0 && ToTarget.y <= 0) {
                if (std::abs(ToTarget.x) > std::abs(ToTarget.y)) {Face = Direction::RIGHT;}
                else {Face = Direction::UP;}
            }
            else if (ToTarget.x >= 0 && ToTarget.y > 0)  {
                if (std::abs(ToTarget.x) > std::abs(ToTarget.y)) {Face = Direction::RIGHT;}
                else {Face = Direction::DOWN;}
            }
            else if (ToTarget.x <= 0 && ToTarget.y <= 0)  {
                if (std::abs(ToTarget.x) > std::abs(ToTarget.y)) {Face = Direction::LEFT;}
                else {Face = Direction::UP;}
            }
            else if (ToTarget.x <= 0 && ToTarget.y > 0)  {
                if (std::abs(ToTarget.x) > std::abs(ToTarget.y)) {Face = Direction::LEFT;}
                else {Face = Direction::DOWN;}
            }
        }
        else if (Aggro <= MinRange) {
            Attacking = true;
        }

        CheckAttack();
    }
}

// Draw enemy lifebar
void Enemy::DrawHP()
{
    float LifeBarScale{2.f};
    float SingleBarWidth{static_cast<float>(LifeBarLeft_Empty.width) * LifeBarScale};
    float MaxBarWidth{SingleBarWidth * MaxHP};
    float CenterLifeBar {(MaxBarWidth - (CurrentSprite->Texture.width/CurrentSprite->MaxFramesX)*Scale ) / 2.f};
    Vector2 LifeBarPos{};                           // Where the lifebar is positioned
    Vector2 LifeBarPosAdd{SingleBarWidth, 0};       // spacing between each life 'bar'

    // update lifebarpos to center of enemy sprite
    LifeBarPos = Vector2Subtract(EnemyPos, Vector2{CenterLifeBar, 20});

    for (auto i = 1; i <= MaxHP; ++i) {
        if (i <= Health) {
            // far left of lifebar
            if (i == 1) {
                DrawTextureEx(LifeBarLeft_Full, LifeBarPos, 0.f, LifeBarScale, WHITE);
            }
            // far right of lifebar
            else if (i == MaxHP) {
                DrawTextureEx(LifeBarRight_Full, LifeBarPos, 0.f, LifeBarScale, WHITE);
            }
            // middle of lifebar
            else {
                DrawTextureEx(LifeBarMiddle_Full, LifeBarPos, 0.f, LifeBarScale, WHITE);
            }
        }
        else {
            // far left of lifebar
            if (i == 1) {
                DrawTextureEx(LifeBarLeft_Empty, LifeBarPos, 0.f, LifeBarScale, WHITE);
            }
            // far right of lifebar
            else if (i == MaxHP) {
                DrawTextureEx(LifeBarRight_Empty, LifeBarPos, 0.f, 2.f, WHITE);
            }
            // middle of lifebar
            else {
                DrawTextureEx(LifeBarMiddle_Empty, LifeBarPos, 0.f, 2.f, WHITE);
            }
        }

        // add spacing between each bar
        LifeBarPos = Vector2Add(LifeBarPos, LifeBarPosAdd);
    }
}

// Return enemy physical collision dimensions
Rectangle Enemy::GetCollisionRec()
{
    return Rectangle 
    {
        EnemyPos.x + ((CurrentSprite->Texture.width/CurrentSprite->MaxFramesX) * Scale) * .31f,
        EnemyPos.y + ((CurrentSprite->Texture.height/CurrentSprite->MaxFramesY) * Scale) * .31f,
        ((CurrentSprite->Texture.width/CurrentSprite->MaxFramesX) * Scale) * .40f,
        ((CurrentSprite->Texture.height/CurrentSprite->MaxFramesY) * Scale) * .60f
    };
}

// Return enemy attack collision dimensions
Rectangle Enemy::GetAttackRec()
{
    switch (Face)
    {
        case Direction::DOWN:
            return Rectangle
            {
                EnemyPos.x + CurrentSprite->Texture.width/CurrentSprite->MaxFramesX/6.f,
                EnemyPos.y + (CurrentSprite->Texture.height/CurrentSprite->MaxFramesY * Scale),
                ((CurrentSprite->Texture.width/CurrentSprite->MaxFramesX/2.f) * Scale - (CurrentSprite->Texture.width/CurrentSprite->MaxFramesX)/1.5f) * Scale,
                ((CurrentSprite->Texture.height/CurrentSprite->MaxFramesY/3.f) * Scale - (CurrentSprite->Texture.height/CurrentSprite->MaxFramesY)/1.5f)  * Scale
            }; 
        case Direction::LEFT: 
            return Rectangle
            {
                EnemyPos.x - (CurrentSprite->Texture.height/CurrentSprite->MaxFramesY/1.8f),
                EnemyPos.y + (CurrentSprite->Texture.height/CurrentSprite->MaxFramesY/6.f),
                ((CurrentSprite->Texture.width/CurrentSprite->MaxFramesX/3.f) * Scale - (CurrentSprite->Texture.width/CurrentSprite->MaxFramesX)/1.5f) * Scale,
                ((CurrentSprite->Texture.height/CurrentSprite->MaxFramesY/2.f) * Scale - (CurrentSprite->Texture.height/CurrentSprite->MaxFramesY)/1.5f)  * Scale
            }; 
        case Direction::RIGHT:
            return Rectangle
            {
                EnemyPos.x + (CurrentSprite->Texture.width/CurrentSprite->MaxFramesX * Scale)/1.2f,
                EnemyPos.y + (CurrentSprite->Texture.height/CurrentSprite->MaxFramesY/6.f),
                ((CurrentSprite->Texture.width/CurrentSprite->MaxFramesX/3.f) * Scale - (CurrentSprite->Texture.width/CurrentSprite->MaxFramesX)/1.5f) * Scale,
                ((CurrentSprite->Texture.height/CurrentSprite->MaxFramesY/2.f) * Scale - (CurrentSprite->Texture.height/CurrentSprite->MaxFramesY)/1.5f)  * Scale
            }; 
        case Direction::UP:
            return Rectangle
            {
                EnemyPos.x + CurrentSprite->Texture.width/CurrentSprite->MaxFramesX/6.f,
                EnemyPos.y - CurrentSprite->Texture.height/CurrentSprite->MaxFramesY/1.5f,
                ((CurrentSprite->Texture.width/CurrentSprite->MaxFramesX/2.f) * Scale - (CurrentSprite->Texture.width/CurrentSprite->MaxFramesX)/1.5f) * Scale,
                ((CurrentSprite->Texture.height/CurrentSprite->MaxFramesY/3.f) * Scale - (CurrentSprite->Texture.height/CurrentSprite->MaxFramesY)/1.5f)  * Scale
            };
        default:
            return Rectangle
            {
                
            };
    }
}

// -----------------------------------------

Enemies::Enemies(std::vector<Enemy>* Monsters) 
    : Monsters{Monsters}
{

}
