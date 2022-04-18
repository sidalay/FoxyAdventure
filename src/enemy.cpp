#include "headers/enemy.hpp"

int Enemy::MonsterCount{};
int Enemy::MonsterDeaths{};

Enemy::Enemy(Sprite Idle,
             Sprite Walk,
             Sprite Attack,
             Sprite Hurt,
             Sprite Death,
             Vector2 WorldPos,
             Window* Screen,
             Background* World)
    : Idle{Idle},
      Walk{Walk},
      Attack{Attack},
      Hurt{Hurt},
      Death{Death},
      WorldPos{WorldPos},
      Screen{Screen},
      World{World}
{
    // Fill vector<Sprite*> with Sprite objects to easily loop through and call Sprite::Tick()
    Sprites.emplace_back(&(this->Idle));
    Sprites.emplace_back(&(this->Walk));
    Sprites.emplace_back(&(this->Attack));
    Sprites.emplace_back(&(this->Hurt));
    Sprites.emplace_back(&(this->Death));

    // Static variable to count how many enemies on the field
    MonsterCount += 1;
}

Enemy::~Enemy()
{
    // Unload all Textures when destructing Character
    for (auto& Sprite:Sprites)
    {
        UnloadTexture(Sprite->Texture);
    }
}

void Enemy::Tick(float DeltaTime, Props& Props, Vector2 HeroWorldPos, Vector2 HeroScreenPos)
{
    if (Alive) {

        SpriteTick(DeltaTime);

        CheckDirection();
        
        WalkOrRun();

        CheckAttack();

        TakingDamage();

        CheckAlive();
    }

    CheckMovement(Props, HeroWorldPos, HeroScreenPos);
}

