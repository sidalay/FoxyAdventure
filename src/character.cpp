#include "character.hpp"

Character::Character(const Sprite& Idle, 
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
                     Background& World)
    : GameTextures{GameTextures},
      Screen{Screen},
      World{World}
{
    WorldPos = Vector2Subtract(WorldPos, Offset);

    Sprites.emplace_back(Idle);
    Sprites.emplace_back(Walk);
    Sprites.emplace_back(Run);
    Sprites.emplace_back(Attack);
    Sprites.emplace_back(Hurt);
    Sprites.emplace_back(Death);
    Sprites.emplace_back(Push);
    Sprites.emplace_back(Sleep);
    Sprites.emplace_back(ItemGrab);

    SpriteIndex = static_cast<int>(FoxState::IDLE);
}

void Character::Tick(float DeltaTime, Props& Props, std::vector<Enemy>& Enemies, std::vector<Prop>& Trees)
{
    UpdateScreenPos();

    SpriteTick(DeltaTime);

    CheckDirection();

    WalkOrRun();

    CheckAttack();

    UpdateSource();

    CheckMovement(Props, Enemies, Trees);

    CheckEmotion();

    CheckSleep();

    CheckHealing();

    CheckIfAlive();
}

void Character::Draw()
{
    DrawTexturePro(Sprites.at(SpriteIndex).Texture, Source, Destination, Vector2{}, 0.f, WHITE);
}

void Character::SpriteTick(float DeltaTime)
{
    for (auto& Sprite:Sprites) {
        Sprite.Tick(DeltaTime);
    }
}

void Character::UpdateScreenPos()
{
    ScreenPos.x = Screen.x/2.f - (Scale * (0.5f * Sprites.at(SpriteIndex).Texture.width/Sprites.at(SpriteIndex).MaxFramesX));
    ScreenPos.y = Screen.y/2.f - (Scale * (0.5f * Sprites.at(SpriteIndex).Texture.height/Sprites.at(SpriteIndex).MaxFramesY));
    Destination.x = ScreenPos.x;
    Destination.y = ScreenPos.y;
    Destination.width = Scale * Sprites.at(SpriteIndex).Texture.width/Sprites.at(SpriteIndex).MaxFramesX;    
    Destination.height = Scale * Sprites.at(SpriteIndex).Texture.height/Sprites.at(SpriteIndex).MaxFramesY;    
}

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
                Sprites.at(SpriteIndex).FrameY = 0;
                break;
            case Direction::LEFT: 
                Sprites.at(SpriteIndex).FrameY = 1;
                break;
            case Direction::RIGHT:
                Sprites.at(SpriteIndex).FrameY = 2;
                break;
            case Direction::UP: 
                Sprites.at(SpriteIndex).FrameY = 3;
                break;
        }
}

void Character::CheckMovement(Props& Props, std::vector<Enemy>& Enemies, std::vector<Prop>& Trees)
{
    PrevWorldPos = WorldPos;
    Vector2 Direction{};

    // Check for movement input
    if (!Locked) {

        if (IsKeyDown(KEY_W)) {
            Direction.y -= Speed;
        }
        if (IsKeyDown(KEY_A)) {
            Direction.x -= Speed;
        }
        if (IsKeyDown(KEY_S)) {
            Direction.y += Speed;
        }
        if (IsKeyDown(KEY_D)) {
            Direction.x += Speed;
        }

        if (Vector2Length(Direction) != 0.f) {
            WorldPos = Vector2Add(WorldPos, Vector2Scale(Vector2Normalize(Direction), Speed));
        }

        // Undo Movement if walking out-of-bounds
        if (WorldPos.x + ScreenPos.x < 0.f - (Sprites.at(SpriteIndex).Texture.width/Sprites.at(SpriteIndex).MaxFramesX)/2.f ||
            WorldPos.y + ScreenPos.y < 0.f - (Sprites.at(SpriteIndex).Texture.height/Sprites.at(SpriteIndex).MaxFramesY)/2.f ||
            WorldPos.x + (Screen.x - ScreenPos.x) > World.GetMapSize().x * World.GetScale() + (Sprites.at(SpriteIndex).Texture.width/Sprites.at(SpriteIndex).MaxFramesX)/2.f ||
            WorldPos.y + (Screen.y - ScreenPos.y) > World.GetMapSize().y * World.GetScale() + (Sprites.at(SpriteIndex).Texture.height/Sprites.at(SpriteIndex).MaxFramesY)/2.f)
        {
            UndoMovement();
        }
    }

    CheckCollision(Props.Under, Direction, Enemies, Trees);
    CheckCollision(Props.Over, Direction, Enemies, Trees);
}

