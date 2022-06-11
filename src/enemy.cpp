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

Enemy::Enemy(Sprite Idle,
             Sprite Walk,
             Sprite Attack,
             Sprite Hurt,
             Sprite Death,
             Sprite Projectile,
             EnemyType Race,
             EnemyType Type,
             Vector2 WorldPos,
             Window* Screen,
             Background* World,
             GameTexture& GameTextures,
             int Health,
             float Scale) 
    : Race{Race},
      Type{Type},
      WorldPos{WorldPos},
      Screen{Screen},
      World{World},
      GameTextures{&GameTextures},
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

    CurrentSprite = &Sprites.at(0);
    ShootingSprite = &Sprites.at(5);

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
    std::random_device Seed;
    std::uniform_int_distribution<int> RandomRange{60, 80};
    std::uniform_int_distribution<int> RandomIdleTime{5, 9};
    std::uniform_int_distribution<int> RandomLeftRight{1, 10};
    std::mt19937 RNG{std::mt19937{Seed()}};

    ActionIdleTime = static_cast<float>(RandomIdleTime(RNG));
    MoveXRange = RandomRange(RNG);
    MoveYRange = RandomRange(RNG);
    LeftOrRight = RandomLeftRight(RNG);
}

Enemy::Enemy(Sprite NpcIdle,
             Sprite NpcIdleTwo,
             Sprite NpcWalk,
             Sprite NpcMisc,
             Sprite NpcSleep,
             EnemyType Race,
             Vector2 WorldPos,
             Window* Screen,
             Background* World,
             GameTexture& GameTextures,
             float Scale)
    : Race{Race},
      Type{EnemyType::NPC},
      WorldPos{WorldPos},
      Screen{Screen},
      World{World},
      GameTextures{&GameTextures},
      Scale{Scale}
{
    // Fill vector<Sprite*> with Sprite objects to easily loop through and call Sprite::Tick()
    Sprites.emplace_back(NpcIdle);
    Sprites.emplace_back(NpcIdleTwo);
    Sprites.emplace_back(NpcWalk);
    Sprites.emplace_back(NpcMisc);
    Sprites.emplace_back(NpcSleep);

    CurrentSprite = &Sprites.at(0);

    // Generate RNG for current object used for randomizing AI movement
    std::random_device Seed;
    std::uniform_int_distribution<int> RandomRange{60,80};
    std::uniform_int_distribution<int> RandomIdleTime{5,9};
    std::uniform_int_distribution<int> RandomLeftRight{1,10};
    std::uniform_int_distribution<int> RandomActionState{1,10};
    std::mt19937 RNG{std::mt19937{Seed()}};

    ActionIdleTime = static_cast<float>(RandomIdleTime(RNG));
    MoveXRange = RandomRange(RNG);
    MoveYRange = RandomRange(RNG);
    LeftOrRight = RandomLeftRight(RNG);
    ActionState = RandomActionState(RNG);
}

void Enemy::Tick(float DeltaTime, Props& Props, Vector2 HeroWorldPos, Vector2 HeroScreenPos, std::vector<Enemy>& Enemies, std::vector<Prop>& Trees)
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
        if (Type == EnemyType::BOSS) 
            CheckBossSummon(HeroWorldPos);
    }
}