// Draw character animation
void Enemy::Draw(Vector2 HeroWorldPos)
{
    if ((WorldPos.x >= (HeroWorldPos.x + 615) - (GetScreenWidth()/2 + (CurrentSprite->Texture.width * Scale))) && (WorldPos.x <= (HeroWorldPos.x + 615) + (GetScreenWidth()/2 + (CurrentSprite->Texture.width * Scale))) &&
        (WorldPos.y >= (HeroWorldPos.y + 335) - (GetScreenHeight()/2 + (CurrentSprite->Texture.height * Scale))) && (WorldPos.y <= (HeroWorldPos.y + 335) + (GetScreenHeight()/2 + (CurrentSprite->Texture.height * Scale))))
    {
        if (CurrentSprite == &Hurt) {
            DrawTexturePro(CurrentSprite->Texture, CurrentSprite->GetSourceRec(), CurrentSprite->GetPosRec(EnemyPos,Scale), Vector2{},0.f, RED);
        }
        else {
            DrawTexturePro(CurrentSprite->Texture, CurrentSprite->GetSourceRec(), CurrentSprite->GetPosRec(EnemyPos,Scale), Vector2{},0.f, WHITE);
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
        Sprite->Tick(DeltaTime);
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
void Enemy::CheckMovement(Props& Props, Vector2 HeroWorldPos, Vector2 HeroScreenPos)
{
    PrevWorldPos = WorldPos;
    EnemyPos = Vector2Subtract(WorldPos, HeroWorldPos); // Where the Enemy is drawn on the screen
    Vector2 ToTarget {Vector2Scale(Vector2Normalize(Vector2Subtract(Vector2Add(HeroScreenPos,{50,50}), EnemyPos)), Speed)}; // Calculate the distance from Enemy to Player
    
    // Enemy movement AI
    if (!Chasing) {
        Movement.x += AIX;
        Movement.y += AIY;

        float IdleTime{8.f};
        WalkingTime += GetFrameTime();

        if (WalkingTime <= IdleTime/2) {
            Walking = true;
            WorldPos.x += AIX;
            WorldPos.y += AIY;
        }
        else if (WalkingTime >= IdleTime) {
            WalkingTime = 0.0f;
        }
        else {
            Walking = false;
        }

        if (Movement.x >= 80 || Movement.x <= -80) {
            AIX = -AIX;
        }
        if (Movement.y >= 60 || Movement.y <= -60) {
            AIY = -AIY;
        }
    }

    // Chase Player
    if (!Stopped && Alive && !Invulnerable) {
        Vector2 RadiusAroundEnemy{50,50};
        float Aggro{Vector2Length(Vector2Subtract(Vector2Add(HeroScreenPos, RadiusAroundEnemy), EnemyPos))};

        // Only move enemy towards Player if within a certain range
        if (Aggro > MaxRange) {
            ToTarget = {0.f,0.f};
            Chasing = false;
            // AIY = 0.f;
        }
        else if ((Aggro > MinRange) && (Aggro < MaxRange)) {
            WorldPos = Vector2Add(WorldPos, ToTarget);
            Chasing = true;

            // Control what direction the enemy will face when chasing
            if ((ToTarget.x >= 0 && ToTarget.y < 0) || (ToTarget.x >= 0 && ToTarget.y == 0)) {
                if (std::abs(ToTarget.x) > std::abs(ToTarget.y)) {Face = Direction::RIGHT;}
                else {Face = Direction::UP;}
            }
            else if (ToTarget.x >= 0 && ToTarget.y > 0)  {
                if (std::abs(ToTarget.x) > std::abs(ToTarget.y)) {Face = Direction::RIGHT;}
                else {Face = Direction::DOWN;}
            }
            else if ((ToTarget.x <= 0 && ToTarget.y < 0) || (ToTarget.x <= 0 && ToTarget.y == 0))  {
                if (std::abs(ToTarget.x) > std::abs(ToTarget.y)) {Face = Direction::LEFT;}
                else {Face = Direction::UP;}
            }
            else if (ToTarget.x <= 0 && ToTarget.y > 0)  {
                if (std::abs(ToTarget.x) > std::abs(ToTarget.y)) {Face = Direction::LEFT;}
                else {Face = Direction::DOWN;}
            }

            // Make enemy face up / down depending on what direction its chasing
            // if (ToTarget.y > 0) {
            //     AIY = 1.f;
            // }
            // else if (ToTarget.y < 0) {
            //     AIY = -1.f;
            // }
        }
    }

    // PrevWorldPos = WorldPos;

    // Undo Movement if walking out-of-bounds
    if (WorldPos.x + EnemyPos.x < 0.f - (CurrentSprite->Texture.width/CurrentSprite->MaxFramesX)/2.f ||
        WorldPos.y + EnemyPos.y < 0.f - (CurrentSprite->Texture.height/CurrentSprite->MaxFramesY)/2.f ||
        WorldPos.x + (Screen->x - EnemyPos.x) > World->GetMap().width * World->GetScale() + (CurrentSprite->Texture.width/CurrentSprite->MaxFramesX)/2.f ||
        WorldPos.y + (Screen->y - EnemyPos.y) > World->GetMap().height * World->GetScale() + (CurrentSprite->Texture.height/CurrentSprite->MaxFramesY)/2.f)
    {
        UndoMovement();
    }

    CheckCollision(Props.Under, HeroWorldPos);
    CheckCollision(Props.Over, HeroWorldPos);
}

// Undo movement if walking out-of-bounds or colliding
void Enemy::UndoMovement()
{
    WorldPos = PrevWorldPos;
    WorldPos.x += -0.1;
    WorldPos.y += 0.1; 
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
        CurrentSprite = Sprites.at(1);
        // CurrentSprite = &Walk;
    }
    else 
    {
        CurrentSprite = Sprites.at(0);
        // CurrentSprite = &Idle;
    }
}

// Check if colliding with props
void Enemy::CheckCollision(std::vector<std::vector<Prop>>* Props, Vector2 HeroWorldPos)
{
    for (auto& PropType:*Props) {
        for (auto& Prop:PropType) {
            if (Prop.HasCollision()) {
                
                // check physical collision
                if (CheckCollisionRecs(GetCollisionRec(), Prop.GetCollisionRec(HeroWorldPos))) {   

                    // manage pushable props
                    if (Prop.IsMoveable()) {
                        if (Prop.GetType() == PropType::BOULDER) {
                            UndoMovement();
                        }
                        if (Prop.GetType() == PropType::GRASS && Alive) {
                            Prop.SetActive(true);
                        }
                    }
                    // if not pushable, block movement   
                    else {
                        if (Prop.IsVisible()) {
                            UndoMovement();
                        }
                    }
                }
            }
        }
    }
}

// Manage attack sprites
void Enemy::CheckAttack()
{
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) || IsKeyDown(KEY_SPACE))
    {
        Attacking = true;
    }
    else
    {
        Attacking = false;
    }

    if (Attacking)
    {
        CurrentSprite = Sprites.at(2);
        // CurrentSprite = &Attack;
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
        // Make enemy unable to move
        Stopped = true;

        // Make invulnerable when HP <= 0
        if (Health <= 0) {
            Invulnerable = true;
        }

        // Time between hurt animation showing
        if (DamageTime <= HurtUpdateTime) {
            CurrentSprite = &Hurt;
        }

        // This is where the enemy takes damage
        if (DamageTime >= UpdateTime) {
            if (Health > 0) {
                Health -= 2;
            }
            DamageTime = 0.f;
        }
    }
    else {
        Stopped = false;
    }
    // else {
    //     CurrentSprite = PreviousSprite;
    // }
}

// Handle death animation
void Enemy::CheckAlive() 
{
    float EndTime{3.5f/8.0f};

    if (Health <= 0) {

        if (!Dead) {
            Dead = true;
            MonsterDeaths += 1;
        }

        IsAttacked = false;

        // Set to death sprite
        CurrentSprite = Sprites.at(4);
        // CurrentSprite = &Death;

        StopTime += GetFrameTime();
        
        if (StopTime >= EndTime) {
            Alive = false;
        }
    }
}

// Return character collision dimensions
Rectangle Enemy::GetCollisionRec()
{
    return Rectangle 
    {
        EnemyPos.x + CurrentSprite->Texture.width/CurrentSprite->MaxFramesX/2.f,
        EnemyPos.y + CurrentSprite->Texture.height/CurrentSprite->MaxFramesY/2.f,
        (CurrentSprite->Texture.width/CurrentSprite->MaxFramesX) * Scale - ((CurrentSprite->Texture.width/CurrentSprite->MaxFramesX) * Scale) * .30f,
        (CurrentSprite->Texture.height/CurrentSprite->MaxFramesY) * Scale - ((CurrentSprite->Texture.height/CurrentSprite->MaxFramesY) * Scale) * .25f
    };
}


// -----------------------------------------

Enemies::Enemies(std::vector<Enemy>* Monsters) 
    : Monsters{Monsters}
{

}

/* 
    TODO: 
        - figure out orientation when chasing   // FINISHED
        - Add min range to enemy aggro          // FINISHED
        - fix undo movement
        - bounce back when being attacked
        - implement attack
*/