void Character::UndoMovement()
{
    WorldPos = PrevWorldPos;
}

void Character::CheckCollision(std::vector<std::vector<Prop>>& Props, const Vector2 Direction, std::vector<Enemy>& Enemies, std::vector<Prop>& Trees)
{
    DamageTime += GetFrameTime();
    
    if (Collidable) {

        // Loop through all Props for collision
        for (auto& PropType:Props) {
            for (auto& Prop:PropType) {
                if (Prop.HasCollision()) {
                    
                    // check physical collision
                    if (CheckCollisionRecs(GetCollisionRec(), Prop.GetCollisionRec(WorldPos))) {   
                        
                        // manage pushable props
                        if (Prop.IsMoveable()) {
                            if (Prop.GetType() == PropType::BOULDER || Prop.GetType() == PropType::STUMP) {
                                Colliding = true;   
                                if(!Prop.IsOutOfBounds()) {
                                    if (Prop.CheckMovement(World, WorldPos, Direction, Speed, Props)) {
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
                            if (Prop.IsSpawned()) {
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
                            if (Prop.IsSpawned()) {
                                Interactable = true;
                            }
                        }

                        // Manage interacting with props
                        if (Interactable) {
                            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || IsKeyPressed(KEY_SPACE))
                                Interacting = true;
                                    
                            if (Interacting) {
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

        // Loop for tree collision
        for (auto& Tree:Trees) {
            if (Tree.HasCollision()) {
                if (CheckCollisionRecs(GetCollisionRec(), Tree.GetCollisionRec(WorldPos))) {
                    if (Tree.IsSpawned()) {
                        UndoMovement();
                    }
                }
            }
        }
        
        // Loop through all Enemies for collision
        for (auto& Enemy:Enemies) {

            if (Enemy.GetType() != EnemyType::NPC) {
                if (Enemy.IsAlive()) {

                    // Check collision of Player against Enemy
                    if (CheckCollisionRecs(GetCollisionRec(), Enemy.GetCollisionRec())) {
                        if (!Enemy.IsDying()) {
                            TakeDamage();
                        }
                    }

                    // Check collision of Player's attack against Enemy
                    if (!Enemy.IsInvulnerable()) {
                        if (Attacking) {
                            if (CheckCollisionRecs(GetAttackRec(), Enemy.GetCollisionRec())) {
                                Enemy.Damaged(true);
                            }
                        }
                        else {
                            Enemy.Damaged(false);
                        }
                    }

                    // Check if Enemy Attack Collision is hitting Player
                    if (Enemy.IsAttacking()) {
                        if (CheckCollisionRecs(GetCollisionRec(), Enemy.GetAttackRec())) {
                            if (!Enemy.IsDying()) {
                                TakeDamage();
                            }
                        }
                    }

                    // Heal fox if enemy is killed
                    if (Enemy.IsDying()) {
                        if (Enemy.GetMaxHP() >= 8) {
                            if (Enemy.GetHealth() <= 0) {
                                Healing = true;
                                AmountToHeal = 4.f;
                            }
                        }
                        else if (Enemy.GetMaxHP() >= 5) {
                            if (Enemy.GetHealth() <= 0) {
                                Healing = true;
                                AmountToHeal = 2.5f;
                            }
                        }
                        else {
                            if (Enemy.GetHealth() <= 0) {
                                Healing = true;
                                AmountToHeal = 1.5f;
                            }
                        }
                    }
                }
                if (Hurting) {
                    if (DamageTime >= 1.f) {
                        Hurting = false;
                    }
                }
            }
        }
    }
}

void Character::WalkOrRun()
{
    if (IsKeyDown(KEY_LEFT_SHIFT)) {
        Running = true;
        if (Colliding) {Speed = 0.9f;}
        else {Speed = 2.5f;}
    }
    else {
        Running = false;
        if (Colliding) {Speed = 0.4f;}
        else {Speed = 1.5f;}
    }

    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_A) || IsKeyDown(KEY_S) || IsKeyDown(KEY_D)) {
        Walking = true;
        Sleeping = false;
    }
    else {
        Walking = false;
    }

    if (Running && Walking) {
        SpriteIndex = static_cast<int>(FoxState::RUN);
    }
    else if (Walking) {
        SpriteIndex = static_cast<int>(FoxState::WALK);
    }
    else if (Sleeping) {
        SpriteIndex = static_cast<int>(FoxState::SLEEP);
    }
    else {
        SpriteIndex = static_cast<int>(FoxState::IDLE);
    }
}

void Character::CheckAttack()
{
    AttackTime += GetFrameTime();

    if (!Locked) {
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) || IsKeyDown(KEY_SPACE)) {
            
            float AttackResetTime{0.7f};

            // Attack animation & damage window lasts 0.4 seconds
            if (AttackTime < 0.4f) {
                Sleeping = false;
                Attacking = true;
                SpriteIndex = static_cast<int>(FoxState::ATTACK);
            }
            // Reset window when reaching AttackResetTime
            else if (AttackTime >= AttackResetTime) {
                Attacking = false;
                AttackTime = 0.0f;
            }
            // 0.3 cooldown before Attack Window opens up again
            else {
                Attacking = false;
            }
        }
        else {
            Attacking = false;
        }        
    }
    else {
        Attacking = false;
    }
}

void Character::CheckSleep()
{
    if (Sleeping) {

        float DeltaTime{GetFrameTime()};
        float UpdateTime{2.f/1.f};
        RunningTime += DeltaTime;    

        if (RunningTime >= UpdateTime) {
            if (Health < 11.f) {
                AddHealth(0.5f);
                RunningTime = 0.f;
            }
        }
    }
}

void Character::CheckEmotion()
{
    if (Health < 1.f) {
        State = Emotion::DEAD;
    }
    else if (Attacking) {
        State = Emotion::ANGRY;
    }
    else if (Hurting) {
        State = Emotion::HURT;
    }
    else if (Sleeping) {
        State = Emotion::SLEEPING;
    }
    else if (Health >= 10.f) {
        State = Emotion::HAPPY;
    }
    else if (Health > 6.f && Health < 10.f) {
        State = Emotion::DEFAULT;
    }
    else if (Health > 3.f && Health <= 6.f) {
        State = Emotion::NERVOUS;
    }
    else if (Health > 1.f && Health <= 3.f) {
        State = Emotion::SAD;
    }
}

void Character::DrawIndicator() 
{
    if (Interactable) {
        DrawTextureEx(GameTextures.Interact, Vector2Subtract(ScreenPos, Vector2{-58.f, -20.f}), 0.f, 2.f, WHITE);
    }
};

void Character::UpdateSource()
{
    // Update which portion of the sprite sheet gets drawn
    Source.x = Sprites.at(SpriteIndex).FrameX * Sprites.at(SpriteIndex).Texture.width / Sprites.at(SpriteIndex).MaxFramesX;
    Source.y = Sprites.at(SpriteIndex).FrameY * Sprites.at(SpriteIndex).Texture.height / Sprites.at(SpriteIndex).MaxFramesY;
    Source.width = Sprites.at(SpriteIndex).Texture.width/Sprites.at(SpriteIndex).MaxFramesX;
    Source.height = Sprites.at(SpriteIndex).Texture.height/Sprites.at(SpriteIndex).MaxFramesY;
}

Rectangle Character::GetCollisionRec()
{
    return Rectangle 
    {
        ScreenPos.x + Sprites.at(SpriteIndex).Texture.width/Sprites.at(SpriteIndex).MaxFramesX/2.f,
        ScreenPos.y + Sprites.at(SpriteIndex).Texture.height/Sprites.at(SpriteIndex).MaxFramesY/2.f,
        ((Sprites.at(SpriteIndex).Texture.width/Sprites.at(SpriteIndex).MaxFramesX) - (Sprites.at(SpriteIndex).Texture.width/Sprites.at(SpriteIndex).MaxFramesX)/1.5f) * Scale,
        ((Sprites.at(SpriteIndex).Texture.height/Sprites.at(SpriteIndex).MaxFramesY) - (Sprites.at(SpriteIndex).Texture.height/Sprites.at(SpriteIndex).MaxFramesY)/1.5f)  * Scale
    };
}

Rectangle Character::GetAttackRec()
{
    switch (Face)
    {
        case Direction::DOWN:
            return Rectangle
            {
                ScreenPos.x + Sprites.at(SpriteIndex).Texture.width/Sprites.at(SpriteIndex).MaxFramesX/2.f,
                ScreenPos.y + (Sprites.at(SpriteIndex).Texture.height/Sprites.at(SpriteIndex).MaxFramesY/2.f * 2.f),
                ((Sprites.at(SpriteIndex).Texture.width/Sprites.at(SpriteIndex).MaxFramesX) - (Sprites.at(SpriteIndex).Texture.width/Sprites.at(SpriteIndex).MaxFramesX)/1.5f) * Scale,
                ((Sprites.at(SpriteIndex).Texture.height/Sprites.at(SpriteIndex).MaxFramesY) - (Sprites.at(SpriteIndex).Texture.height/Sprites.at(SpriteIndex).MaxFramesY)/1.5f)  * Scale
            }; 
        case Direction::LEFT: 
            return Rectangle
            {
                ScreenPos.x,
                ScreenPos.y + (Sprites.at(SpriteIndex).Texture.height/Sprites.at(SpriteIndex).MaxFramesY/2.f),
                ((Sprites.at(SpriteIndex).Texture.width/Sprites.at(SpriteIndex).MaxFramesX) - (Sprites.at(SpriteIndex).Texture.width/Sprites.at(SpriteIndex).MaxFramesX)/1.5f) * Scale,
                ((Sprites.at(SpriteIndex).Texture.height/Sprites.at(SpriteIndex).MaxFramesY) - (Sprites.at(SpriteIndex).Texture.height/Sprites.at(SpriteIndex).MaxFramesY)/1.5f)  * Scale
            }; 
        case Direction::RIGHT:
            return Rectangle
            {
                ScreenPos.x + (Sprites.at(SpriteIndex).Texture.width/Sprites.at(SpriteIndex).MaxFramesX/2.f * 2.f),
                ScreenPos.y + Sprites.at(SpriteIndex).Texture.height/Sprites.at(SpriteIndex).MaxFramesY/2.f,
                ((Sprites.at(SpriteIndex).Texture.width/Sprites.at(SpriteIndex).MaxFramesX) - (Sprites.at(SpriteIndex).Texture.width/Sprites.at(SpriteIndex).MaxFramesX)/1.5f) * Scale,
                ((Sprites.at(SpriteIndex).Texture.height/Sprites.at(SpriteIndex).MaxFramesY) - (Sprites.at(SpriteIndex).Texture.height/Sprites.at(SpriteIndex).MaxFramesY)/1.5f)  * Scale
            }; 
        case Direction::UP:
            return Rectangle
            {
                ScreenPos.x + Sprites.at(SpriteIndex).Texture.width/Sprites.at(SpriteIndex).MaxFramesX/2.f,
                ScreenPos.y,
                ((Sprites.at(SpriteIndex).Texture.width/Sprites.at(SpriteIndex).MaxFramesX) - (Sprites.at(SpriteIndex).Texture.width/Sprites.at(SpriteIndex).MaxFramesX)/1.5f) * Scale,
                ((Sprites.at(SpriteIndex).Texture.height/Sprites.at(SpriteIndex).MaxFramesY) - (Sprites.at(SpriteIndex).Texture.height/Sprites.at(SpriteIndex).MaxFramesY)/1.5f)  * Scale
            };
        default:
            return Rectangle
            {
                
            };
    }

}

void Character::CheckIfAlive()
{
    if (Health <= 1.f)
    {
        Locked = true;
        Alive = false;
        SpriteIndex = static_cast<int>(FoxState::DEATH);
    }
    else
    {
        Locked = false;
        Alive = true;
    }
}

void Character::TakeDamage()
{
    float UpdateTime {2.f/1.f};
    float HurtUpdateTime{1.f};

    // Cancel sleeping state if attacked
    if (Sleeping) {
        Sleeping = false;
    }

    // How often the hurt animation should play
    if (DamageTime <= HurtUpdateTime) {
        SpriteIndex = static_cast<int>(FoxState::HURT);
        Hurting = true;
    }

    // How often health should decrease when colliding into enemy
    if (DamageTime >= UpdateTime) {
        if (Health > 0.f) {
            Health -= 0.5f;
        }
        DamageTime = 0.f;
    }
}

// -------------------------------------------------------- //

// Debug Function
void Character::AddHealth(float HP)
{
    Health += HP;
    if (Health > 11) {
        Health = 11;
    }
}

void Character::HealOverTime(float HP)
{   
    // Gradually heal fox when killing an enemy
    float TimeToHeal{2.f/3.f};
    HealTime += GetFrameTime();
    static float StopHealing{HP};

    if (StopHealing >= HP) {
        StopHealing = 0.f;
        Healing = false;
        return;
    }
    else {
        if (HealTime >= TimeToHeal) {
            AddHealth(0.5f);
            StopHealing += 0.5f;
            HealTime = 0.f;
        }
    }
}

void Character::CheckHealing()
{
    if (Healing) {
        HealOverTime(AmountToHeal);
    }
}