void Enemy::Draw(Vector2 HeroWorldPos)
{
    if (WithinScreen(HeroWorldPos))
    {
        Visible = true;

        if (!OOB) {
            if (Type == EnemyType::NORMAL || Type == EnemyType::NPC || (Type == EnemyType::BOSS && Summoned)) {
                if (Hurting) {
                    DrawTexturePro(CurrentSprite->Texture, CurrentSprite->GetSourceRec(), CurrentSprite->GetPosRec(ScreenPos,Scale), Vector2{},0.f, RED);
                }
                else {
                    DrawTexturePro(CurrentSprite->Texture, CurrentSprite->GetSourceRec(), CurrentSprite->GetPosRec(ScreenPos,Scale), Vector2{},0.f, WHITE);
                }

                // Draw Ranged projectile
                if (Ranged && Attacking && !Dying && !Hurting) {
                    DrawTexturePro(ShootingSprite->Texture, ShootingSprite->GetSourceRec(), CurrentSprite->GetPosRec(UpdateProjectile(),Scale), Vector2{},0.f, WHITE);
                }
            }
        }
        // OOB is used for debugging purposes only
        else {
            DrawTexturePro(CurrentSprite->Texture, CurrentSprite->GetSourceRec(), CurrentSprite->GetPosRec(ScreenPos,Scale), Vector2{},0.f, PURPLE);
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
    /* 
        Loop through Sprites vector and call Sprite::Tick() to maintain sprite animations current 'spot'.
        Halfway through walk animation -> Run will start from the halfway point. Etc.
    */
    for (auto& Sprite:Sprites)
    {   
        if (Type != EnemyType::NPC) {
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
        else {
            Sprite.Tick(DeltaTime);
        }
    }
}

void Enemy::UpdateScreenPos(Vector2 HeroWorldPos)
{
    ScreenPos = Vector2Subtract(WorldPos, HeroWorldPos);
}

void Enemy::CheckDirection()
{   
    // NPC only has left and right orientation sprites
    if (Type == EnemyType::NPC) {
        if (AIX < 0) Face = Direction::LEFT;
        if (AIX > 0) Face = Direction::RIGHT;
    }
    else {
        if (!Chasing) {
            if (AIY < 0) Face = Direction::UP;
            if (AIX < 0) Face = Direction::LEFT;
            if (AIY > 0) Face = Direction::DOWN;
            if (AIX > 0) Face = Direction::RIGHT;
        }
    }

    if (Type != EnemyType::NPC) {
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
    else {
        switch (Face)
        {
            case Direction::DOWN: 
                CurrentSprite->FrameY = 0;
                break;
            case Direction::LEFT: 
                CurrentSprite->FrameY = 1;
                break;
            case Direction::RIGHT:
                CurrentSprite->FrameY = 0;
                break;
            case Direction::UP: 
                CurrentSprite->FrameY = 1;
                break;
        }
    }

    if (Ranged) {
        switch (Face)
        {
            case Direction::DOWN: 
                ShootingSprite->FrameY = 0;
                break;
            case Direction::LEFT: 
                ShootingSprite->FrameY = 1;
                break;
            case Direction::RIGHT:
                ShootingSprite->FrameY = 2;
                break;
            case Direction::UP: 
                ShootingSprite->FrameY = 3;
                break;
        }
    }
}

void Enemy::CheckMovement(Props& Props, Vector2 HeroWorldPos, Vector2 HeroScreenPos, std::vector<Enemy>& Enemies, std::vector<Prop>& Trees)
{
    PrevWorldPos = WorldPos;
    
    // Enemy movement AI
    EnemyAI();

    // Chase Player
    if (Type != EnemyType::NPC) {
        EnemyAggro(HeroScreenPos);
    }

    // Check if moving OOB 
    OutOfBounds();

    // Check for collision against player or props
    if (Alive) {
        CheckCollision(Props.Under, HeroWorldPos, Enemies, Trees);
        CheckCollision(Props.Over, HeroWorldPos, Enemies, Trees);
    }
}

void Enemy::UndoMovement()
{
    WorldPos = PrevWorldPos;
}

void Enemy::OutOfBounds()
{
    if (WorldPos.x < 0.f ||
        WorldPos.y < 0.f ||
        WorldPos.x > World->GetMapSize().x * World->GetScale() - (CurrentSprite->Texture.width/CurrentSprite->MaxFramesX)*Scale ||
        WorldPos.y > World->GetMapSize().y * World->GetScale() - (CurrentSprite->Texture.height/CurrentSprite->MaxFramesY)*Scale) 
    {
        UndoMovement();
        // OOB = true;
    }
    else {
        // OOB = false;
    }
}

void Enemy::NeutralAction()
{
    // Check if Enemy is moving and change sprites if needed
    if (Chasing || Walking) {
        if (Type == EnemyType::NPC) {
            CurrentSprite = &Sprites.at(2); // Walking sprite for NPC
        }
        else {
            CurrentSprite = &Sprites.at(1); // Walking sprite 
        }
    }
    else if (MiscAction) {
        if (Type == EnemyType::NPC) {
            CurrentSprite = &Sprites.at(3); // Misc sprite
        }
    }
    else if (Sleeping) {
        if (Type == EnemyType::NPC) {
            CurrentSprite = &Sprites.at(4); // Sleeping sprite
        }
    }
    else {
        if (Type == EnemyType::NPC) {
            if (IdleTwo) {
                CurrentSprite = &Sprites.at(1); // IdleTwo sprite
            }
            else {
                CurrentSprite = &Sprites.at(0); // Idle sprite
            }
        }
        else {
            CurrentSprite = &Sprites.at(0); // Idle sprite
        }
    }
}

void Enemy::CheckCollision(std::vector<std::vector<Prop>>& Props, Vector2 HeroWorldPos, std::vector<Enemy>& Enemies, std::vector<Prop>& Trees)
{
    if (Race != EnemyType::CROW) { // Crows should not be blocked by anything

        // Prop collision handling
        for (auto& PropType:Props) {
            for (auto& Prop:PropType) {
                if (Prop.HasCollision()) {
                    
                    /*
                        Undo movement wasn't working so implemented a reverse aggro logic. 
                        When enemy gets within min aggro range of an obstacle it 'runs' away from it.
                        Therefore it will never collide with a prop.
                    */
                    Vector2 PropScreenPos{Vector2{Prop.GetCollisionRec(HeroWorldPos).x, Prop.GetCollisionRec(HeroWorldPos).y}}; // Grab the collision rectangle screen position
                    Vector2 RadiusAroundEnemy{5,5};
                    Vector2 ToTarget {Vector2Scale(Vector2Normalize(Vector2Subtract(Vector2Add(PropScreenPos, RadiusAroundEnemy), ScreenPos)), Speed)}; // Calculate the distance from Enemy to Prop
                    float AvoidProp{Vector2Length(Vector2Subtract(Vector2Add(PropScreenPos, RadiusAroundEnemy), ScreenPos))};

                    // move away from moveable objects 
                    if (Prop.IsMoveable()) {
                        if (Prop.GetType() == PropType::BOULDER) {
                            if (AvoidProp <= MinRange) {
                                WorldPos = Vector2Subtract(WorldPos, ToTarget);
                            }
                        }
                    }
                    // move away from all other objects
                    else if (Prop.IsSpawned()) {
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
        
        // Tree collision handling
        for (auto& Tree:Trees) {
            if (Tree.HasCollision()) {
                Vector2 TreeScreenPos{Vector2{Tree.GetCollisionRec(HeroWorldPos).x, Tree.GetCollisionRec(HeroWorldPos).y}}; // Grab the collision rectangle screen position
                Vector2 RadiusAroundEnemy{5,5};
                Vector2 ToTarget {Vector2Scale(Vector2Normalize(Vector2Subtract(Vector2Add(TreeScreenPos, RadiusAroundEnemy), ScreenPos)), Speed)}; // Calculate the distance from Enemy to Tree
                float AvoidTree{Vector2Length(Vector2Subtract(Vector2Add(TreeScreenPos, RadiusAroundEnemy), ScreenPos))};

                if (Tree.IsSpawned()) {
                    if (AvoidTree <= MinRange) {
                        WorldPos = Vector2Subtract(WorldPos, ToTarget);
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
            Vector2 ToTarget{Vector2Scale(Vector2Normalize(Vector2Subtract(Vector2Add(Enemy.GetEnemyPos(), RadiusAroundEnemy), ScreenPos)), Speed)}; // Calculate the distance from this->Enemy to Enemy
            float AvoidEnemy{Vector2Length(Vector2Subtract(Vector2Add(Enemy.GetEnemyPos(), RadiusAroundEnemy), ScreenPos))};

            if ((AvoidEnemy <= MinRange) && !Dying) {
                WorldPos = Vector2Subtract(WorldPos, ToTarget);
            }
        }
    }
}

void Enemy::CheckAttack()
{
    if (Attacking) {
        CurrentSprite = &Sprites.at(2);

        if (Ranged) {
            ShootingSprite = &Sprites.at(5);
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
    float UpdateTime {0.7f};
    float HurtUpdateTime{0.6f};

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
            CurrentSprite = &Sprites.at(3);
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
    // Amount of time needed for death animation to complete beginning to end
    float EndTime{1.35f};

    if (Health <= 0) {
        // Set to death sprite
        CurrentSprite = &Sprites.at(4);
        
        // Turn off any TakeDamage() functionality
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
    // Only run this code block once
    if (Intro && LeftOrRight <= 5) {
        AIX = -AIX;
        AIY = -AIY;
        IdleTwo = false;
        Intro = false;
    }

    if (!Chasing && !Blocked) {

        ActionTime += GetFrameTime();

        if (Type == EnemyType::NPC) {

            switch (ActionState)
            {
                case 1:
                case 2:
                case 3:
                case 4:
                case 5:
                case 6:
                {
                    Movement.x += AIX;
                    Movement.y += AIY;

                    // Handle walking 
                    if (ActionTime <= ActionIdleTime/2) {
                        Walking = true;
                        WorldPos.x += AIX;
                        WorldPos.y += AIY;
                    }
                    else if (ActionTime >= ActionIdleTime) {
                        ActionTime = 0.0f;
                        std::random_device Seed;
                        std::uniform_int_distribution<int> RandomActionState{1, 10};
                        std::mt19937 RNG{std::mt19937{Seed()}};
                        ActionState = RandomActionState(RNG);
                        IdleTwo = !IdleTwo;
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
                    break;
                }
                case 7:
                {
                    // Handle misc action 
                    if (ActionTime <= ActionIdleTime) {
                        MiscAction = true;
                    }
                    else {
                        ActionTime = 0.0f;
                        std::uniform_int_distribution<int> RandomActionState{1, 10};
                        std::random_device Seed;
                        std::mt19937 RNG{std::mt19937{Seed()}};
                        ActionState = RandomActionState(RNG);
                        MiscAction = false;
                    }
                    break;
                }
                case 8:
                case 9:
                case 10:
                {
                    // Handle sleep action
                    if (ActionTime <= ActionIdleTime) {
                        Sleeping = true;
                    }
                    else {
                        ActionTime = 0.0f;
                        std::uniform_int_distribution<int> RandomActionState{1, 10};
                        std::random_device Seed;
                        std::mt19937 RNG{std::mt19937{Seed()}};
                        ActionState = RandomActionState(RNG);
                        Sleeping = false;
                    }
                    break;
                }
                default:
                    ActionState = 1;
                    break;
            }
        }
        else {
            Movement.x += AIX;
            Movement.y += AIY;

            // Handle walking 
            if (ActionTime <= ActionIdleTime/2) {
                Walking = true;
                WorldPos.x += AIX;
                WorldPos.y += AIY;
            }
            else if (ActionTime >= ActionIdleTime) {
                ActionTime = 0.0f;
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
}

void Enemy::EnemyAggro(Vector2 HeroScreenPos)
{
    // Calculate the distance from Enemy to Player
    Vector2 ToTarget {Vector2Scale(Vector2Normalize(Vector2Subtract(Vector2Add(HeroScreenPos,{50,50}), ScreenPos)), Speed)}; 
    
    if (Alive && !Stopped && !Invulnerable && !Blocked && !Dying) {
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
            Trajectory += 1.3f;
        }

        CheckAttack();
    }
}

void Enemy::DrawHP()
{
    float LifeBarScale{2.f};
    float SingleBarWidth{static_cast<float>(GameTextures->LifebarLeftEmpty.width) * LifeBarScale};
    float MaxBarWidth{SingleBarWidth * MaxHP};
    float CenterLifeBar {(MaxBarWidth - (CurrentSprite->Texture.width/CurrentSprite->MaxFramesX)*Scale ) / 2.f};
    Vector2 LifeBarPos{};                           // Where the lifebar is positioned
    Vector2 LifeBarPosAdd{SingleBarWidth, 0};       // spacing between each life 'bar'

    // update lifebarpos to center of enemy sprite
    LifeBarPos = Vector2Subtract(ScreenPos, Vector2{CenterLifeBar, 20});

    for (auto i = 1; i <= MaxHP; ++i) {
        if (i <= Health) {
            // far left of lifebar
            if (i == 1) {
                DrawTextureEx(GameTextures->LifebarLeftFilled, LifeBarPos, 0.f, LifeBarScale, WHITE);
            }
            // far right of lifebar
            else if (i == MaxHP) {
                DrawTextureEx(GameTextures->LifebarRightFilled, LifeBarPos, 0.f, LifeBarScale, WHITE);
            }
            // middle of lifebar
            else {
                DrawTextureEx(GameTextures->LifebarMiddleFilled, LifeBarPos, 0.f, LifeBarScale, WHITE);
            }
        }
        else {
            // far left of lifebar
            if (i == 1) {
                DrawTextureEx(GameTextures->LifebarLeftEmpty, LifeBarPos, 0.f, LifeBarScale, WHITE);
            }
            // far right of lifebar
            else if (i == MaxHP) {
                DrawTextureEx(GameTextures->LifebarRightEmpty, LifeBarPos, 0.f, 2.f, WHITE);
            }
            // middle of lifebar
            else {
                DrawTextureEx(GameTextures->LifebarMiddleEmpty, LifeBarPos, 0.f, 2.f, WHITE);
            }
        }

        // add spacing between each bar
        LifeBarPos = Vector2Add(LifeBarPos, LifeBarPosAdd);
    }
}

void Enemy::CheckBossSummon(Vector2 HeroWorldPos)
{
    if (!WithinScreen(HeroWorldPos)) {
        if ((Type == EnemyType::BOSS) && (MonsterCounter[Race] <= 0)) {
            if (!Summoned) {
                Summoned = true;
            }
        }
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
                ScreenPos.x + ((CurrentSprite->Texture.width/CurrentSprite->MaxFramesX) * Scale) * .31f,
                ScreenPos.y + ((CurrentSprite->Texture.height/CurrentSprite->MaxFramesY) * Scale) * .40f,
                ((CurrentSprite->Texture.width/CurrentSprite->MaxFramesX) * Scale) * .40f,
                ((CurrentSprite->Texture.height/CurrentSprite->MaxFramesY) * Scale) * .30f
            };
        }
        case EnemyType::CROW:
        {
            return Rectangle
            {
                ScreenPos.x + ((CurrentSprite->Texture.width/CurrentSprite->MaxFramesX) * Scale) * .31f,
                ScreenPos.y + ((CurrentSprite->Texture.height/CurrentSprite->MaxFramesY) * Scale) * .31f,
                ((CurrentSprite->Texture.width/CurrentSprite->MaxFramesX) * Scale) * .40f,
                ((CurrentSprite->Texture.height/CurrentSprite->MaxFramesY) * Scale) * .30f
            };
        }
        case EnemyType::SQUIRREL:
        {
            return Rectangle
            {
                ScreenPos.x + ((CurrentSprite->Texture.width/CurrentSprite->MaxFramesX) * Scale) * .35f,
                ScreenPos.y + ((CurrentSprite->Texture.height/CurrentSprite->MaxFramesY) * Scale) * .35f,
                ((CurrentSprite->Texture.width/CurrentSprite->MaxFramesX) * Scale) * .20f,
                ((CurrentSprite->Texture.height/CurrentSprite->MaxFramesY) * Scale) * .30f
            };
        }
        default:
        {
            return Rectangle 
            {
                ScreenPos.x + ((CurrentSprite->Texture.width/CurrentSprite->MaxFramesX) * Scale) * .31f,
                ScreenPos.y + ((CurrentSprite->Texture.height/CurrentSprite->MaxFramesY) * Scale) * .31f,
                ((CurrentSprite->Texture.width/CurrentSprite->MaxFramesX) * Scale) * .40f,
                ((CurrentSprite->Texture.height/CurrentSprite->MaxFramesY) * Scale) * .60f
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
                    UpdateProjectile().x + ShootingSprite->Texture.width/ShootingSprite->MaxFramesX/6.f,
                    UpdateProjectile().y + ((ShootingSprite->Texture.height/ShootingSprite->MaxFramesY)/2 * Scale),
                    ((ShootingSprite->Texture.width/ShootingSprite->MaxFramesX/2.f) * Scale - (ShootingSprite->Texture.width/ShootingSprite->MaxFramesX)/1.5f) * Scale,
                    ((ShootingSprite->Texture.height/ShootingSprite->MaxFramesY/3.f) * Scale - (ShootingSprite->Texture.height/ShootingSprite->MaxFramesY)/1.5f)  * Scale
                };
            case Direction::LEFT:
                return Rectangle
                {
                    UpdateProjectile().x + (ShootingSprite->Texture.height/ShootingSprite->MaxFramesY/2.f),
                    UpdateProjectile().y + (ShootingSprite->Texture.height/ShootingSprite->MaxFramesY/6.f),
                    ((ShootingSprite->Texture.width/ShootingSprite->MaxFramesX/3.f) * Scale - (ShootingSprite->Texture.width/ShootingSprite->MaxFramesX)/1.5f) * Scale,
                    ((ShootingSprite->Texture.height/ShootingSprite->MaxFramesY/2.f) * Scale - (ShootingSprite->Texture.height/ShootingSprite->MaxFramesY)/1.5f)  * Scale
                };
            case Direction::RIGHT:
                return Rectangle
                {
                    UpdateProjectile().x + (ShootingSprite->Texture.width/ShootingSprite->MaxFramesX * Scale)/2.5f,
                    UpdateProjectile().y + (ShootingSprite->Texture.height/ShootingSprite->MaxFramesY/6.f),
                    ((ShootingSprite->Texture.width/ShootingSprite->MaxFramesX/3.f) * Scale - (ShootingSprite->Texture.width/ShootingSprite->MaxFramesX)/1.5f) * Scale,
                    ((ShootingSprite->Texture.height/ShootingSprite->MaxFramesY/2.f) * Scale - (ShootingSprite->Texture.height/ShootingSprite->MaxFramesY)/1.5f)  * Scale
                };
            case Direction::UP:
                return Rectangle
                {
                    UpdateProjectile().x + ShootingSprite->Texture.width/ShootingSprite->MaxFramesX/6.f,
                    UpdateProjectile().y + ShootingSprite->Texture.height/ShootingSprite->MaxFramesY/2.f,
                    ((ShootingSprite->Texture.width/ShootingSprite->MaxFramesX/2.f) * Scale - (ShootingSprite->Texture.width/ShootingSprite->MaxFramesX)/1.5f) * Scale,
                    ((ShootingSprite->Texture.height/ShootingSprite->MaxFramesY/3.f) * Scale - (ShootingSprite->Texture.height/ShootingSprite->MaxFramesY)/1.5f)  * Scale
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
                    ScreenPos.x + CurrentSprite->Texture.width/CurrentSprite->MaxFramesX/6.f,
                    ScreenPos.y + (CurrentSprite->Texture.height/CurrentSprite->MaxFramesY * Scale),
                    ((CurrentSprite->Texture.width/CurrentSprite->MaxFramesX/2.f) * Scale - (CurrentSprite->Texture.width/CurrentSprite->MaxFramesX)/1.5f) * Scale,
                    ((CurrentSprite->Texture.height/CurrentSprite->MaxFramesY/3.f) * Scale - (CurrentSprite->Texture.height/CurrentSprite->MaxFramesY)/1.5f)  * Scale
                }; 
            case Direction::LEFT: 
                return Rectangle
                {
                    ScreenPos.x - (CurrentSprite->Texture.height/CurrentSprite->MaxFramesY/1.8f),
                    ScreenPos.y + (CurrentSprite->Texture.height/CurrentSprite->MaxFramesY/6.f),
                    ((CurrentSprite->Texture.width/CurrentSprite->MaxFramesX/3.f) * Scale - (CurrentSprite->Texture.width/CurrentSprite->MaxFramesX)/1.5f) * Scale,
                    ((CurrentSprite->Texture.height/CurrentSprite->MaxFramesY/2.f) * Scale - (CurrentSprite->Texture.height/CurrentSprite->MaxFramesY)/1.5f)  * Scale
                }; 
            case Direction::RIGHT:
                return Rectangle
                {
                    ScreenPos.x + (CurrentSprite->Texture.width/CurrentSprite->MaxFramesX * Scale)/1.2f,
                    ScreenPos.y + (CurrentSprite->Texture.height/CurrentSprite->MaxFramesY/6.f),
                    ((CurrentSprite->Texture.width/CurrentSprite->MaxFramesX/3.f) * Scale - (CurrentSprite->Texture.width/CurrentSprite->MaxFramesX)/1.5f) * Scale,
                    ((CurrentSprite->Texture.height/CurrentSprite->MaxFramesY/2.f) * Scale - (CurrentSprite->Texture.height/CurrentSprite->MaxFramesY)/1.5f)  * Scale
                }; 
            case Direction::UP:
                return Rectangle
                {
                    ScreenPos.x + CurrentSprite->Texture.width/CurrentSprite->MaxFramesX/6.f,
                    ScreenPos.y - CurrentSprite->Texture.height/CurrentSprite->MaxFramesY/1.5f,
                    ((CurrentSprite->Texture.width/CurrentSprite->MaxFramesX/2.f) * Scale - (CurrentSprite->Texture.width/CurrentSprite->MaxFramesX)/1.5f) * Scale,
                    ((CurrentSprite->Texture.height/CurrentSprite->MaxFramesY/3.f) * Scale - (CurrentSprite->Texture.height/CurrentSprite->MaxFramesY)/1.5f)  * Scale
                };
            default:
                return Rectangle
                {
                    
                };
        }
    }
}

bool Enemy::WithinScreen(Vector2 HeroWorldPos)
{
    if (
        (WorldPos.x >= (HeroWorldPos.x + 615) - (GetScreenWidth()/2 + (CurrentSprite->Texture.width * Scale))) && 
        (WorldPos.x <= (HeroWorldPos.x + 615) + (GetScreenWidth()/2 + (CurrentSprite->Texture.width * Scale))) &&
        (WorldPos.y >= (HeroWorldPos.y + 335) - (GetScreenHeight()/2 + (CurrentSprite->Texture.height * Scale))) && 
        (WorldPos.y <= (HeroWorldPos.y + 335) + (GetScreenHeight()/2 + (CurrentSprite->Texture.height * Scale)))
       ) {
        return true;
    }
    else {
        return false;
    }
}
