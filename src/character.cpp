#include "headers/character.hpp"

Character::Character(Sprite Idle, 
                     Sprite Walk, 
                     Sprite Run, 
                     Sprite Attack, 
                     Sprite Hurt, 
                     Sprite Death, 
                     Sprite Push, 
                     Sprite Sleep, 
                     Sprite ItemGrab, 
                     Window* Screen, 
                     Background* World)
    : Idle{Idle},
      Walk{Walk},
      Run{Run},
      Attack{Attack},
      Hurt{Hurt},
      Death{Death},
      Push{Push},
      Sleep{Sleep},
      ItemGrab{ItemGrab},
      Screen{Screen},
      World{World}
{
    // Fill vector<Sprite*> with Sprite objects to easily loop through and call Sprite::Tick()
    Sprites.emplace_back(&(this->Idle));
    Sprites.emplace_back(&(this->Walk));
    Sprites.emplace_back(&(this->Run));
    Sprites.emplace_back(&(this->Attack));
    Sprites.emplace_back(&(this->Hurt));
    Sprites.emplace_back(&(this->Death));
    Sprites.emplace_back(&(this->Push));
    Sprites.emplace_back(&(this->Sleep));
    Sprites.emplace_back(&(this->ItemGrab));
}

Character::~Character()
{
    // Unload all Textures when destructing Character
    for (auto Sprite:Sprites)
    {
        UnloadTexture(Sprite->Texture);
    }
}

void Character::Tick(float DeltaTime, Props& Props)
{
    UpdateCharacterPos();

    SpriteTick(DeltaTime);

    CheckDirection();

    CheckMovement(Props);

    WalkOrRun();

    CheckAttack(Props);

    UpdateSource();

    CheckEmotion();

    CheckSleep();
}

// Draw character animation
void Character::Draw()
{
    DrawTexturePro(CurrentSprite->Texture, Source, Destination, Vector2{}, 0.f, WHITE);
}

/* 
    Loop through Sprites vector and call Sprite::Tick() to maintain sprite animations current 'spot'.
    Halfway through walk animation -> Run will start from the halfway point. Etc.
*/
void Character::SpriteTick(float DeltaTime)
{
    for (auto Sprite:Sprites)
    {
        Sprite->Tick(DeltaTime);
    }
}

// Update Character to middle of screen if screen is resized
void Character::UpdateCharacterPos()
{
    CharacterPos.x = Screen->x/2.f - (Scale * (0.5f * CurrentSprite->Texture.width/CurrentSprite->MaxFramesX));
    CharacterPos.y = Screen->y/2.f - (Scale * (0.5f * CurrentSprite->Texture.height/CurrentSprite->MaxFramesY));
    Destination.x = CharacterPos.x;
    Destination.y = CharacterPos.y;
    Destination.width = Scale * CurrentSprite->Texture.width/CurrentSprite->MaxFramesX;    
    Destination.height = Scale * CurrentSprite->Texture.height/CurrentSprite->MaxFramesY;    
}

