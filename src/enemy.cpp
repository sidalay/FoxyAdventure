#include "headers/enemy.hpp"

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
    SpriteTick(DeltaTime);

    CheckDirection();

    CheckMovement(Props, HeroWorldPos, HeroScreenPos);
    
    WalkOrRun();

    CheckAttack();
}

// Draw character animation
void Enemy::Draw(Vector2 HeroWorldPos)
{
    if ((WorldPos.x >= (HeroWorldPos.x + 615) - (GetScreenWidth()/2 + (CurrentSprite->Texture.width * Scale))) && (WorldPos.x <= (HeroWorldPos.x + 615) + (GetScreenWidth()/2 + (CurrentSprite->Texture.width * Scale))) &&
       (WorldPos.y >= (HeroWorldPos.y + 335) - (GetScreenHeight()/2 + (CurrentSprite->Texture.height * Scale))) && (WorldPos.y <= (HeroWorldPos.y + 335) + (GetScreenHeight()/2 + (CurrentSprite->Texture.height * Scale))))
    {
        DrawTexturePro(CurrentSprite->Texture, CurrentSprite->GetSourceRec(), CurrentSprite->GetPosRec(EnemyPos,Scale), Vector2{},0.f, WHITE);
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
    if (IsKeyDown(KEY_W)) Face = Direction::UP;
    if (IsKeyDown(KEY_A)) Face = Direction::LEFT;
    if (IsKeyDown(KEY_S)) Face = Direction::DOWN;
    if (IsKeyDown(KEY_D)) Face = Direction::RIGHT;

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
    EnemyPos = Vector2Subtract(WorldPos, HeroWorldPos); // Where the prop is drawn on the screen
    Vector2 ToTarget {Vector2Scale(Vector2Normalize(Vector2Subtract(Vector2Add(HeroScreenPos,{50,50}), EnemyPos)), Speed)}; // Calculate the distance from Enemy to Player

    // Only move enemy towards Player if within a certain range
    if (Vector2Length(Vector2Subtract(Vector2Add(HeroScreenPos, {50,0}), EnemyPos)) > Range)
    {
        ToTarget = {0.f,0.f};
        Moving = false;
    }
    else
    {
        WorldPos = Vector2Add(WorldPos, ToTarget);
        Moving = true;
    }

    PrevWorldPos = WorldPos;

    // Undo Movement if walking out-of-bounds
    if (WorldPos.x + EnemyPos.x < 0.f - (CurrentSprite->Texture.width/CurrentSprite->MaxFramesX)/2.f ||
        WorldPos.y + EnemyPos.y < 0.f - (CurrentSprite->Texture.height/CurrentSprite->MaxFramesY)/2.f ||
        WorldPos.x + (Screen->x - EnemyPos.x) > World->GetMap().width * World->GetScale() + (CurrentSprite->Texture.width/CurrentSprite->MaxFramesX)/2.f ||
        WorldPos.y + (Screen->y - EnemyPos.y) > World->GetMap().height * World->GetScale() + (CurrentSprite->Texture.height/CurrentSprite->MaxFramesY)/2.f)
    {
        UndoMovement();
    }

    // CheckCollision(Props.Under, HeroWorldPos, HeroScreenPos);
    // CheckCollision(Props.Over, HeroWorldPos, HeroScreenPos);
}

// Undo movement if walking out-of-bounds or colliding
void Enemy::UndoMovement()
{
    WorldPos = PrevWorldPos;
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

    if (Moving) 
    {
        CurrentSprite = Sprites.at(1);
    }
    else 
    {
        CurrentSprite = Sprites.at(0);
    }
}

// Check if colliding with props
void Enemy::CheckCollision(std::vector<std::vector<Prop>>* Props, Vector2 HeroWorldPos, Vector2 HeroScreenPos)
{
    for (auto& PropType:*Props) {
        for (auto& Prop:PropType) {
            if (Prop.HasCollision()) {
                
                // check physical collision
                if (CheckCollisionRecs(GetCollisionRec(HeroWorldPos), Prop.GetCollisionRec(WorldPos))) {   
                    
                    // manage pushable props
                    if (Prop.IsMoveable()) {
                        if (Prop.GetType() == PropType::GRASS) {
                            Prop.SetActive(true);
                        }
                    }
                    // if not pushable, block movement   
                    else {
                        UndoMovement();
                    }
                }
                else {
                    // This breaks the treasure chests when main character opens... 
                    // Prop.SetActive(false);
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
    }
}

// Return character collision dimensions
Rectangle Enemy::GetCollisionRec(Vector2 HeroWorldPos)
{
    return Rectangle 
    {
        EnemyPos.x + CurrentSprite->Texture.width/CurrentSprite->MaxFramesX/2.f,
        EnemyPos.y + CurrentSprite->Texture.height/CurrentSprite->MaxFramesY/2.f,
        (CurrentSprite->Texture.width/CurrentSprite->MaxFramesX) * Scale - ((CurrentSprite->Texture.width/CurrentSprite->MaxFramesX) * Scale) * .30f,
        (CurrentSprite->Texture.height/CurrentSprite->MaxFramesY) * Scale - ((CurrentSprite->Texture.height/CurrentSprite->MaxFramesY) * Scale) * .25f
    };
}

