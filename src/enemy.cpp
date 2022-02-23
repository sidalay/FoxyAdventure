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
    Sprites.emplace_back(&(this->Idle));
    Sprites.emplace_back(&(this->Walk));
    Sprites.emplace_back(&(this->Attack));
    Sprites.emplace_back(&(this->Hurt));
    Sprites.emplace_back(&(this->Death));
}

Enemy::~Enemy()
{
    // Unload all Textures when destructing Character
    for (auto Sprite:Sprites)
    {
        UnloadTexture(Sprite->Texture);
    }
}

void Enemy::Tick(float DeltaTime, Props& Props, Vector2 HeroWorldPos)
{
    SpriteTick(DeltaTime);

    CheckDirection();

    // CheckMovement(Props, HeroWorldPos);

    // CheckAttack(Props);

    UpdateSource();
}

// Draw character animation
void Enemy::Draw(Vector2 CharacterWorldPos)
{
    Vector2 ScreenPos {Vector2Subtract(WorldPos, CharacterWorldPos)}; // Where the prop is drawn on the screen

    if ((WorldPos.x >= (CharacterWorldPos.x + 615) - (GetScreenWidth()/2 + (CurrentSprite->Texture.width * Scale))) && (WorldPos.x <= (CharacterWorldPos.x + 615) + (GetScreenWidth()/2 + (CurrentSprite->Texture.width * Scale))) &&
       (WorldPos.y >= (CharacterWorldPos.y + 335) - (GetScreenHeight()/2 + (CurrentSprite->Texture.height * Scale))) && (WorldPos.y <= (CharacterWorldPos.y + 335) + (GetScreenHeight()/2 + (CurrentSprite->Texture.height * Scale))))
    {
        DrawTexturePro(CurrentSprite->Texture, CurrentSprite->GetSourceRec(), CurrentSprite->GetPosRec(ScreenPos, Scale), Vector2{}, 0.f, WHITE);
    }
}

/* 
    Loop through Sprites vector and call Sprite::Tick() to maintain sprite animations current 'spot'.
    Halfway through walk animation -> Run will start from the halfway point. Etc.
*/
void Enemy::SpriteTick(float DeltaTime)
{
    for (auto Sprite:Sprites)
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
void Enemy::CheckMovement(Props& Props, Vector2 HeroWorldPos)
{
    PrevWorldPos = WorldPos;
    Vector2 Direction{};

    // Undo Movement if walking out-of-bounds
    if (WorldPos.x + CharacterPos.x < 0.f - (CurrentSprite->Texture.width/CurrentSprite->MaxFramesX)/2.f||
        WorldPos.y + CharacterPos.y < 0.f - (CurrentSprite->Texture.height/CurrentSprite->MaxFramesY)/2.f||
        WorldPos.x + (Screen->x - CharacterPos.x) > World->GetMap().width * World->GetScale() + (CurrentSprite->Texture.width/CurrentSprite->MaxFramesX)/2.f ||
        WorldPos.y + (Screen->y - CharacterPos.y) > World->GetMap().height * World->GetScale() + (CurrentSprite->Texture.height/CurrentSprite->MaxFramesY)/2.f)
    {
        UndoMovement();
    }

    CheckCollision(Props.Under, Direction, HeroWorldPos);
    CheckCollision(Props.Over, Direction, HeroWorldPos);

}

// Undo movement if walking out-of-bounds or colliding
void Enemy::UndoMovement()
{
    WorldPos = PrevWorldPos;
}

// Check if colliding with props
void Enemy::CheckCollision(std::vector<std::vector<Prop>>* Props, Vector2 Direction, Vector2 HeroWorldPos)
{
    for (auto& PropType:*Props) {
        for (auto& Prop:PropType) {
            if (Prop.HasCollision()) {
                
                // check physical collision
                if (CheckCollisionRecs(GetCollisionRec(HeroWorldPos), Prop.GetCollisionRec(WorldPos))) {   
                    
                    // manage pushable props
                    if (Prop.IsMoveable()) {
                        if (Prop.GetType() == PropType::BOULDER || Prop.GetType() == PropType::STUMP) {
                            Colliding = true;   
                            UndoMovement();
                        }
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
                    Prop.SetActive(false);
                }
            }
            else {
                Colliding = false;
            }
        }
    }
}

// manage attack sprites
void Enemy::CheckAttack(Props& Props, Vector2 HeroWorldPos)
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
        CurrentSprite = &Attack;
    }
    else
    {
        CheckMovement(Props, HeroWorldPos);
        // CurrentSprite = &Idle;
    }

}

// Update which portion of the spritesheet is drawn
void Enemy::UpdateSource()
{
    Source.x = CurrentSprite->FrameX * CurrentSprite->Texture.width / CurrentSprite->MaxFramesX;
    Source.y = CurrentSprite->FrameY * CurrentSprite->Texture.height / CurrentSprite->MaxFramesY;
    Source.width = CurrentSprite->Texture.width/CurrentSprite->MaxFramesX;
    Source.height = CurrentSprite->Texture.height/CurrentSprite->MaxFramesY;
}

// Return character collision dimensions
Rectangle Enemy::GetCollisionRec(Vector2 HeroWorldPos)
{
    // This variable is needed for any object that is collidable with the Main Character
    Vector2 ScreenPos {Vector2Subtract(WorldPos, HeroWorldPos)};

    return Rectangle 
    {
        ScreenPos.x + CurrentSprite->Texture.width/CurrentSprite->MaxFramesX/2.f,
        ScreenPos.y + CurrentSprite->Texture.height/CurrentSprite->MaxFramesY/2.f,
        (CurrentSprite->Texture.width/CurrentSprite->MaxFramesX) * Scale - ((CurrentSprite->Texture.width/CurrentSprite->MaxFramesX) * Scale) * .30f,
        (CurrentSprite->Texture.height/CurrentSprite->MaxFramesY) * Scale - ((CurrentSprite->Texture.height/CurrentSprite->MaxFramesY) * Scale) * .25f
    };
}