// Check which orientation the character is facing
void Character::CheckDirection()
{
    if (!Locked)
    {
        if (IsKeyDown(KEY_W)) Face = Direction::UP;
        if (IsKeyDown(KEY_A)) Face = Direction::LEFT;
        if (IsKeyDown(KEY_S)) Face = Direction::DOWN;
        if (IsKeyDown(KEY_D)) Face = Direction::RIGHT;
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
void Character::CheckMovement(Props& Props)
{
    PrevWorldPos = WorldPos;
    Vector2 Direction{};

    if (!Locked) 
    {

        if (IsKeyDown(KEY_W)) 
        {
            Direction.y -= Speed;
        }
        if (IsKeyDown(KEY_A)) 
        {
            Direction.x -= Speed;
        }
        if (IsKeyDown(KEY_S)) 
        {
            Direction.y += Speed;
        }
        if (IsKeyDown(KEY_D)) 
        {
            Direction.x += Speed;
        }

        if (Vector2Length(Direction) != 0.f)
        {
            // set MapPos -= Direction
            WorldPos = Vector2Add(WorldPos, Vector2Scale(Vector2Normalize(Direction), Speed));
        }

        // Undo Movement if walking out-of-bounds
        if (WorldPos.x + CharacterPos.x < 0.f - (CurrentSprite->Texture.width/CurrentSprite->MaxFramesX)/2.f||
            WorldPos.y + CharacterPos.y < 0.f - (CurrentSprite->Texture.height/CurrentSprite->MaxFramesY)/2.f||
            WorldPos.x + (Screen->x - CharacterPos.x) > World->GetMap().width * World->GetScale() + (CurrentSprite->Texture.width/CurrentSprite->MaxFramesX)/2.f ||
            WorldPos.y + (Screen->y - CharacterPos.y) > World->GetMap().height * World->GetScale() + (CurrentSprite->Texture.height/CurrentSprite->MaxFramesY)/2.f)
        {
            UndoMovement();
        }

    }

    CheckCollision(Props.Under, Direction);
    CheckCollision(Props.Over, Direction);

}

// Undo movement if walking out-of-bounds or colliding
void Character::UndoMovement()
{
    WorldPos = PrevWorldPos;
}

// Check if colliding with props
void Character::CheckCollision(std::vector<std::vector<Prop>>* Props, Vector2 Direction)
{
    for (auto& PropType:*Props) {
        for (auto& Prop:PropType) {
            if (Prop.HasCollision()) {
                
                // check physical collision
                if (CheckCollisionRecs(GetCollisionRec(), Prop.GetCollisionRec(WorldPos))) {   
                    
                    // manage pushable props
                    if (Prop.IsMoveable()) {
                        if (Prop.GetType() == PropType::BOULDER || Prop.GetType() == PropType::STUMP) {
                            Colliding = true;   
                            if(!Prop.IsOutOfBounds()) {
                                if (Prop.CheckMovement(*World, WorldPos, Direction, Speed, Props)) {
                                    UndoMovement();
                                }
                            }
                            else {
                                UndoMovement();
                            }
                        }
                        if (Prop.GetType() == PropType::GRASS) {
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
                else {
                    Prop.SetActive(false);
                }

                // check interactable collision
                if (CheckCollisionRecs(GetCollisionRec(), Prop.GetInteractRec(WorldPos))) {
                    // Check for interact collision to display ! over character
                    if (Prop.IsInteractable()) {
                        if (Prop.IsVisible()) {
                            Interactable = true;
                        }
                    }

                    // Manage interacting with props
                    if (Interactable == true) {
                        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || IsKeyPressed(KEY_SPACE))
                            Interacting = true;
                                
                        if (Interacting == true) {
                            Prop.SetActive(true);
                            Interactable = false;
                            Locked = true;
                        }

                        if (Prop.IsOpened()) {
                            Interacting = false;
                            Interactable = false;
                            Prop.SetActive(false);
                            Locked = false;

                            // Make NPC's interactable again
                            if (Prop.GetType() == PropType::NPC_A || Prop.GetType() == PropType::NPC_B || Prop.GetType() == PropType::NPC_C || Prop.GetType() == PropType::ANIMATEDALTAR)
                            {
                                Prop.SetOpened(false);
                            }
                        }
                    }
                }
            }
            else {
                Interactable = false;
                Colliding = false;
            }
        }
    }
}

// Check if character is moving and change sprites if needed
void Character::WalkOrRun()
{
    if (IsKeyDown(KEY_LEFT_SHIFT))
    {
        Running = true;

        if (Colliding)
            Speed = 0.9f;
        else 
            Speed = 1.5f;
    }
    else 
    {
        Running = false;

        if (Colliding)
            Speed = 0.4f;
        else
            Speed = 1.0f;

    }
    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_A) || IsKeyDown(KEY_S) || IsKeyDown(KEY_D))
    {
        Walking = true;
        Sleeping = false;
    }
    else
    {
        Walking = false;
    }

    if (Running && Walking) 
    {
        CurrentSprite = &Run;
    }
    else if (Walking) 
    {
        CurrentSprite = &Walk;
    }
    else if (Sleeping)
    {
        CurrentSprite = &Sleep;
    }
    else 
    {
        CurrentSprite = &Idle;
    }
}

// manage attack sprites
void Character::CheckAttack(Props& Props)
{
    
    if (!Locked)
    {
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) || IsKeyDown(KEY_SPACE))
        {
            Attacking = true;
            Sleeping = false;
        }
        else
        {
            Attacking = false;
        }
    }

    if (Attacking)
    {
        CurrentSprite = &Attack;
    }
    else
    {
        CheckMovement(Props);
    }

}

// manage sleep skill
void Character::CheckSleep()
{
    float DeltaTime{GetFrameTime()};
    float UpdateTime{2.f/1.f};

    if (Sleeping)
    {
        RunningTime += DeltaTime;    

        if (RunningTime >= UpdateTime) {
            if (Health < 10) {
                SetHealth(1);
                RunningTime = 0.f;
            }
        }
    }
}

// manage character portraits 
void Character::CheckEmotion()
{
    if (Attacking)
        State = Emotion::ANGRY;
    else if (Sleeping)
        State = Emotion::SLEEPING;
    else if (Health >= 9)
        State = Emotion::HAPPY;
    else if (Health > 5 && Health < 9)
        State = Emotion::DEFAULT;
    else if (Health > 2 && Health <= 5)
        State = Emotion::NERVOUS;
    else 
        State = Emotion::SAD;
}

// Draw ! when interactable is true
void Character::DrawIndicator() 
{
    if (Interactable) {
        DrawTextureEx(Interact, Vector2Subtract(CharacterPos, Vector2{-58, -20}), 0.f, 2.f, WHITE);
    }
};

// Update which portion of the spritesheet is drawn
void Character::UpdateSource()
{
    Source.x = CurrentSprite->FrameX * CurrentSprite->Texture.width / CurrentSprite->MaxFramesX;
    Source.y = CurrentSprite->FrameY * CurrentSprite->Texture.height / CurrentSprite->MaxFramesY;
    Source.width = CurrentSprite->Texture.width/CurrentSprite->MaxFramesX;
    Source.height = CurrentSprite->Texture.height/CurrentSprite->MaxFramesY;
}

// Return character collision dimensions
Rectangle Character::GetCollisionRec()
{
    return Rectangle 
    {
        CharacterPos.x + CurrentSprite->Texture.width/CurrentSprite->MaxFramesX/2.f,
        CharacterPos.y + CurrentSprite->Texture.height/CurrentSprite->MaxFramesY/2.f,
        ((CurrentSprite->Texture.width/CurrentSprite->MaxFramesX) - (CurrentSprite->Texture.width/CurrentSprite->MaxFramesX)/1.5f) * Scale,
        ((CurrentSprite->Texture.height/CurrentSprite->MaxFramesY) - (CurrentSprite->Texture.height/CurrentSprite->MaxFramesY)/1.5f)  * Scale
    };
}

// -------------------------------------------------------- //

// Debug Function
void Character::SetHealth(int HP)
{
    Health += HP;
}

