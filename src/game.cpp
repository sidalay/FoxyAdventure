#include "headers/game.hpp"
#include <assert.h>
#include <vector>

namespace Game 
{
    void Run() 
    {
        // Initialization ---------------------------
        Window Window {1280, 720};                               // Window Dimensions
        int FPS {144};                                           // Frames Per Second
        Game::Initialize(Window, FPS, "Cryptex Adventure");      // Create Window
        SetTraceLogLevel(LOG_WARNING);
        GameState State{GameState::RUNNING};
        GameState NextState{};

        // Initialize Background
        Background MapBG{};

        // Initialize Character
        Character Champion
        {
            Sprite{"sprites/characters/fox/Fox_idle.png", 4, 4}, 
            Sprite{"sprites/characters/fox/Fox_walk.png", 4, 4},
            Sprite{"sprites/characters/fox/Fox_run.png", 4, 4}, 
            Sprite{"sprites/characters/fox/Fox_melee.png", 4, 4},
            Sprite{"sprites/characters/fox/Fox_hit.png", 2, 4}, 
            Sprite{"sprites/characters/fox/Fox_die.png", 1, 4},
            Sprite{"sprites/characters/fox/Fox_push.png", 4, 4},
            Sprite{"sprites/characters/fox/Fox_sleeping.png", 4, 1}, 
            Sprite{"sprites/characters/fox/Fox_itemGot.png", 1, 4},
            &Window, &MapBG
        };

        // Initialize Enemies
        std::vector<Enemy> Enemies{Game::InitializeEnemies(MapBG, Window)};

        // Initialize Crows
        std::vector<Enemy> Crows{Game::InitializeCrows(MapBG, Window)};

        // Initialize HUD
        HUD Hud{};

        // Initialize Props
        std::vector<std::vector<Prop>> UnderProps{Game::InitializePropsUnder()};
        std::vector<std::vector<Prop>> OverProps{Game::InitializePropsOver()};
        Props Props{&UnderProps, &OverProps};

        // Initialize fox Sprites for Pause Menu
        std::array<Sprite, 5> PauseFox
        {
            Sprite{"sprites/characters/fox/Fox_idle.png", 4, 4},
            Sprite{"sprites/characters/fox/Fox_walk.png", 4, 4},
            Sprite{"sprites/characters/fox/Fox_run.png", 4, 4},
            Sprite{"sprites/characters/fox/Fox_sleeping.png", 4, 1},
            Sprite{"sprites/characters/fox/Fox_melee.png", 4, 1}
        };

        // Initialize button textures for Pause Menu
        std::array<Texture2D, 9> Buttons 
        {
            LoadTexture("sprites/buttons/W.png"),
            LoadTexture("sprites/buttons/A.png"),
            LoadTexture("sprites/buttons/S.png"),
            LoadTexture("sprites/buttons/D.png"),
            LoadTexture("sprites/buttons/L.png"),
            LoadTexture("sprites/buttons/M.png"),
            LoadTexture("sprites/buttons/Shift.png"),
            LoadTexture("sprites/buttons/Space.png"),
            LoadTexture("sprites/buttons/Lmouse.png")
        };

        GameInfo GameInfo{0, 0.f, 0.f, 0.f};

        // Start Game Loop
        while (!WindowShouldClose()) 
        {
            Game::Tick(Window, MapBG, State, NextState, Champion, Props, Hud, Enemies, Crows, PauseFox, Buttons, GameInfo);
        }

        // Clean-up
        assert(GetWindowHandle());
        CloseWindow();
    }

    // Set the Game's window configurations
    void Initialize(Window& Window, int FPS, std::string Title)
    {
        assert(!GetWindowHandle());
        SetConfigFlags(FLAG_WINDOW_RESIZABLE);
        InitWindow(Window.x, Window.y, Title.c_str());
        SetTargetFPS(FPS);
        SetExitKey(0);
    }

    void Tick(Window& Window, Background& Map, GameState& State, GameState& NextState, Character& Character, Props& Props, HUD& Hud, std::vector<Enemy>& Enemies, std::vector<Enemy>& Crows, std::array<Sprite, 5>& PauseFox, std::array<Texture2D, 9>& Buttons, GameInfo& GameInfo)
    {
        float MaxTransitionTime{0.3f};
        Game::CheckScreenSizing(Window);

        BeginDrawing();

        if (State == GameState::RUNNING) {
            ClearBackground(BLACK);


            Game::Update(Map, State, NextState, Character, Props, Enemies, Crows);
            Game::Draw(Map, Character, Props, Hud, Enemies, Crows);

            if (GameInfo.TransitionOutTime < MaxTransitionTime) {
                GameInfo.TransitionInTime = GetFrameTime();
                DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, GameInfo.Opacity));
                GameInfo.Opacity -= 0.01f;
            }
            else {
                GameInfo.TransitionInTime = 0.f;
                GameInfo.Opacity = 0.f;
            }
        }
        else if (State == GameState::TRANSITION) {

            if (GameInfo.TransitionOutTime < MaxTransitionTime) {
                GameInfo.TransitionOutTime += GetFrameTime();
                DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, GameInfo.Opacity));
                GameInfo.Opacity += 0.01f;
            }
            else {
                GameInfo.TransitionOutTime = 0.f;
                GameInfo.Opacity = 1.f;
                State = NextState;
            }
        }
        else if (State == GameState::PAUSED) {

            ClearBackground(BLACK);

            Game::PauseUpdate(State, NextState, PauseFox, Buttons, GameInfo.PauseFoxIndex);
            Game::PauseDraw(PauseFox, Buttons, State, GameInfo.PauseFoxIndex);

            if (GameInfo.TransitionOutTime < MaxTransitionTime) {
                GameInfo.TransitionInTime = GetFrameTime();
                DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, GameInfo.Opacity));
                GameInfo.Opacity -= 0.01f;
            }
            else {
                GameInfo.TransitionInTime = 0.f;
                GameInfo.Opacity = 0.f;
            }
        }

        EndDrawing();
    }

    // Check if Window has been resized or fullscreened
    void CheckScreenSizing(Window& Window)
    {
        // set window.x / window.y to resized dimensions
        if (IsWindowResized()) 
        {
            Window.x = GetScreenWidth();
            Window.y = GetScreenHeight();
        }

        // check for alt + enter [FULLSCREEN]
        if (IsKeyPressed(KEY_ENTER) && (IsKeyDown(KEY_RIGHT_ALT) || IsKeyDown(KEY_LEFT_ALT)))
        {
            SetFullScreen(Window);
        }
    }

    // Full screen with current resolution settings
    void SetFullScreen(Window& Window)
    {
        // see what display we are on right now
        // int display = GetCurrentMonitor();

        if (IsWindowFullscreen()) 
        {
            // if we are full screen, then go back to the windowed size
            ToggleFullscreen();                  
            // SetWindowSize(Window.x, Window.y); 
        }
        else
        {                                        
            // if we are not full screen, set the window size to match the monitor we are on
            // SetWindowSize(GetMonitorWidth(display), GetMonitorHeight(display));
            ToggleFullscreen();
        }
    }

    // Manage Ticks for all objects
    void Update(Background& Map, GameState& State, GameState& NextState, Character& Character, Props& Props, std::vector<Enemy>& Enemies, std::vector<Enemy>& Crows)
    {
        if (State != GameState::TRANSITION) {
            // Create DeltaTime
            float DeltaTime{GetFrameTime()};

            // Call Ticks
            Map.Tick(Character.GetWorldPos());
            Character.Tick(DeltaTime, Props, Enemies);

            for (auto& Enemy:Enemies)
                Enemy.Tick(DeltaTime, Props, Character.GetWorldPos(), Character.GetCharPos(), Enemies);

            for (auto& Crow:Crows)
                Crow.Tick(DeltaTime, Props, Character.GetWorldPos(), Character.GetCharPos(), Enemies);

            for (auto& Proptype:*Props.Under)
                for (auto& Prop:Proptype)
                    Prop.Tick(DeltaTime, Map);

            for (auto& Proptype:*Props.Over)
                for (auto& Prop:Proptype)
                    Prop.Tick(DeltaTime, Map);

            // Debugging --------------------------------------
            if (Character.GetHealth() < 11)
                if (IsKeyPressed(KEY_RIGHT_BRACKET))
                    Character.AddHealth(0.5f);

            if (Character.GetHealth() > 0)
                if (IsKeyPressed(KEY_LEFT_BRACKET))
                    Character.AddHealth(-0.5f);

            if (IsKeyPressed(KEY_L))
                Character.SetSleep();

            if (IsKeyPressed(KEY_N))
                Character.SwitchCollidable();
        }

        if (IsKeyPressed(KEY_P)) {
            NextState = GameState::PAUSED;
            State = GameState::TRANSITION;
        }
    }

    // Call Draw functions for all objects
    void Draw(Background& Map, Character& Character, Props& Props, HUD& Hud, std::vector<Enemy>& Enemies, std::vector<Enemy>& Crows)
    {
        Map.Draw();

        for (auto& PropType:*Props.Under)
            for (auto& Prop:PropType) {
                Prop.Draw(Character.GetWorldPos());
                // Draw Collision Squares
                // DrawRectangle(Prop.GetCollisionRec(Character.GetWorldPos()).x,
                //               Prop.GetCollisionRec(Character.GetWorldPos()).y,
                //               Prop.GetCollisionRec(Character.GetWorldPos()).width,
                //               Prop.GetCollisionRec(Character.GetWorldPos()).height, CLITERAL(Color){ 0, 121, 241, 150 });
                // if (Prop.IsInteractable())
                //     DrawRectangle(Prop.GetInteractRec(Character.GetWorldPos()).x,
                //                   Prop.GetInteractRec(Character.GetWorldPos()).y,
                //                   Prop.GetInteractRec(Character.GetWorldPos()).width,
                //                   Prop.GetInteractRec(Character.GetWorldPos()).height, CLITERAL(Color){ 200, 122, 255, 150 });
            }

        Character.Draw();
        // Draw Collision Squares ---------------------------------
        // DrawRectangle(Character.GetCollisionRec().x,
        //               Character.GetCollisionRec().y,
        //               Character.GetCollisionRec().width,
        //               Character.GetCollisionRec().height, CLITERAL(Color){ 230, 41, 55, 150 });
        
        // Draw Attack Rectangle ----------------------------------
        // if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) || IsKeyDown(KEY_SPACE))
        // {
        //     DrawRectangle(Character.GetAttackRec().x,
        //                   Character.GetAttackRec().y,
        //                   Character.GetAttackRec().width,
        //                   Character.GetAttackRec().height, CLITERAL(Color){ 230, 41, 55, 150 });
        // }

        for (auto& Enemy:Enemies) {
            Enemy.Draw(Character.GetWorldPos());
            // Draw Collision Squares
            // DrawRectangle(Enemy.GetCollisionRec().x,
            //               Enemy.GetCollisionRec().y,
            //               Enemy.GetCollisionRec().width,
            //               Enemy.GetCollisionRec().height, CLITERAL(Color){ 230, 41, 55, 150 });
            // if (Enemy.IsAttacking()) {
            //     DrawRectangle(Enemy.GetAttackRec().x,
            //                 Enemy.GetAttackRec().y,
            //                 Enemy.GetAttackRec().width,
            //                 Enemy.GetAttackRec().height, CLITERAL(Color){ 230, 41, 55, 150 });
            // }
        }

        for (auto& PropType:*Props.Over) 
            for (auto& Prop:PropType) {
                Prop.Draw(Character.GetWorldPos());
                // Draw Collision Squares
                // DrawRectangle(Prop.GetCollisionRec(Character.GetWorldPos()).x,
                //               Prop.GetCollisionRec(Character.GetWorldPos()).y,
                //               Prop.GetCollisionRec(Character.GetWorldPos()).width,
                //               Prop.GetCollisionRec(Character.GetWorldPos()).height, CLITERAL(Color){ 200, 122, 255, 150 });
                // if (Prop.IsInteractable())
                //     DrawRectangle(Prop.GetInteractRec(Character.GetWorldPos()).x,
                //                   Prop.GetInteractRec(Character.GetWorldPos()).y,
                //                   Prop.GetInteractRec(Character.GetWorldPos()).width,
                //                   Prop.GetInteractRec(Character.GetWorldPos()).height, CLITERAL(Color){ 200, 122, 255, 150 });
            }

        for (auto& Crow:Crows) {
            Crow.Draw(Character.GetWorldPos());
        }

        // Draw ! when within an interactable entity
        Character.DrawIndicator();
        
        // Draw Portrait and Health Bars
        Hud.Draw(Character.GetHealth(), Character.GetEmotion());

        // Debugging info
        DrawText(TextFormat("Player.x: %i", (int)Character.GetWorldPos().x + 615), 20, 150, 20, WHITE);
        DrawText(TextFormat("Player.y: %i", (int)Character.GetWorldPos().y + 335), 20, 170, 20, WHITE);
        // DrawText(TextFormat("Monster Counter: %i", Enemies.at(0).GetMonsterCount(EnemyType::BEAR)), 20, 300, 20, WHITE);
        // DrawText(TextFormat("Player.HP: %i", (int)Character.GetHealth()), 20, 190, 20, WHITE);
        // DrawText(TextFormat("Enemy.x: %i", (int)Enemies.at(0).GetWorldPos().x), 20, 190, 20, WHITE);
        // DrawText(TextFormat("Enemy.y: %i", (int)Enemies.at(0).GetWorldPos().y), 20, 210, 20, WHITE);
        // DrawText(TextFormat("Blocked: %i", Enemies.at(0).IsBlocked()), 20, 230, 20, WHITE);
        // DrawText(TextFormat("Velocity: %i", (int)Vector2Length(Vector2Subtract(Character.GetCharPos(), Enemies.at(0).GetEnemyPos()))), 20, 190, 20, WHITE);
        DrawFPS(20, 223);

        Map.DrawMiniMap(Character.GetWorldPos());

    }

    // Manage Tick functions during pause menu
    void PauseUpdate(GameState& State, GameState& NextState, std::array<Sprite, 5>& PauseFox, std::array<Texture2D, 9>& Buttons, int& PauseFoxIndex)
    {
        if (State != GameState::TRANSITION) {
            if (IsKeyDown(KEY_L)) {
                PauseFoxIndex = 3;
            }
            else if (IsKeyDown(KEY_W) || IsKeyDown(KEY_A) || IsKeyDown(KEY_S) || IsKeyDown(KEY_D)) {
                if (IsKeyDown(KEY_LEFT_SHIFT)) {
                    PauseFoxIndex = 2;
                }
                else {
                    PauseFoxIndex = 1;
                }
            }
            else if (IsKeyDown(KEY_SPACE) || IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                PauseFoxIndex = 4;
            }
            else {
                PauseFoxIndex = 0;
            }

            for (auto& Fox:PauseFox) {
                Fox.Tick(GetFrameTime());
            }
        }

        if (IsKeyPressed(KEY_P)) {
            NextState = GameState::RUNNING;
            State = GameState::TRANSITION;
            
            // Add audio functionality here later

            // ++Index;
            // if (Index >= static_cast<int>(PauseFox.size())) {
            //     Index = 0;
            // }
        }
    }

    // Draw Pause sprite & button presses
    void PauseDraw(std::array<Sprite, 5>& PauseFox, std::array<Texture2D, 9>& Buttons, GameState& State, const int PauseFoxIndex)
    {
        DrawTextureEx(LoadTexture("sprites/maps/PauseBackground.png"), Vector2{0.f,0.f}, 0.0f, 4.f, WHITE);

        if (State != GameState::TRANSITION) {
            // PauseFoxIndex controls which Fox sprite is drawn
            DrawTexturePro(PauseFox.at(PauseFoxIndex).Texture, PauseFox.at(PauseFoxIndex).GetSourceRec(), PauseFox.at(PauseFoxIndex).GetPosRec(Vector2{674.f,396.f}, 4.f), Vector2{}, 0.f, WHITE);

            // Draw Buttons Depending on which are pushed
            if (IsKeyDown(KEY_W)) DrawTextureEx(Buttons.at(0), Vector2{208.f,124.f}, 0.f, 4.f, WHITE);
            if (IsKeyDown(KEY_A)) DrawTextureEx(Buttons.at(1), Vector2{160.f,180.f}, 0.f, 4.f, WHITE);
            if (IsKeyDown(KEY_S)) DrawTextureEx(Buttons.at(2), Vector2{208.f,180.f}, 0.f, 4.f, WHITE);
            if (IsKeyDown(KEY_D)) DrawTextureEx(Buttons.at(3), Vector2{256.f,180.f}, 0.f, 4.f, WHITE);
            if (IsKeyDown(KEY_L)) DrawTextureEx(Buttons.at(4), Vector2{160.f,460.f}, 0.f, 4.f, WHITE);
            if (IsKeyDown(KEY_M)) DrawTextureEx(Buttons.at(5), Vector2{160.f,372.f}, 0.f, 4.f, WHITE);
            if (IsKeyDown(KEY_LEFT_SHIFT)) DrawTextureEx(Buttons.at(6), Vector2{160.f,276.f}, 0.f, 4.f, WHITE);
            if (IsKeyDown(KEY_SPACE)) DrawTextureEx(Buttons.at(7), Vector2{152.f,552.f}, 0.f, 4.f, WHITE);
            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) DrawTextureEx(Buttons.at(8), Vector2{264.f,548.f}, 0.f, 4.f, WHITE);
        }
    }

    // Initialize props drawn under character
    std::vector<std::vector<Prop>> InitializePropsUnder()
    {
        std::vector<std::vector<Prop>> Props{};

        /*  Placeholder prop to allow "!" to turn off. Need to figure out a better solution to this later.
            Removing Placeholder will make "!" keep showing even if you walk out of an interactable range.
            It will also introduce many bugs such as getting permanantly stuck in place if space is pressed when not in an interactable range.
        */
        std::vector<Prop> Placeholder
        {
            Prop{"sprites/placeholder/placeholder.png", Vector2{0,0}, PropType::PLACEHOLDER, 4.f, false, true}
        };
        Props.emplace_back(Placeholder);

        std::vector<Prop> DungeonEntrance
        {
            Prop{"sprites/props/DungeonEntrance.png", Vector2{3520,60}, PropType::DUNGEONLEFT, 4.f},
            Prop{"sprites/props/DungeonEntrance.png", Vector2{3520,60}, PropType::DUNGEONRIGHT, 4.f},
            Prop{"sprites/props/DungeonEntrance.png", Vector2{3520,60}, PropType::DUNGEON, 4.f, false, true},
        };
        Props.emplace_back(DungeonEntrance);

        // std::vector<Prop> Altar
        // {
        //     Prop{"sprites/props/AltarDormant.png", Vector2{700,1025}, PropType::ALTAR, 4.f, false, true},
        // };
        // Props.emplace_back(Altar);

        std::vector<Prop> Grass
        {   
            // --------------------------------------------- Far Left Column -----------------------------------------------

            // ---------------------------------------------------- -4 ------------------------------------------------------
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1816,1806}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1816,1846}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1880,1806}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1880,1846}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1816,1870}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1816,1910}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1880,1870}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1880,1910}, PropType::GRASS, 4.f, true},

            // ---------------------------------------------------- -3 ------------------------------------------------------
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1816,1678}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1816,1718}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1880,1678}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1880,1718}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1816,1742}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1816,1782}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1880,1742}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1880,1782}, PropType::GRASS, 4.f, true},

            // ---------------------------------------------------- -2 ------------------------------------------------------
            // Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1816,1550}, PropType::GRASS, 4.f, true},
            // Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1816,1590}, PropType::GRASS, 4.f, true},
            
            // Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1880,1550}, PropType::GRASS, 4.f, true},
            // Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1880,1590}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1816,1614}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1816,1654}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1880,1614}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1880,1654}, PropType::GRASS, 4.f, true},

            // ---------------------------------------------------- -1 ------------------------------------------------------
            // Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1944,1294}, PropType::GRASS, 4.f, true},
            // Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1944,1334}, PropType::GRASS, 4.f, true},
            
            // Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2008,1294}, PropType::GRASS, 4.f, true},
            // Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2008,1334}, PropType::GRASS, 4.f, true},
            
            // Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1944,1358}, PropType::GRASS, 4.f, true},
            // Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1944,1398}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2008,1358}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2008,1398}, PropType::GRASS, 4.f, true},

            // ---------------------------------------------------- 0 ------------------------------------------------------
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1944,1442}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1944,1462}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2008,1442}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2008,1462}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1944,1486}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1944,1526}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2008,1486}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2008,1526}, PropType::GRASS, 4.f, true},

            // ---------------------------------------------------- 1 ------------------------------------------------------
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1944,1550}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1944,1590}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2008,1550}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2008,1590}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1944,1614}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1944,1654}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2008,1614}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2008,1654}, PropType::GRASS, 4.f, true},

            // ---------------------------------------------------- 2 ------------------------------------------------------
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1944,1678}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1944,1718}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2008,1678}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2008,1718}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1944,1742}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1944,1782}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2008,1742}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2008,1782}, PropType::GRASS, 4.f, true},

            // ---------------------------------------------------- 3 ------------------------------------------------------
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1944,1806}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1944,1846}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2008,1806}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2008,1846}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1944,1870}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1944,1910}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2008,1870}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2008,1910}, PropType::GRASS, 4.f, true},

            // ---------------------------------------------------- 3 ------------------------------------------------------
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1944,1806}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1944,1846}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2008,1806}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2008,1846}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1944,1870}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1944,1910}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2008,1870}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2008,1910}, PropType::GRASS, 4.f, true},

            // ---------------------------------------------------- 4 ------------------------------------------------------
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1944,1934}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1944,1974}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2008,1934}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2008,1974}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1944,1998}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1944,2038}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2008,1998}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2008,2038}, PropType::GRASS, 4.f, true},

            // ------------------------------------------------ Left Column ------------------------------------------------

            // ---------------------------------------------------- -1 ------------------------------------------------------
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2072,1294}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2072,1334}, PropType::GRASS, 4.f, true},
            
            // Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2136,1294}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2136,1334}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2072,1358}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2072,1398}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2136,1358}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2136,1398}, PropType::GRASS, 4.f, true},

            // ---------------------------------------------------- 0 ------------------------------------------------------
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2072,1422}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2072,1462}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2136,1422}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2136,1462}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2072,1486}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2072,1526}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2136,1486}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2136,1526}, PropType::GRASS, 4.f, true},

            // ---------------------------------------------------- 1 ------------------------------------------------------
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2072,1550}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2072,1590}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2136,1550}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2136,1590}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2072,1614}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2072,1654}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2136,1614}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2136,1654}, PropType::GRASS, 4.f, true},

            // --------------------------------------------------- 2 -------------------------------------------------------
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2072,1678}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2072,1718}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2136,1678}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2136,1718}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2072,1742}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2072,1782}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2136,1742}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2136,1782}, PropType::GRASS, 4.f, true},

            // --------------------------------------------------- 3 -------------------------------------------------------
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2072,1806}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2072,1846}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2136,1806}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2136,1846}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2072,1870}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2072,1910}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2136,1870}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2136,1910}, PropType::GRASS, 4.f, true},

            // --------------------------------------------------- 4 -------------------------------------------------------
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2072,1934}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2072,1974}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2136,1934}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2136,1974}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2072,1998}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2072,2038}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2136,1998}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2136,2038}, PropType::GRASS, 4.f, true},



            // --------------------------------------------- Far Right Column ----------------------------------------------

            // --------------------------------------------------- -6 -------------------------------------------------------
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2456,1934}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2456,1974}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2520,1934}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2520,1974}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2456,1998}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2456,2038}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2520,1998}, PropType::GRASS, 4.f, true},
            // Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2520,2038}, PropType::GRASS, 4.f, true},

            // --------------------------------------------------- -5 -------------------------------------------------------
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2456,1806}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2456,1846}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2520,1806}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2520,1846}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2456,1870}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2456,1910}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2520,1870}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2520,1910}, PropType::GRASS, 4.f, true},

            // --------------------------------------------------- -4 -------------------------------------------------------
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2456,1678}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2456,1718}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2520,1678}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2520,1718}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2456,1742}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2456,1782}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2520,1742}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2520,1782}, PropType::GRASS, 4.f, true},

            // --------------------------------------------------- -3 -------------------------------------------------------
            // Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2456,1550}, PropType::GRASS, 4.f, true},
            // Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2456,1590}, PropType::GRASS, 4.f, true},
            
            // Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2520,1550}, PropType::GRASS, 4.f, true},
            // Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2520,1590}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2456,1614}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2456,1654}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2520,1614}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2520,1654}, PropType::GRASS, 4.f, true},

            // --------------------------------------------------- -2 -------------------------------------------------------
            // Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2456,1422}, PropType::GRASS, 4.f, true},
            // Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2456,1462}, PropType::GRASS, 4.f, true},
            
            // Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2520,1422}, PropType::GRASS, 4.f, true},
            // Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2520,1462}, PropType::GRASS, 4.f, true},
            
            // Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2456,1486}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2456,1526}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2520,1486}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2520,1526}, PropType::GRASS, 4.f, true},

            // --------------------------------------------------- -1 -------------------------------------------------------
            // Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2328,1294}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2328,1334}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2392,1294}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2392,1334}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2328,1358}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2328,1398}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2392,1358}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2392,1398}, PropType::GRASS, 4.f, true},

            // --------------------------------------------------- 0 -------------------------------------------------------
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2328,1422}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2328,1462}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2392,1422}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2392,1462}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2328,1486}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2328,1526}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2392,1486}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2392,1526}, PropType::GRASS, 4.f, true},
            
            // --------------------------------------------------- 1 -------------------------------------------------------
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2328,1550}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2328,1590}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2392,1550}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2392,1590}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2328,1614}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2328,1654}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2392,1614}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2392,1654}, PropType::GRASS, 4.f, true},

            // --------------------------------------------------- 2 -------------------------------------------------------
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2328,1678}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2328,1718}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2392,1678}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2392,1718}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2328,1742}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2328,1782}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2392,1742}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2392,1782}, PropType::GRASS, 4.f, true},
            
            // --------------------------------------------------- 3 -------------------------------------------------------
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2328,1806}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2328,1846}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2392,1806}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2392,1846}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2328,1870}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2328,1910}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2392,1870}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2392,1910}, PropType::GRASS, 4.f, true},

            // --------------------------------------------------- 4 -------------------------------------------------------
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2328,1934}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2328,1974}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2392,1934}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2392,1974}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2328,1998}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2328,2038}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2392,1998}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2392,2038}, PropType::GRASS, 4.f, true},

            // ------------------------------------------------ Right Column -----------------------------------------------

            // --------------------------------------------------- -1 -------------------------------------------------------
            // Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2200,1294}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2200,1334}, PropType::GRASS, 4.f, true},
            
            // Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2264,1294}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2264,1334}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2200,1358}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2200,1398}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2264,1358}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2264,1398}, PropType::GRASS, 4.f, true},

            // --------------------------------------------------- 0 -------------------------------------------------------
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2200,1422}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2200,1462}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2264,1422}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2264,1462}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2200,1486}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2200,1526}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2264,1486}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2264,1526}, PropType::GRASS, 4.f, true},


            // --------------------------------------------------- 1 -------------------------------------------------------
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2200,1550}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2200,1590}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2264,1550}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2264,1590}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2200,1614}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2200,1654}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2264,1614}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2264,1654}, PropType::GRASS, 4.f, true},

            // --------------------------------------------------- 2 -------------------------------------------------------
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2200,1678}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2200,1718}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2264,1678}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2264,1718}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2200,1742}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2200,1782}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2264,1742}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2264,1782}, PropType::GRASS, 4.f, true},

            // --------------------------------------------------- 3 -------------------------------------------------------
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2200,1806}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2200,1846}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2264,1806}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2264,1846}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2200,1870}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2200,1910}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2264,1870}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2264,1910}, PropType::GRASS, 4.f, true},

            // --------------------------------------------------- 4 -------------------------------------------------------
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2200,1934}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2200,1974}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2264,1934}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2264,1974}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2200,1998}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2200,2038}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2264,1998}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2264,2038}, PropType::GRASS, 4.f, true},

            // --------------------------------------------------------------------------------------------------------------
            // -----------------------------------------Dungeon Entrance Grass-----------------------------------------------
            // --------------------------------------------------------------------------------------------------------------

            // ------------------------------------------------ Left Column -------------------------------------------------
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3500,170}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3500,210}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3564,170}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3564,210}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3500,234}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3500,274}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3564,234}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3564,274}, PropType::GRASS, 4.f, true},

            // --------------------------------------------------------------------------------------------------------------

            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3500,298}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3500,338}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3564,298}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3564,338}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3500,362}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3500,402}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3564,362}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3564,402}, PropType::GRASS, 4.f, true},

            // --------------------------------------------------------------------------------------------------------------
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3500,426}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3500,446}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3564,426}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3564,446}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3500,490}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3500,530}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3564,490}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3564,530}, PropType::GRASS, 4.f, true},

            // --------------------------------------------------------------------------------------------------------------
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3500,554}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3500,594}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3564,554}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3564,594}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3500,618}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3500,658}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3564,618}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3564,658}, PropType::GRASS, 4.f, true},

            // --------------------------------------------------------------------------------------------------------------
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3500,682}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3500,722}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3564,682}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3564,722}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3500,746}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3500,786}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3564,746}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3564,786}, PropType::GRASS, 4.f, true},

            // --------------------------------------------------------------------------------------------------------------
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3500,810}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3500,850}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3564,810}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3564,850}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3500,874}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3500,914}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3564,874}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3564,914}, PropType::GRASS, 4.f, true},

            // --------------------------------------------------------------------------------------------------------------
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3500,938}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3500,978}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3564,938}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3564,978}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3500,1002}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3500,1042}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3564,1002}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3564,1042}, PropType::GRASS, 4.f, true},

            // --------------------------------------------------------------------------------------------------------------
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3500,1066}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3500,1106}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3564,1066}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3564,1106}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3500,1130}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3500,1170}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3564,1130}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3564,1170}, PropType::GRASS, 4.f, true},

            // --------------------------------------------------------------------------------------------------------------
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3500,1194}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3500,1234}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3564,1194}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3564,1234}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3500,1258}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3500,1298}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3564,1258}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3564,1298}, PropType::GRASS, 4.f, true},

            // --------------------------------------------------------------------------------------------------------------
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3500,1322}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3500,1362}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3564,1322}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3564,1362}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3500,1386}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3500,1426}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3564,1386}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3564,1426}, PropType::GRASS, 4.f, true},
            
            // --------------------------------------------------------------------------------------------------------------
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3500,1450}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3500,1490}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3564,1450}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3564,1490}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3500,1514}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3500,1554}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3564,1514}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3564,1554}, PropType::GRASS, 4.f, true},

            // --------------------------------------------------------------------------------------------------------------
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3500,1578}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3500,1618}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3564,1578}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3564,1618}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3500,1642}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3500,1682}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3564,1642}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3564,1682}, PropType::GRASS, 4.f, true},

            // --------------------------------------------------------------------------------------------------------------
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3500,1706}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3500,1746}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3564,1706}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3564,1746}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3500,1770}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3500,1810}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3564,1770}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3564,1810}, PropType::GRASS, 4.f, true},

            // --------------------------------------------------------------------------------------------------------------
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3500,1834}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3500,1874}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3564,1834}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3564,1874}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3500,1898}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3500,1938}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3564,1898}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3564,1938}, PropType::GRASS, 4.f, true},

            // --------------------------------------------------------------------------------------------------------------
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3500,1962}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3500,2002}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3564,1962}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3564,2002}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3500,2026}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3500,2046}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3564,2026}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3564,2046}, PropType::GRASS, 4.f, true},

            // ------------------------------------------------ Right Column -------------------------------------------------
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3628,170}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3628,210}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3692,170}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3692,210}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3628,234}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3628,274}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3692,234}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3692,274}, PropType::GRASS, 4.f, true},

            // --------------------------------------------------------------------------------------------------------------

            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3628,298}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3628,338}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3692,298}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3692,338}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3628,362}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3628,402}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3692,362}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3692,402}, PropType::GRASS, 4.f, true},

            // --------------------------------------------------------------------------------------------------------------
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3628,426}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3628,446}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3692,426}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3692,446}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3628,490}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3628,530}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3692,490}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3692,530}, PropType::GRASS, 4.f, true},

            // --------------------------------------------------------------------------------------------------------------
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3628,554}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3628,594}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3692,554}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3692,594}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3628,618}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3628,658}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3692,618}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3692,658}, PropType::GRASS, 4.f, true},

            // --------------------------------------------------------------------------------------------------------------
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3628,682}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3628,722}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3692,682}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3692,722}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3628,746}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3628,786}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3692,746}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3692,786}, PropType::GRASS, 4.f, true},

            // --------------------------------------------------------------------------------------------------------------
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3628,810}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3628,850}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3692,810}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3692,850}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3628,874}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3628,914}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3692,874}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3692,914}, PropType::GRASS, 4.f, true},

            // --------------------------------------------------------------------------------------------------------------
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3628,938}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3628,978}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3692,938}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3692,978}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3628,1002}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3628,1042}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3692,1002}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3692,1042}, PropType::GRASS, 4.f, true},

            // --------------------------------------------------------------------------------------------------------------
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3628,1066}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3628,1106}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3692,1066}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3692,1106}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3628,1130}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3628,1170}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3692,1130}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3692,1170}, PropType::GRASS, 4.f, true},

            // --------------------------------------------------------------------------------------------------------------
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3628,1194}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3628,1234}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3692,1194}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3692,1234}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3628,1258}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3628,1298}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3692,1258}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3692,1298}, PropType::GRASS, 4.f, true},

            // --------------------------------------------------------------------------------------------------------------
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3628,1322}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3628,1362}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3692,1322}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3692,1362}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3628,1386}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3628,1426}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3692,1386}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3692,1426}, PropType::GRASS, 4.f, true},
            
            // --------------------------------------------------------------------------------------------------------------
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3628,1450}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3628,1490}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3692,1450}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3692,1490}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3628,1514}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3628,1554}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3692,1514}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3692,1554}, PropType::GRASS, 4.f, true},

            // --------------------------------------------------------------------------------------------------------------
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3628,1578}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3628,1618}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3692,1578}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3692,1618}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3628,1642}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3628,1682}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3692,1642}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3692,1682}, PropType::GRASS, 4.f, true},

            // --------------------------------------------------------------------------------------------------------------
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3628,1706}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3628,1746}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3692,1706}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3692,1746}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3628,1770}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3628,1810}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3692,1770}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3692,1810}, PropType::GRASS, 4.f, true},

            // --------------------------------------------------------------------------------------------------------------
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3628,1834}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3628,1874}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3692,1834}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3692,1874}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3628,1898}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3628,1938}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3692,1898}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3692,1938}, PropType::GRASS, 4.f, true},

            // --------------------------------------------------------------------------------------------------------------
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3628,1962}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3628,2002}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3692,1962}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3692,2002}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3628,2026}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3628,2046}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3692,2026}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3692,2046}, PropType::GRASS, 4.f, true},
            
            
        };
        Props.emplace_back(Grass);

        std::vector<Prop> Walls
        {
            // ---------------------------------------- Left Side Hill -----------------------------------

            // Left Side Walls
            Prop{"sprites/props/GrassWallLeft.png", Vector2{768, 2688}, PropType::LEFTSIDEWALL},
            Prop{"sprites/props/GrassWallLeft.png", Vector2{768, 2752}, PropType::LEFTSIDEWALL},
            Prop{"sprites/props/GrassWallLeft.png", Vector2{768, 2816}, PropType::LEFTSIDEWALL},
            Prop{"sprites/props/GrassWallLeft.png", Vector2{768, 2880}, PropType::LEFTSIDEWALL},
            Prop{"sprites/props/GrassWallLeft.png", Vector2{768, 2944}, PropType::LEFTSIDEWALL},
            Prop{"sprites/props/GrassWallLeft.png", Vector2{768, 3008}, PropType::LEFTSIDEWALL},
            Prop{"sprites/props/GrassWallLeft.png", Vector2{768, 3072}, PropType::LEFTSIDEWALL},
            Prop{"sprites/props/GrassWallLeft.png", Vector2{768, 3136}, PropType::LEFTSIDEWALL},
            Prop{"sprites/props/GrassWallLeft.png", Vector2{768, 3200}, PropType::LEFTSIDEWALL},
            Prop{"sprites/props/GrassWallLeft.png", Vector2{768, 3264}, PropType::LEFTSIDEWALL},
            Prop{"sprites/props/GrassWallLeft.png", Vector2{768, 3328}, PropType::LEFTSIDEWALL},
            Prop{"sprites/props/GrassWallLeft.png", Vector2{768, 3392}, PropType::LEFTSIDEWALL},
            Prop{"sprites/props/GrassWallLeft.png", Vector2{768, 3456}, PropType::LEFTSIDEWALL},
            Prop{"sprites/props/GrassWallLeft.png", Vector2{768, 3520}, PropType::LEFTSIDEWALL},
            Prop{"sprites/props/GrassWallBotLeft.png", Vector2{768, 3584}, PropType::BOTTOMWALL},
            
            // Bottom Walls
            Prop{"sprites/props/GrassWallBottom.png", Vector2{832, 3584}, PropType::BOTTOMWALL},
            Prop{"sprites/props/GrassWallBottom.png", Vector2{896, 3584}, PropType::BOTTOMWALL},
            Prop{"sprites/props/GrassWallBottom.png", Vector2{960, 3584}, PropType::BOTTOMWALL},
            Prop{"sprites/props/GrassWallBottom.png", Vector2{1024, 3584}, PropType::BOTTOMWALL},
            Prop{"sprites/props/GrassWallBottom.png", Vector2{1216, 3584}, PropType::BOTTOMWALL},
            Prop{"sprites/props/GrassWallBotRight.png", Vector2{1280, 3584}, PropType::BOTTOMWALL},
        
            // Bottom Right Walls
            Prop{"sprites/props/GrassWallRight.png", Vector2{1280, 3520}, PropType::RIGHTSIDEWALL},
            Prop{"sprites/props/GrassWallRight.png", Vector2{1280, 3456}, PropType::RIGHTSIDEWALL},
            Prop{"sprites/props/GrassWallRight.png", Vector2{1280, 3392}, PropType::RIGHTSIDEWALL},
            Prop{"sprites/props/GrassWallRight.png", Vector2{1280, 3328}, PropType::RIGHTSIDEWALL},
            Prop{"sprites/props/GrassWallInnerRight.png", Vector2{1280, 3264}, PropType::RIGHTSIDEWALL},
            
            // Bottom Walls
            Prop{"sprites/props/GrassWallBottom.png", Vector2{1344, 3264}, PropType::BOTTOMWALL},
            Prop{"sprites/props/GrassWallBottom.png", Vector2{1408, 3264}, PropType::BOTTOMWALL},
            Prop{"sprites/props/GrassWallBottom.png", Vector2{1472, 3264}, PropType::BOTTOMWALL},
            Prop{"sprites/props/GrassWallBottom.png", Vector2{1536, 3264}, PropType::BOTTOMWALL},
            Prop{"sprites/props/GrassWallBottom.png", Vector2{1600, 3264}, PropType::BOTTOMWALL},
            Prop{"sprites/props/GrassWallBottom.png", Vector2{1664, 3264}, PropType::BOTTOMWALL},
            Prop{"sprites/props/GrassWallBottom.png", Vector2{1728, 3264}, PropType::BOTTOMWALL},
            Prop{"sprites/props/GrassWallBottom.png", Vector2{1792, 3264}, PropType::BOTTOMWALL},
            Prop{"sprites/props/GrassWallBottom.png", Vector2{1856, 3264}, PropType::BOTTOMWALL},
            Prop{"sprites/props/GrassWallBotRight.png", Vector2{1920, 3264}, PropType::BOTTOMWALL},     

            // Right Side Walls
            Prop{"sprites/props/GrassWallRight.png", Vector2{1920, 3200}, PropType::RIGHTSIDEWALL},
            Prop{"sprites/props/GrassWallRight.png", Vector2{1920, 3136}, PropType::RIGHTSIDEWALL},
            Prop{"sprites/props/GrassWallRight.png", Vector2{1920, 3072}, PropType::RIGHTSIDEWALL},
            Prop{"sprites/props/GrassWallRight.png", Vector2{1920, 3008}, PropType::RIGHTSIDEWALL},
            Prop{"sprites/props/GrassWallRight.png", Vector2{1920, 2944}, PropType::RIGHTSIDEWALL},
            Prop{"sprites/props/GrassWallRight.png", Vector2{1920, 2880}, PropType::RIGHTSIDEWALL},
            Prop{"sprites/props/GrassWallRight.png", Vector2{1920, 2816}, PropType::RIGHTSIDEWALL},
            Prop{"sprites/props/GrassWallRight.png", Vector2{1920, 2752}, PropType::RIGHTSIDEWALL},
            Prop{"sprites/props/GrassWallRight.png", Vector2{1920, 2688}, PropType::RIGHTSIDEWALL},
            Prop{"sprites/props/GrassWallTopRight.png", Vector2{1920, 2624}, PropType::TOPWALL},

            // Top Walls
            Prop{"sprites/props/GrassWallTop.png", Vector2{1856, 2624}, PropType::TOPWALL},
            Prop{"sprites/props/GrassWallTop.png", Vector2{1792, 2624}, PropType::TOPWALL},
            Prop{"sprites/props/GrassWallTop.png", Vector2{1728, 2624}, PropType::TOPWALL},
            Prop{"sprites/props/GrassWallTop.png", Vector2{1664, 2624}, PropType::TOPWALL},
            Prop{"sprites/props/GrassWallTop.png", Vector2{1600, 2624}, PropType::TOPWALL},
            Prop{"sprites/props/GrassWallTop.png", Vector2{1536, 2624}, PropType::TOPWALL},
            Prop{"sprites/props/GrassWallTop.png", Vector2{1472, 2624}, PropType::TOPWALL},
            Prop{"sprites/props/GrassWallTop.png", Vector2{1408, 2624}, PropType::TOPWALL},
            Prop{"sprites/props/GrassWallTop.png", Vector2{1344, 2624}, PropType::TOPWALL},
            Prop{"sprites/props/GrassWallTop.png", Vector2{1280, 2624}, PropType::TOPWALL},
            Prop{"sprites/props/GrassWallTop.png", Vector2{1216, 2624}, PropType::TOPWALL},
            Prop{"sprites/props/GrassWallTop.png", Vector2{1152, 2624}, PropType::TOPWALL},
            Prop{"sprites/props/GrassWallTop.png", Vector2{1088, 2624}, PropType::TOPWALL},
            Prop{"sprites/props/GrassWallTop.png", Vector2{1024, 2624}, PropType::TOPWALL},
            Prop{"sprites/props/GrassWallTop.png", Vector2{960, 2624}, PropType::TOPWALL},
            Prop{"sprites/props/GrassWallTop.png", Vector2{896, 2624}, PropType::TOPWALL},
            Prop{"sprites/props/GrassWallTop.png", Vector2{832, 2624}, PropType::TOPWALL},
            Prop{"sprites/props/GrassWallTopLeft.png", Vector2{768, 2624}, PropType::TOPWALL},

            // Fences
            Prop{"sprites/props/FenceDown.png", Vector2{832, 3662}, PropType::FENCE},
            Prop{"sprites/props/FenceDown.png", Vector2{896, 3662}, PropType::FENCE},
            Prop{"sprites/props/FenceDown.png", Vector2{960, 3662}, PropType::FENCE},
            Prop{"sprites/props/FenceDown.png", Vector2{1024, 3662}, PropType::FENCE},
            Prop{"sprites/props/FenceDown.png", Vector2{1216, 3662}, PropType::FENCE},

            // ---------------------------------------- Garden Fences -----------------------------------

            // Left Side
            Prop{"sprites/props/FenceLeft.png", Vector2{1688, 3070}, PropType::FENCE},
            Prop{"sprites/props/FenceLeft.png", Vector2{1688, 3198}, PropType::FENCE},

            // Right Side
            Prop{"sprites/props/FenceRight.png", Vector2{1925, 3070}, PropType::FENCE},
            Prop{"sprites/props/FenceRight.png", Vector2{1925, 3128}, PropType::FENCE},
            Prop{"sprites/props/FenceRight.png", Vector2{1925, 3198}, PropType::FENCE},

            // Top Side
            Prop{"sprites/props/FenceUp.png", Vector2{1728, 3070}, PropType::FENCE},
            Prop{"sprites/props/FenceUp.png", Vector2{1856, 3070}, PropType::FENCE},
            
            // Bottom Side
            Prop{"sprites/props/FenceDown.png", Vector2{1728, 3215}, PropType::FENCE},
            Prop{"sprites/props/FenceDown.png", Vector2{1792, 3215}, PropType::FENCE},
            Prop{"sprites/props/FenceDown.png", Vector2{1856, 3215}, PropType::FENCE},


            // ---------------------------------------- Left Side Inner Hill -----------------------------------
            
            // Holes
            Prop{"sprites/props/Hole.png", Vector2{960, 3008}, PropType::HOLE},
            Prop{"sprites/props/Hole.png", Vector2{1088, 3008}, PropType::HOLE},


            // Left Side Walls
            Prop{"sprites/props/WallLeft.png", Vector2{896, 2816}, PropType::LEFTSIDEWALL},
            Prop{"sprites/props/WallLeft.png", Vector2{896, 2880}, PropType::LEFTSIDEWALL},
            Prop{"sprites/props/WallLeft.png", Vector2{896, 2944}, PropType::LEFTSIDEWALL},
            Prop{"sprites/props/WallLeft.png", Vector2{896, 3008}, PropType::LEFTSIDEWALL},
            Prop{"sprites/props/WallLeft.png", Vector2{896, 3072}, PropType::LEFTSIDEWALL},
            Prop{"sprites/props/WallLeft.png", Vector2{896, 3136}, PropType::LEFTSIDEWALL},
            Prop{"sprites/props/WallLeft.png", Vector2{896, 3200}, PropType::LEFTSIDEWALL},
            Prop{"sprites/props/WallBotLeft.png", Vector2{896, 3264}, PropType::BOTTOMWALL},

            // Bottom Walls
            Prop{"sprites/props/WallBottom.png", Vector2{960, 3264}, PropType::BOTTOMWALL},
            Prop{"sprites/props/WallBottom.png", Vector2{1024, 3264}, PropType::BOTTOMWALL},
            Prop{"sprites/props/WallBotRight.png", Vector2{1152, 3264}, PropType::BOTTOMWALL},

            // Right Side Walls
            Prop{"sprites/props/WallRight.png", Vector2{1152, 3200}, PropType::RIGHTSIDEWALL},
            Prop{"sprites/props/WallRight.png", Vector2{1152, 3136}, PropType::RIGHTSIDEWALL},
            Prop{"sprites/props/WallRight.png", Vector2{1152, 3072}, PropType::RIGHTSIDEWALL},
            Prop{"sprites/props/WallRight.png", Vector2{1152, 3008}, PropType::RIGHTSIDEWALL},
            Prop{"sprites/props/WallRight.png", Vector2{1152, 2944}, PropType::RIGHTSIDEWALL},
            Prop{"sprites/props/WallRight.png", Vector2{1152, 2880}, PropType::RIGHTSIDEWALL},
            Prop{"sprites/props/WallRight.png", Vector2{1152, 2816}, PropType::RIGHTSIDEWALL},
            Prop{"sprites/props/WallTopRight.png", Vector2{1152, 2752}, PropType::TOPWALL},

            // Top Walls
            Prop{"sprites/props/GrassWallTop.png", Vector2{1088, 2752}, PropType::TOPWALL},
            Prop{"sprites/props/GrassWallTop.png", Vector2{1024, 2752}, PropType::TOPWALL},
            Prop{"sprites/props/GrassWallTop.png", Vector2{960, 2752}, PropType::TOPWALL},
            Prop{"sprites/props/WallTopLeft.png", Vector2{896, 2752}, PropType::TOPWALL},

            // ---------------------------------------- Right Side Hill -----------------------------------

            // Left Side Walls
            Prop{"sprites/props/GrassWallLeft.png", Vector2{2688, 2560}, PropType::LEFTSIDEWALL},
            Prop{"sprites/props/GrassWallLeft.png", Vector2{2688, 2624}, PropType::LEFTSIDEWALL},
            Prop{"sprites/props/GrassWallLeft.png", Vector2{2688, 2688}, PropType::LEFTSIDEWALL},
            Prop{"sprites/props/GrassWallLeft.png", Vector2{2688, 2752}, PropType::LEFTSIDEWALL},
            Prop{"sprites/props/GrassWallLeft.png", Vector2{2688, 2816}, PropType::LEFTSIDEWALL},
            Prop{"sprites/props/GrassWallLeft.png", Vector2{2688, 2880}, PropType::LEFTSIDEWALL},
            Prop{"sprites/props/GrassWallLeft.png", Vector2{2688, 2944}, PropType::LEFTSIDEWALL},
            Prop{"sprites/props/GrassWallLeft.png", Vector2{2688, 3008}, PropType::LEFTSIDEWALL},
            Prop{"sprites/props/GrassWallLeft.png", Vector2{2688, 3072}, PropType::LEFTSIDEWALL},
            Prop{"sprites/props/GrassWallLeft.png", Vector2{2688, 3136}, PropType::LEFTSIDEWALL},
            Prop{"sprites/props/GrassWallLeft.png", Vector2{2688, 3200}, PropType::LEFTSIDEWALL},
            Prop{"sprites/props/GrassWallLeft.png", Vector2{2688, 3264}, PropType::LEFTSIDEWALL},
            Prop{"sprites/props/GrassWallLeft.png", Vector2{2688, 3328}, PropType::LEFTSIDEWALL},
            Prop{"sprites/props/GrassWallLeft.png", Vector2{2688, 3392}, PropType::LEFTSIDEWALL},
            Prop{"sprites/props/GrassWallBotLeft.png", Vector2{2688, 3456}, PropType::BOTTOMWALL},

            // Bottom Walls
            Prop{"sprites/props/GrassWallBottom.png", Vector2{2752, 3456}, PropType::BOTTOMWALL},
            Prop{"sprites/props/GrassWallBottom.png", Vector2{2816, 3456}, PropType::BOTTOMWALL},
            Prop{"sprites/props/GrassWallBottom.png", Vector2{2880, 3456}, PropType::BOTTOMWALL},
            Prop{"sprites/props/GrassWallBottom.png", Vector2{3264, 3456}, PropType::BOTTOMWALL},
            Prop{"sprites/props/GrassWallBottom.png", Vector2{3328, 3456}, PropType::BOTTOMWALL},
            Prop{"sprites/props/GrassWallBottom.png", Vector2{3392, 3456}, PropType::BOTTOMWALL},
            Prop{"sprites/props/GrassWallBottom.png", Vector2{3456, 3456}, PropType::BOTTOMWALL},
            Prop{"sprites/props/GrassWallBottom.png", Vector2{3520, 3456}, PropType::BOTTOMWALL},
            Prop{"sprites/props/GrassWallBottom.png", Vector2{3584, 3456}, PropType::BOTTOMWALL},
            Prop{"sprites/props/GrassWallBottom.png", Vector2{3648, 3456}, PropType::BOTTOMWALL},
            Prop{"sprites/props/GrassWallBottom.png", Vector2{3712, 3456}, PropType::BOTTOMWALL},
            Prop{"sprites/props/GrassWallBottom.png", Vector2{3776, 3456}, PropType::BOTTOMWALL},
            Prop{"sprites/props/GrassWallBotRight.png", Vector2{3840, 3456}, PropType::BOTTOMWALL},

            // Right Side Walls
            Prop{"sprites/props/GrassWallRight.png", Vector2{3840, 3392}, PropType::RIGHTSIDEWALL},
            Prop{"sprites/props/GrassWallRight.png", Vector2{3840, 3328}, PropType::RIGHTSIDEWALL},
            Prop{"sprites/props/GrassWallRight.png", Vector2{3840, 3264}, PropType::RIGHTSIDEWALL},
            Prop{"sprites/props/GrassWallRight.png", Vector2{3840, 3200}, PropType::RIGHTSIDEWALL},
            Prop{"sprites/props/GrassWallRight.png", Vector2{3840, 3136}, PropType::RIGHTSIDEWALL},
            Prop{"sprites/props/GrassWallRight.png", Vector2{3840, 3072}, PropType::RIGHTSIDEWALL},
            Prop{"sprites/props/GrassWallRight.png", Vector2{3840, 3008}, PropType::RIGHTSIDEWALL},
            Prop{"sprites/props/GrassWallRight.png", Vector2{3840, 2944}, PropType::RIGHTSIDEWALL},
            Prop{"sprites/props/GrassWallRight.png", Vector2{3840, 2880}, PropType::RIGHTSIDEWALL},
            Prop{"sprites/props/GrassWallRight.png", Vector2{3840, 2816}, PropType::RIGHTSIDEWALL},
            Prop{"sprites/props/GrassWallRight.png", Vector2{3840, 2752}, PropType::RIGHTSIDEWALL},
            Prop{"sprites/props/GrassWallRight.png", Vector2{3840, 2688}, PropType::RIGHTSIDEWALL},
            Prop{"sprites/props/GrassWallRight.png", Vector2{3840, 2624}, PropType::RIGHTSIDEWALL},
            Prop{"sprites/props/GrassWallRight.png", Vector2{3840, 2560}, PropType::RIGHTSIDEWALL},
            Prop{"sprites/props/GrassWallTopRight.png", Vector2{3840, 2496}, PropType::TOPWALL},

            // Top Side Walls
            Prop{"sprites/props/GrassWallTop.png", Vector2{3776, 2496}, PropType::TOPWALL},
            Prop{"sprites/props/GrassWallTop.png", Vector2{3712, 2496}, PropType::TOPWALL},
            Prop{"sprites/props/GrassWallTop.png", Vector2{3648, 2496}, PropType::TOPWALL},
            Prop{"sprites/props/GrassWallTop.png", Vector2{3584, 2496}, PropType::TOPWALL},
            Prop{"sprites/props/GrassWallTop.png", Vector2{3520, 2496}, PropType::TOPWALL},
            Prop{"sprites/props/GrassWallTop.png", Vector2{3456, 2496}, PropType::TOPWALL},
            Prop{"sprites/props/GrassWallTop.png", Vector2{3392, 2496}, PropType::TOPWALL},
            Prop{"sprites/props/GrassWallTop.png", Vector2{3328, 2496}, PropType::TOPWALL},
            Prop{"sprites/props/GrassWallTop.png", Vector2{3264, 2496}, PropType::TOPWALL},
            Prop{"sprites/props/GrassWallTop.png", Vector2{3200, 2496}, PropType::TOPWALL},
            Prop{"sprites/props/GrassWallTop.png", Vector2{3136, 2496}, PropType::TOPWALL},
            Prop{"sprites/props/GrassWallTop.png", Vector2{3072, 2496}, PropType::TOPWALL},
            Prop{"sprites/props/GrassWallTop.png", Vector2{3008, 2496}, PropType::TOPWALL},
            Prop{"sprites/props/GrassWallTop.png", Vector2{2944, 2496}, PropType::TOPWALL},
            Prop{"sprites/props/GrassWallTop.png", Vector2{2880, 2496}, PropType::TOPWALL},
            Prop{"sprites/props/GrassWallTop.png", Vector2{2816, 2496}, PropType::TOPWALL},
            Prop{"sprites/props/GrassWallTop.png", Vector2{2752, 2496}, PropType::TOPWALL},
            Prop{"sprites/props/GrassWallTopLeft.png", Vector2{2688, 2496}, PropType::TOPWALL},


            // ---------------------------------------- Right Side Second Hill -----------------------------------
            
            // Left Side Walls
            Prop{"sprites/props/GrassWallLeft.png", Vector2{2752, 2624}, PropType::LEFTSIDEWALL},
            Prop{"sprites/props/GrassWallLeft.png", Vector2{2752, 2688}, PropType::LEFTSIDEWALL},
            Prop{"sprites/props/GrassWallLeft.png", Vector2{2752, 2752}, PropType::LEFTSIDEWALL},
            Prop{"sprites/props/GrassWallLeft.png", Vector2{2752, 2816}, PropType::LEFTSIDEWALL},
            Prop{"sprites/props/GrassWallLeft.png", Vector2{2752, 2880}, PropType::LEFTSIDEWALL},
            Prop{"sprites/props/GrassWallLeft.png", Vector2{2752, 2944}, PropType::LEFTSIDEWALL},
            Prop{"sprites/props/GrassWallLeft.png", Vector2{2752, 3008}, PropType::LEFTSIDEWALL},
            Prop{"sprites/props/GrassWallLeft.png", Vector2{2752, 3072}, PropType::LEFTSIDEWALL},
            Prop{"sprites/props/GrassWallLeft.png", Vector2{2752, 3136}, PropType::LEFTSIDEWALL},
            Prop{"sprites/props/GrassWallLeft.png", Vector2{2752, 3200}, PropType::LEFTSIDEWALL},
            Prop{"sprites/props/GrassWallBotLeft.png", Vector2{2752, 3264}, PropType::BOTTOMWALL},

            // Bottom Walls
            Prop{"sprites/props/GrassWallBottom.png", Vector2{2816, 3264}, PropType::BOTTOMWALL},
            Prop{"sprites/props/GrassWallBottom.png", Vector2{2880, 3264}, PropType::BOTTOMWALL},
            Prop{"sprites/props/GrassWallBottom.png", Vector2{2944, 3264}, PropType::BOTTOMWALL},
            Prop{"sprites/props/GrassWallBottom.png", Vector2{3136, 3264}, PropType::BOTTOMWALL},
            Prop{"sprites/props/GrassWallBottom.png", Vector2{3200, 3264}, PropType::BOTTOMWALL},
            Prop{"sprites/props/GrassWallBottom.png", Vector2{3264, 3264}, PropType::BOTTOMWALL},
            Prop{"sprites/props/GrassWallBottom.png", Vector2{3328, 3264}, PropType::BOTTOMWALL},
            Prop{"sprites/props/GrassWallBottom.png", Vector2{3392, 3264}, PropType::BOTTOMWALL},
            Prop{"sprites/props/GrassWallBottom.png", Vector2{3456, 3264}, PropType::BOTTOMWALL},
            Prop{"sprites/props/GrassWallBottom.png", Vector2{3520, 3264}, PropType::BOTTOMWALL},
            Prop{"sprites/props/GrassWallBottom.png", Vector2{3584, 3264}, PropType::BOTTOMWALL},
            Prop{"sprites/props/GrassWallBottom.png", Vector2{3648, 3264}, PropType::BOTTOMWALL},
            Prop{"sprites/props/GrassWallBotRight.png", Vector2{3712, 3264}, PropType::BOTTOMWALL},

            // Right Side Walls
            Prop{"sprites/props/GrassWallRight.png", Vector2{3712, 3200}, PropType::RIGHTSIDEWALL},
            Prop{"sprites/props/GrassWallRight.png", Vector2{3712, 3136}, PropType::RIGHTSIDEWALL},
            Prop{"sprites/props/GrassWallRight.png", Vector2{3712, 3072}, PropType::RIGHTSIDEWALL},
            Prop{"sprites/props/GrassWallRight.png", Vector2{3712, 3008}, PropType::RIGHTSIDEWALL},
            Prop{"sprites/props/GrassWallRight.png", Vector2{3712, 2944}, PropType::RIGHTSIDEWALL},
            Prop{"sprites/props/GrassWallRight.png", Vector2{3712, 2880}, PropType::RIGHTSIDEWALL},
            Prop{"sprites/props/GrassWallRight.png", Vector2{3712, 2816}, PropType::RIGHTSIDEWALL},
            Prop{"sprites/props/GrassWallRight.png", Vector2{3712, 2752}, PropType::RIGHTSIDEWALL},
            Prop{"sprites/props/GrassWallRight.png", Vector2{3712, 2688}, PropType::RIGHTSIDEWALL},
            Prop{"sprites/props/GrassWallRight.png", Vector2{3712, 2624}, PropType::RIGHTSIDEWALL},
            Prop{"sprites/props/GrassWallTopRight.png", Vector2{3712, 2560}, PropType::TOPWALL},

            // Top Walls
            Prop{"sprites/props/GrassWallTop.png", Vector2{3648, 2560}, PropType::TOPWALL},
            Prop{"sprites/props/GrassWallTop.png", Vector2{3584, 2560}, PropType::TOPWALL},
            Prop{"sprites/props/GrassWallTop.png", Vector2{3520, 2560}, PropType::TOPWALL},
            Prop{"sprites/props/GrassWallTop.png", Vector2{3456, 2560}, PropType::TOPWALL},
            Prop{"sprites/props/GrassWallTop.png", Vector2{3392, 2560}, PropType::TOPWALL},
            Prop{"sprites/props/GrassWallTop.png", Vector2{3328, 2560}, PropType::TOPWALL},
            Prop{"sprites/props/GrassWallTop.png", Vector2{3264, 2560}, PropType::TOPWALL},
            Prop{"sprites/props/GrassWallTop.png", Vector2{3200, 2560}, PropType::TOPWALL},
            Prop{"sprites/props/GrassWallTop.png", Vector2{3136, 2560}, PropType::TOPWALL},
            Prop{"sprites/props/GrassWallTop.png", Vector2{3072, 2560}, PropType::TOPWALL},
            Prop{"sprites/props/GrassWallTop.png", Vector2{3008, 2560}, PropType::TOPWALL},
            Prop{"sprites/props/GrassWallTop.png", Vector2{2944, 2560}, PropType::TOPWALL},
            Prop{"sprites/props/GrassWallTop.png", Vector2{2880, 2560}, PropType::TOPWALL},
            Prop{"sprites/props/GrassWallTop.png", Vector2{2816, 2560}, PropType::TOPWALL},
            Prop{"sprites/props/GrassWallTopLeft.png", Vector2{2752, 2560}, PropType::TOPWALL},

            // ---------------------------------------- Right Side Third Hill -----------------------------------

            // Left Side Walls
            Prop{"sprites/props/WallLeft.png", Vector2{2944, 2688}, PropType::LEFTSIDEWALL},
            Prop{"sprites/props/WallLeft.png", Vector2{2944, 2752}, PropType::LEFTSIDEWALL},
            Prop{"sprites/props/WallLeft.png", Vector2{2944, 2816}, PropType::LEFTSIDEWALL},
            Prop{"sprites/props/WallLeft.png", Vector2{2944, 2880}, PropType::LEFTSIDEWALL},
            Prop{"sprites/props/WallLeft.png", Vector2{2944, 2944}, PropType::LEFTSIDEWALL},
            Prop{"sprites/props/WallLeft.png", Vector2{2944, 3008}, PropType::LEFTSIDEWALL},
            Prop{"sprites/props/WallBotLeft.png", Vector2{2944, 3072}, PropType::BOTTOMWALL},

            // Bottom Walls
            Prop{"sprites/props/WallBottom.png", Vector2{3008, 3072}, PropType::BOTTOMWALL},
            Prop{"sprites/props/WallBottom.png", Vector2{3136, 3072}, PropType::BOTTOMWALL},
            Prop{"sprites/props/WallBottom.png", Vector2{3200, 3072}, PropType::BOTTOMWALL},
            Prop{"sprites/props/WallBottom.png", Vector2{3264, 3072}, PropType::BOTTOMWALL},
            Prop{"sprites/props/WallBottom.png", Vector2{3328, 3072}, PropType::BOTTOMWALL},
            Prop{"sprites/props/WallBottom.png", Vector2{3456, 3072}, PropType::BOTTOMWALL},
            Prop{"sprites/props/WallBotRight.png", Vector2{3520, 3072}, PropType::BOTTOMWALL},

            // Right Side Walls
            Prop{"sprites/props/WallRight.png", Vector2{3520, 3008}, PropType::RIGHTSIDEWALL},
            Prop{"sprites/props/WallRight.png", Vector2{3520, 2944}, PropType::RIGHTSIDEWALL},
            Prop{"sprites/props/WallRight.png", Vector2{3520, 2880}, PropType::RIGHTSIDEWALL},
            Prop{"sprites/props/WallRight.png", Vector2{3520, 2816}, PropType::RIGHTSIDEWALL},
            Prop{"sprites/props/WallRight.png", Vector2{3520, 2752}, PropType::RIGHTSIDEWALL},
            Prop{"sprites/props/WallRight.png", Vector2{3520, 2688}, PropType::RIGHTSIDEWALL},
            Prop{"sprites/props/WallTopRight.png", Vector2{3520, 2624}, PropType::TOPWALL},

            // Bottom Walls
            Prop{"sprites/props/GrassWallTop.png", Vector2{3456, 2624}, PropType::TOPWALL},
            Prop{"sprites/props/GrassWallTop.png", Vector2{3392, 2624}, PropType::TOPWALL},
            Prop{"sprites/props/GrassWallTop.png", Vector2{3328, 2624}, PropType::TOPWALL},
            Prop{"sprites/props/GrassWallTop.png", Vector2{3264, 2624}, PropType::TOPWALL},
            Prop{"sprites/props/GrassWallTop.png", Vector2{3200, 2624}, PropType::TOPWALL},
            Prop{"sprites/props/GrassWallTop.png", Vector2{3136, 2624}, PropType::TOPWALL},
            Prop{"sprites/props/GrassWallTop.png", Vector2{3072, 2624}, PropType::TOPWALL},
            Prop{"sprites/props/GrassWallTop.png", Vector2{3008, 2624}, PropType::TOPWALL},
            Prop{"sprites/props/WallTopLeft.png", Vector2{2944, 2624}, PropType::TOPWALL},

            // Door
            Prop{"sprites/props/DoorRed.png", Vector2{1612,2815}, PropType::DOOR, 4.f, false, true}, 
            Prop{"sprites/props/DoorBlue.png", Vector2{3120,2685}, PropType::DOOR, 4.f, false, true}
        };
        Props.emplace_back(Walls);

        std::vector<Prop> Moveable
        {
            Prop{"sprites/props/TreeStump.png", Vector2{3525,3640}, PropType::STUMP, 4.f, true},
            Prop{"sprites/props/Boulder.png", Vector2{1025,3050}, PropType::BOULDER, 4.f, true},
        };
        Props.emplace_back(Moveable);
        
        return Props;
    }

    // Initialize props drawn over character
    std::vector<std::vector<Prop>> InitializePropsOver()
    {
        std::vector<std::vector<Prop>> Props{};

        std::vector<Prop> Houses
        {
            // Red House
            Prop{"sprites/props/HouseRed.png", Vector2{1472,2688}, PropType::rHOUSELEFT},   
            Prop{"sprites/props/HouseRed.png", Vector2{1472,2688}, PropType::rHOUSERIGHT},

            // Blue House   
            Prop{"sprites/props/HouseBlue.png", Vector2{3073,2495}, PropType::bHOUSELEFT},   
            Prop{"sprites/props/HouseBlue.png", Vector2{3073,2495}, PropType::bHOUSERIGHT},

        };
        Props.emplace_back(Houses);

        std::vector<Prop> Boulder
        {
            // Prop{"sprites/props/Boulder.png", Vector2{1000,1000}, PropType::BOULDER}, 
            // Prop{"sprites/props/Boulder.png", Vector2{1112,1000}, PropType::BOULDER},
            // Prop{"sprites/props/Boulder.png", Vector2{1172,1000}, PropType::BOULDER, 4.f, true},
            // Prop{"sprites/props/Boulder.png", Vector2{750,900}, PropType::BOULDER, 4.f, true},
            
            Prop{"sprites/props/Boulder.png", Vector2{960,2950}, PropType::BOULDER},
            Prop{"sprites/props/Boulder.png", Vector2{1090,2950}, PropType::BOULDER},
            Prop{"sprites/props/Boulder.png", Vector2{940,3050}, PropType::BOULDER},
            Prop{"sprites/props/Boulder.png", Vector2{1110,3050}, PropType::BOULDER},

        };
        Props.emplace_back(Boulder);

        std::vector<Prop> Moveable
        {
            Prop{"sprites/props/TreeStump.png", Vector2{3525,3640}, PropType::STUMP, 4.f, true},
            Prop{"sprites/props/Boulder.png", Vector2{1025,3050}, PropType::BOULDER, 4.f, true},
        };
        Props.emplace_back(Moveable);

        std::vector<Prop> Trees
        {
            // row1
            Prop{"sprites/props/TreeGreen.png", Vector2{20,67}, PropType::TREE},        // 1
            Prop{"sprites/props/TreeGreen.png", Vector2{120,67}, PropType::TREE},       // 2
            Prop{"sprites/props/TreeGreen.png", Vector2{220,67}, PropType::TREE},       // 3
            Prop{"sprites/props/TreeGreen.png", Vector2{320,67}, PropType::TREE},       // 4
            Prop{"sprites/props/TreeGreen.png", Vector2{420,67}, PropType::TREE},       // 5
            Prop{"sprites/props/TreeGreen.png", Vector2{520,67}, PropType::TREE},       // 6
            Prop{"sprites/props/TreeGreen.png", Vector2{620,67}, PropType::TREE},       // 7
            Prop{"sprites/props/TreeGreen.png", Vector2{720,67}, PropType::TREE},       // 8
            Prop{"sprites/props/TreeGreen.png", Vector2{820,67}, PropType::TREE},       // 9
            Prop{"sprites/props/TreeGreen.png", Vector2{920,67}, PropType::TREE},       // 10
            Prop{"sprites/props/TreeGreen.png", Vector2{1020,67}, PropType::TREE},      // 11
            Prop{"sprites/props/TreeGreen.png", Vector2{1120,67}, PropType::TREE},      // 12
            Prop{"sprites/props/TreeGreen.png", Vector2{1220,67}, PropType::TREE},      // 13
            Prop{"sprites/props/TreeGreen.png", Vector2{1320,67}, PropType::TREE},      // 14
            Prop{"sprites/props/TreeGreen.png", Vector2{1420,67}, PropType::TREE},      // 15
            Prop{"sprites/props/TreeGreen.png", Vector2{1520,67}, PropType::TREE},      // 16
            Prop{"sprites/props/TreeGreen.png", Vector2{1620,67}, PropType::TREE},      // 17
            Prop{"sprites/props/TreeGreen.png", Vector2{1720,67}, PropType::TREE},      // 18
            Prop{"sprites/props/TreeGreen.png", Vector2{1820,67}, PropType::TREE},      // 19
            Prop{"sprites/props/TreeGreen.png", Vector2{1920,67}, PropType::TREE},      // 20
            Prop{"sprites/props/TreeGreen.png", Vector2{2020,67}, PropType::TREE},      // 21
            Prop{"sprites/props/TreeGreen.png", Vector2{2120,67}, PropType::TREE},      // 22
            Prop{"sprites/props/TreeGreen.png", Vector2{2220,67}, PropType::TREE},      // 23
            Prop{"sprites/props/TreeGreen.png", Vector2{2320,67}, PropType::TREE},      // 24
            Prop{"sprites/props/TreeGreen.png", Vector2{2420,67}, PropType::TREE},      // 25
            Prop{"sprites/props/TreeGreen.png", Vector2{2520,67}, PropType::TREE},      // 26
            Prop{"sprites/props/TreeGreen.png", Vector2{2620,67}, PropType::TREE},      // 27
            Prop{"sprites/props/TreeGreen.png", Vector2{2720,67}, PropType::TREE},      // 28
            Prop{"sprites/props/TreeGreen.png", Vector2{2820,67}, PropType::TREE},      // 29
            Prop{"sprites/props/TreeGreen.png", Vector2{2920,67}, PropType::TREE},      // 30
            Prop{"sprites/props/TreeGreen.png", Vector2{3020,67}, PropType::TREE},      // 31
            Prop{"sprites/props/TreeGreen.png", Vector2{3120,67}, PropType::TREE},      // 32
            Prop{"sprites/props/TreeGreen.png", Vector2{3220,67}, PropType::TREE},      // 33
            Prop{"sprites/props/TreeGreen.png", Vector2{3320,67}, PropType::TREE},      // 34
            Prop{"sprites/props/TreeGreen.png", Vector2{3420,67}, PropType::TREE},      // 35

            Prop{"sprites/props/TreeGreen.png", Vector2{3700,67}, PropType::TREE},      // 36
            Prop{"sprites/props/TreeGreen.png", Vector2{3800,67}, PropType::TREE},      // 37
            Prop{"sprites/props/TreeGreen.png", Vector2{3900,67}, PropType::TREE},      // 38
            Prop{"sprites/props/TreeGreen.png", Vector2{4000,67}, PropType::TREE},      // 39

            // row2
            Prop{"sprites/props/TreeGreen.png", Vector2{0,167}, PropType::TREE},        // 1
            // Prop{"sprites/props/TreeGreen.png", Vector2{100,167}, PropType::TREE},   // 2
            Prop{"sprites/props/TreeGreen.png", Vector2{200,167}, PropType::TREE},      // 3
            Prop{"sprites/props/TreeGreen.png", Vector2{300,167}, PropType::TREE},      // 4
            Prop{"sprites/props/TreeGreen.png", Vector2{400,167}, PropType::TREE},      // 5
            Prop{"sprites/props/TreeGreen.png", Vector2{500,167}, PropType::TREE},      // 6
            // Prop{"sprites/props/TreeGreen.png", Vector2{600,167}, PropType::TREE},   // 7
            Prop{"sprites/props/TreeGreen.png", Vector2{700,167}, PropType::TREE},      // 8
            Prop{"sprites/props/TreeGreen.png", Vector2{800,167}, PropType::TREE},      // 9
            Prop{"sprites/props/TreeGreen.png", Vector2{900,167}, PropType::TREE},      // 10
            Prop{"sprites/props/TreeGreen.png", Vector2{1000,167}, PropType::TREE},     // 11
            // Prop{"sprites/props/TreeGreen.png", Vector2{1100,167}, PropType::TREE},     // 12
            // Prop{"sprites/props/TreeGreen.png", Vector2{1200,167}, PropType::TREE},     // 13
            // Prop{"sprites/props/TreeGreen.png", Vector2{1300,167}, PropType::TREE},     // 14
            Prop{"sprites/props/TreeGreen.png", Vector2{1400,167}, PropType::TREE},     // 15
            Prop{"sprites/props/TreeGreen.png", Vector2{1500,167}, PropType::TREE},     // 16
            Prop{"sprites/props/TreeGreen.png", Vector2{1600,167}, PropType::TREE},     // 17
            // Prop{"sprites/props/TreeGreen.png", Vector2{1700,167}, PropType::TREE},  // 18
            // Prop{"sprites/props/TreeGreen.png", Vector2{1800,167}, PropType::TREE},  // 19
            Prop{"sprites/props/TreeGreen.png", Vector2{1900,167}, PropType::TREE},     // 20
            Prop{"sprites/props/TreeGreen.png", Vector2{2000,167}, PropType::TREE},     // 21
            Prop{"sprites/props/TreeGreen.png", Vector2{2100,167}, PropType::TREE},     // 22
            Prop{"sprites/props/TreeGreen.png", Vector2{2200,167}, PropType::TREE},     // 23
            // Prop{"sprites/props/TreeGreen.png", Vector2{2300,167}, PropType::TREE},  // 24
            // Prop{"sprites/props/TreeGreen.png", Vector2{2400,167}, PropType::TREE},  // 25
            Prop{"sprites/props/TreeGreen.png", Vector2{2500,167}, PropType::TREE},     // 26
            Prop{"sprites/props/TreeGreen.png", Vector2{2600,167}, PropType::TREE},     // 27
            Prop{"sprites/props/TreeGreen.png", Vector2{2700,167}, PropType::TREE},     // 28
            Prop{"sprites/props/TreeGreen.png", Vector2{2800,167}, PropType::TREE},     // 29
            // Prop{"sprites/props/TreeGreen.png", Vector2{2900,167}, PropType::TREE},  // 30
            Prop{"sprites/props/TreeGreen.png", Vector2{3000,167}, PropType::TREE},     // 31
            Prop{"sprites/props/TreeGreen.png", Vector2{3100,167}, PropType::TREE},     // 32
            Prop{"sprites/props/TreeGreen.png", Vector2{3200,167}, PropType::TREE},     // 33
            Prop{"sprites/props/TreeGreen.png", Vector2{3300,167}, PropType::TREE},     // 34
            Prop{"sprites/props/TreeGreen.png", Vector2{3400,167}, PropType::TREE},     // 35

            Prop{"sprites/props/TreeGreen.png", Vector2{3720,167}, PropType::TREE},     // 36
            // Prop{"sprites/props/TreeGreen.png", Vector2{3820,167}, PropType::TREE},  // 37
            Prop{"sprites/props/TreeGreen.png", Vector2{3920,167}, PropType::TREE},     // 38

            // row3
            Prop{"sprites/props/TreeGreen.png", Vector2{20,267}, PropType::TREE},       // 1 
            Prop{"sprites/props/TreeGreen.png", Vector2{120,267}, PropType::TREE},      // 2 
            Prop{"sprites/props/TreeGreen.png", Vector2{220,267}, PropType::TREE},      // 3 
            Prop{"sprites/props/TreeGreen.png", Vector2{320,267}, PropType::TREE},      // 4 
            Prop{"sprites/props/TreeGreen.png", Vector2{420,267}, PropType::TREE},      // 5 
            // Prop{"sprites/props/TreeGreen.png", Vector2{520,267}, PropType::TREE},   // 6     
            Prop{"sprites/props/TreeGreen.png", Vector2{620,267}, PropType::TREE},      // 7 
            Prop{"sprites/props/TreeGreen.png", Vector2{720,267}, PropType::TREE},      // 8 
            Prop{"sprites/props/TreeGreen.png", Vector2{820,267}, PropType::TREE},      // 9 
            Prop{"sprites/props/TreeGreen.png", Vector2{920,267}, PropType::TREE},      // 10
            // Prop{"sprites/props/TreeGreen.png", Vector2{1020,267}, PropType::TREE},  // 11
            // Prop{"sprites/props/TreeGreen.png", Vector2{1120,267}, PropType::TREE},     // 12
            // Prop{"sprites/props/TreeGreen.png", Vector2{1220,267}, PropType::TREE},     // 13
            // Prop{"sprites/props/TreeGreen.png", Vector2{1320,267}, PropType::TREE},     // 14
            // Prop{"sprites/props/TreeGreen.png", Vector2{1420,267}, PropType::TREE},  // 15
            Prop{"sprites/props/TreeGreen.png", Vector2{1520,267}, PropType::TREE},     // 16
            Prop{"sprites/props/TreeGreen.png", Vector2{1620,267}, PropType::TREE},     // 17
            Prop{"sprites/props/TreeGreen.png", Vector2{1720,267}, PropType::TREE},     // 18
            Prop{"sprites/props/TreeGreen.png", Vector2{1820,267}, PropType::TREE},     // 19
            Prop{"sprites/props/TreeGreen.png", Vector2{1920,267}, PropType::TREE},     // 20
            Prop{"sprites/props/TreeGreen.png", Vector2{2020,267}, PropType::TREE},     // 21
            // Prop{"sprites/props/TreeGreen.png", Vector2{2120,267}, PropType::TREE},  // 22
            Prop{"sprites/props/TreeGreen.png", Vector2{2220,267}, PropType::TREE},     // 23
            Prop{"sprites/props/TreeGreen.png", Vector2{2320,267}, PropType::TREE},     // 24
            Prop{"sprites/props/TreeGreen.png", Vector2{2420,267}, PropType::TREE},     // 25
            Prop{"sprites/props/TreeGreen.png", Vector2{2520,267}, PropType::TREE},     // 26
            Prop{"sprites/props/TreeGreen.png", Vector2{2620,267}, PropType::TREE},     // 27
            // Prop{"sprites/props/TreeGreen.png", Vector2{2720,267}, PropType::TREE},  // 28
            Prop{"sprites/props/TreeGreen.png", Vector2{2820,267}, PropType::TREE},     // 29
            Prop{"sprites/props/TreeGreen.png", Vector2{2920,267}, PropType::TREE},     // 30
            Prop{"sprites/props/TreeGreen.png", Vector2{3020,267}, PropType::TREE},     // 31
            Prop{"sprites/props/TreeGreen.png", Vector2{3120,267}, PropType::TREE},     // 32
            // Prop{"sprites/props/TreeGreen.png", Vector2{3220,267}, PropType::TREE},  // 33
            Prop{"sprites/props/TreeGreen.png", Vector2{3320,267}, PropType::TREE},     // 34
            Prop{"sprites/props/TreeGreen.png", Vector2{3420,267}, PropType::TREE},     // 35

            Prop{"sprites/props/TreeGreen.png", Vector2{3700,267}, PropType::TREE},     // 36
            Prop{"sprites/props/TreeGreen.png", Vector2{3800,267}, PropType::TREE},     // 37
            Prop{"sprites/props/TreeGreen.png", Vector2{3900,267}, PropType::TREE},     // 38
            Prop{"sprites/props/TreeGreen.png", Vector2{4000,267}, PropType::TREE},     // 39

            // row4
            Prop{"sprites/props/TreeGreen.png", Vector2{0,367}, PropType::TREE},        // 1 
            Prop{"sprites/props/TreeGreen.png", Vector2{100,367}, PropType::TREE},      // 2 
            Prop{"sprites/props/TreeGreen.png", Vector2{200,367}, PropType::TREE},      // 3 
            // Prop{"sprites/props/TreeGreen.png", Vector2{300,367}, PropType::TREE},   // 4     
            Prop{"sprites/props/TreeGreen.png", Vector2{400,367}, PropType::TREE},      // 5 
            Prop{"sprites/props/TreeGreen.png", Vector2{500,367}, PropType::TREE},      // 6 
            // Prop{"sprites/props/TreeGreen.png", Vector2{600,367}, PropType::TREE},   // 7 
            Prop{"sprites/props/TreeGreen.png", Vector2{700,367}, PropType::TREE},      // 8 
            Prop{"sprites/props/TreeGreen.png", Vector2{800,367}, PropType::TREE},      // 9 
            Prop{"sprites/props/TreeGreen.png", Vector2{900,367}, PropType::TREE},      // 10
            // Prop{"sprites/props/TreeGreen.png", Vector2{1000,367}, PropType::TREE},     // 11
            // Prop{"sprites/props/TreeGreen.png", Vector2{1100,367}, PropType::TREE},     // 12
            // Prop{"sprites/props/TreeGreen.png", Vector2{1200,367}, PropType::TREE},     // 13
            // Prop{"sprites/props/TreeGreen.png", Vector2{1300,367}, PropType::TREE},     // 14
            // Prop{"sprites/props/TreeGreen.png", Vector2{1400,367}, PropType::TREE},     // 15
            Prop{"sprites/props/TreeGreen.png", Vector2{1500,367}, PropType::TREE},     // 16
            Prop{"sprites/props/TreeGreen.png", Vector2{1600,367}, PropType::TREE},     // 17
            // Prop{"sprites/props/TreeGreen.png", Vector2{1700,367}, PropType::TREE},  // 18
            Prop{"sprites/props/TreeGreen.png", Vector2{1800,367}, PropType::TREE},     // 19
            Prop{"sprites/props/TreeGreen.png", Vector2{1900,367}, PropType::TREE},     // 20
            // Prop{"sprites/props/TreeGreen.png", Vector2{2000,367}, PropType::TREE},     // 21
            // Prop{"sprites/props/TreeGreen.png", Vector2{2100,367}, PropType::TREE},     // 22
            // Prop{"sprites/props/TreeGreen.png", Vector2{2200,367}, PropType::TREE},     // 23
            // Prop{"sprites/props/TreeGreen.png", Vector2{2300,367}, PropType::TREE},     // 24
            Prop{"sprites/props/TreeGreen.png", Vector2{2400,367}, PropType::TREE},  // 25
            Prop{"sprites/props/TreeGreen.png", Vector2{2500,367}, PropType::TREE},     // 26
            Prop{"sprites/props/TreeGreen.png", Vector2{2600,367}, PropType::TREE},     // 27
            Prop{"sprites/props/TreeGreen.png", Vector2{2700,367}, PropType::TREE},     // 28
            Prop{"sprites/props/TreeGreen.png", Vector2{2800,367}, PropType::TREE},     // 29
            Prop{"sprites/props/TreeGreen.png", Vector2{2900,367}, PropType::TREE},     // 30
            Prop{"sprites/props/TreeGreen.png", Vector2{3000,367}, PropType::TREE},     // 31
            Prop{"sprites/props/TreeGreen.png", Vector2{3100,367}, PropType::TREE},     // 32
            Prop{"sprites/props/TreeGreen.png", Vector2{3200,367}, PropType::TREE},     // 33
            Prop{"sprites/props/TreeGreen.png", Vector2{3300,367}, PropType::TREE},     // 34
            Prop{"sprites/props/TreeGreen.png", Vector2{3400,367}, PropType::TREE},     // 35

            Prop{"sprites/props/TreeGreen.png", Vector2{3720,367}, PropType::TREE},     // 36
            Prop{"sprites/props/TreeGreen.png", Vector2{3820,367}, PropType::TREE},     // 37
            Prop{"sprites/props/TreeGreen.png", Vector2{3920,367}, PropType::TREE},     // 38

            //row5
            Prop{"sprites/props/TreeGreen.png", Vector2{20,467}, PropType::TREE},       // 1 
            Prop{"sprites/props/TreeGreen.png", Vector2{120,467}, PropType::TREE},      // 2 
            // Prop{"sprites/props/TreeGreen.png", Vector2{220,467}, PropType::TREE},   // 3 
            Prop{"sprites/props/TreeGreen.png", Vector2{320,467}, PropType::TREE},      // 4 
            // Prop{"sprites/props/TreeGreen.png", Vector2{420,467}, PropType::TREE},   // 5 
            Prop{"sprites/props/TreeGreen.png", Vector2{520,467}, PropType::TREE},      // 6 
            Prop{"sprites/props/TreeGreen.png", Vector2{620,467}, PropType::TREE},      // 7 
            Prop{"sprites/props/TreeGreen.png", Vector2{720,467}, PropType::TREE},      // 8 
            // Prop{"sprites/props/TreeGreen.png", Vector2{820,467}, PropType::TREE},   // 9 
            Prop{"sprites/props/TreeGreen.png", Vector2{920,467}, PropType::TREE},      // 10
            Prop{"sprites/props/TreeGreen.png", Vector2{1020,467}, PropType::TREE},     // 11
            // Prop{"sprites/props/TreeGreen.png", Vector2{1120,467}, PropType::TREE},     // 12
            // Prop{"sprites/props/TreeGreen.png", Vector2{1220,467}, PropType::TREE},     // 13
            // Prop{"sprites/props/TreeGreen.png", Vector2{1320,467}, PropType::TREE},     // 14
            Prop{"sprites/props/TreeGreen.png", Vector2{1420,467}, PropType::TREE},     // 15
            Prop{"sprites/props/TreeGreen.png", Vector2{1520,467}, PropType::TREE},     // 16
            Prop{"sprites/props/TreeGreen.png", Vector2{1620,467}, PropType::TREE},     // 17
            // Prop{"sprites/props/TreeGreen.png", Vector2{1720,467}, PropType::TREE},  // 18
            Prop{"sprites/props/TreeGreen.png", Vector2{1820,467}, PropType::TREE},     // 19
            // Prop{"sprites/props/TreeGreen.png", Vector2{1920,467}, PropType::TREE},     // 20
            // Prop{"sprites/props/TreeGreen.png", Vector2{2020,467}, PropType::TREE},     // 21
            // Prop{"sprites/props/TreeGreen.png", Vector2{2120,467}, PropType::TREE},     // 22
            // Prop{"sprites/props/TreeGreen.png", Vector2{2220,467}, PropType::TREE},     // 23
            // Prop{"sprites/props/TreeGreen.png", Vector2{2320,467}, PropType::TREE},     // 24
            // Prop{"sprites/props/TreeGreen.png", Vector2{2420,467}, PropType::TREE},  // 25
            Prop{"sprites/props/TreeGreen.png", Vector2{2520,467}, PropType::TREE},     // 26
            Prop{"sprites/props/TreeGreen.png", Vector2{2620,467}, PropType::TREE},     // 27
            Prop{"sprites/props/TreeGreen.png", Vector2{2720,467}, PropType::TREE},     // 28
            Prop{"sprites/props/TreeGreen.png", Vector2{2820,467}, PropType::TREE},     // 29
            Prop{"sprites/props/TreeGreen.png", Vector2{2920,467}, PropType::TREE},     // 30
            // Prop{"sprites/props/TreeGreen.png", Vector2{3020,467}, PropType::TREE},  // 31
            Prop{"sprites/props/TreeGreen.png", Vector2{3120,467}, PropType::TREE},     // 32
            Prop{"sprites/props/TreeGreen.png", Vector2{3220,467}, PropType::TREE},     // 33
            Prop{"sprites/props/TreeGreen.png", Vector2{3320,467}, PropType::TREE},     // 34
            Prop{"sprites/props/TreeGreen.png", Vector2{3420,467}, PropType::TREE},     // 35

            Prop{"sprites/props/TreeGreen.png", Vector2{3700,467}, PropType::TREE},     // 36
            Prop{"sprites/props/TreeGreen.png", Vector2{3800,467}, PropType::TREE},     // 37
            Prop{"sprites/props/TreeGreen.png", Vector2{3900,467}, PropType::TREE},     // 38
            Prop{"sprites/props/TreeGreen.png", Vector2{4000,467}, PropType::TREE},     // 39

            // row6
            Prop{"sprites/props/TreeGreen.png", Vector2{0,567}, PropType::TREE},        // 1 
            Prop{"sprites/props/TreeGreen.png", Vector2{100,567}, PropType::TREE},      // 2 
            Prop{"sprites/props/TreeGreen.png", Vector2{200,567}, PropType::TREE},      // 3 
            Prop{"sprites/props/TreeGreen.png", Vector2{300,567}, PropType::TREE},      // 4 
            Prop{"sprites/props/TreeGreen.png", Vector2{400,567}, PropType::TREE},      // 5 
            Prop{"sprites/props/TreeGreen.png", Vector2{500,567}, PropType::TREE},      // 6 
            Prop{"sprites/props/TreeGreen.png", Vector2{600,567}, PropType::TREE},      // 7 
            // Prop{"sprites/props/TreeGreen.png", Vector2{700,567}, PropType::TREE},   // 8 
            Prop{"sprites/props/TreeGreen.png", Vector2{800,567}, PropType::TREE},      // 9 
            Prop{"sprites/props/TreeGreen.png", Vector2{900,567}, PropType::TREE},      // 10
            Prop{"sprites/props/TreeGreen.png", Vector2{1000,567}, PropType::TREE},     // 11
            Prop{"sprites/props/TreeGreen.png", Vector2{1100,567}, PropType::TREE},     // 12
            Prop{"sprites/props/TreeGreen.png", Vector2{1200,567}, PropType::TREE},     // 13
            // Prop{"sprites/props/TreeGreen.png", Vector2{1300,567}, PropType::TREE},  // 14
            Prop{"sprites/props/TreeGreen.png", Vector2{1400,567}, PropType::TREE},     // 15
            Prop{"sprites/props/TreeGreen.png", Vector2{1500,567}, PropType::TREE},     // 16
            Prop{"sprites/props/TreeGreen.png", Vector2{1600,567}, PropType::TREE},     // 17
            Prop{"sprites/props/TreeGreen.png", Vector2{1700,567}, PropType::TREE},     // 18
            // Prop{"sprites/props/TreeGreen.png", Vector2{1800,567}, PropType::TREE},     // 19
            // Prop{"sprites/props/TreeGreen.png", Vector2{1900,567}, PropType::TREE},     // 20
            // Prop{"sprites/props/TreeGreen.png", Vector2{2000,567}, PropType::TREE},     // 21
            // Prop{"sprites/props/TreeGreen.png", Vector2{2100,567}, PropType::TREE},     // 22
            // Prop{"sprites/props/TreeGreen.png", Vector2{2200,567}, PropType::TREE},     // 23
            // Prop{"sprites/props/TreeGreen.png", Vector2{2300,567}, PropType::TREE},     // 24
            // Prop{"sprites/props/TreeGreen.png", Vector2{2400,567}, PropType::TREE},     // 25
            Prop{"sprites/props/TreeGreen.png", Vector2{2500,567}, PropType::TREE},     // 26
            // Prop{"sprites/props/TreeGreen.png", Vector2{2600,567}, PropType::TREE},  // 27
            Prop{"sprites/props/TreeGreen.png", Vector2{2700,567}, PropType::TREE},     // 28
            Prop{"sprites/props/TreeGreen.png", Vector2{2800,567}, PropType::TREE},     // 29
            Prop{"sprites/props/TreeGreen.png", Vector2{2900,567}, PropType::TREE},     // 30
            Prop{"sprites/props/TreeGreen.png", Vector2{3000,567}, PropType::TREE},     // 31
            Prop{"sprites/props/TreeGreen.png", Vector2{3100,567}, PropType::TREE},     // 32
            Prop{"sprites/props/TreeGreen.png", Vector2{3200,567}, PropType::TREE},     // 33
            Prop{"sprites/props/TreeGreen.png", Vector2{3300,567}, PropType::TREE},     // 34
            Prop{"sprites/props/TreeGreen.png", Vector2{3400,567}, PropType::TREE},     // 35

            Prop{"sprites/props/TreeGreen.png", Vector2{3720,567}, PropType::TREE},     // 36
            Prop{"sprites/props/TreeGreen.png", Vector2{3820,567}, PropType::TREE},     // 37
            Prop{"sprites/props/TreeGreen.png", Vector2{3920,567}, PropType::TREE},     // 38

            // row7
            Prop{"sprites/props/TreeGreen.png", Vector2{20,667}, PropType::TREE},       // 1 
            Prop{"sprites/props/TreeGreen.png", Vector2{120,667}, PropType::TREE},      // 2 
            Prop{"sprites/props/TreeGreen.png", Vector2{220,667}, PropType::TREE},      // 3 
            Prop{"sprites/props/TreeGreen.png", Vector2{320,667}, PropType::TREE},      // 4 
            Prop{"sprites/props/TreeGreen.png", Vector2{420,667}, PropType::TREE},      // 5 
            Prop{"sprites/props/TreeGreen.png", Vector2{520,667}, PropType::TREE},      // 6 
            // Prop{"sprites/props/TreeGreen.png", Vector2{620,667}, PropType::TREE},   // 7     
            Prop{"sprites/props/TreeGreen.png", Vector2{720,667}, PropType::TREE},      // 8 
            Prop{"sprites/props/TreeGreen.png", Vector2{820,667}, PropType::TREE},      // 9 
            Prop{"sprites/props/TreeGreen.png", Vector2{920,667}, PropType::TREE},      // 10
            Prop{"sprites/props/TreeGreen.png", Vector2{1020,667}, PropType::TREE},     // 11
            // Prop{"sprites/props/TreeGreen.png", Vector2{1120,667}, PropType::TREE},  // 12    
            Prop{"sprites/props/TreeGreen.png", Vector2{1220,667}, PropType::TREE},     // 13
            Prop{"sprites/props/TreeGreen.png", Vector2{1320,667}, PropType::TREE},     // 14
            Prop{"sprites/props/TreeGreen.png", Vector2{1420,667}, PropType::TREE},     // 15
            Prop{"sprites/props/TreeGreen.png", Vector2{1520,667}, PropType::TREE},     // 16
            // Prop{"sprites/props/TreeGreen.png", Vector2{1620,667}, PropType::TREE},  // 17    
            Prop{"sprites/props/TreeGreen.png", Vector2{1720,667}, PropType::TREE},     // 18
            Prop{"sprites/props/TreeGreen.png", Vector2{1820,667}, PropType::TREE},     // 19
            // Prop{"sprites/props/TreeGreen.png", Vector2{1920,667}, PropType::TREE},     // 20
            // Prop{"sprites/props/TreeGreen.png", Vector2{2020,667}, PropType::TREE},     // 21
            // Prop{"sprites/props/TreeGreen.png", Vector2{2120,667}, PropType::TREE},     // 22
            // Prop{"sprites/props/TreeGreen.png", Vector2{2220,667}, PropType::TREE},     // 23
            // // Prop{"sprites/props/TreeGreen.png", Vector2{2320,667}, PropType::TREE},  // 24    
            Prop{"sprites/props/TreeGreen.png", Vector2{2420,667}, PropType::TREE},     // 25
            Prop{"sprites/props/TreeGreen.png", Vector2{2520,667}, PropType::TREE},     // 26
            Prop{"sprites/props/TreeGreen.png", Vector2{2620,667}, PropType::TREE},     // 27
            Prop{"sprites/props/TreeGreen.png", Vector2{2720,667}, PropType::TREE},     // 28    
            // Prop{"sprites/props/TreeGreen.png", Vector2{2820,667}, PropType::TREE},     // 29
            // Prop{"sprites/props/TreeGreen.png", Vector2{2920,667}, PropType::TREE},     // 30
            // Prop{"sprites/props/TreeGreen.png", Vector2{3020,667}, PropType::TREE},     // 31
            Prop{"sprites/props/TreeGreen.png", Vector2{3120,667}, PropType::TREE},     // 32    
            Prop{"sprites/props/TreeGreen.png", Vector2{3220,667}, PropType::TREE},     // 33
            Prop{"sprites/props/TreeGreen.png", Vector2{3320,667}, PropType::TREE},     // 34
            Prop{"sprites/props/TreeGreen.png", Vector2{3420,667}, PropType::TREE},     // 35

            Prop{"sprites/props/TreeGreen.png", Vector2{3700,667}, PropType::TREE},     // 36    
            Prop{"sprites/props/TreeGreen.png", Vector2{3800,667}, PropType::TREE},     // 37
            Prop{"sprites/props/TreeGreen.png", Vector2{3900,667}, PropType::TREE},     // 38
            Prop{"sprites/props/TreeGreen.png", Vector2{4000,667}, PropType::TREE},     // 39

            // row8
            Prop{"sprites/props/TreeGreen.png", Vector2{0,767}, PropType::TREE},        // 1 
            Prop{"sprites/props/TreeGreen.png", Vector2{100,767}, PropType::TREE},      // 2 
            Prop{"sprites/props/TreeGreen.png", Vector2{200,767}, PropType::TREE},      // 3 
            Prop{"sprites/props/TreeGreen.png", Vector2{300,767}, PropType::TREE},      // 4 
            Prop{"sprites/props/TreeGreen.png", Vector2{400,767}, PropType::TREE},      // 5 
            Prop{"sprites/props/TreeGreen.png", Vector2{500,767}, PropType::TREE},      // 6 
            Prop{"sprites/props/TreeGreen.png", Vector2{600,767}, PropType::TREE},      // 7 
            // Prop{"sprites/props/TreeGreen.png", Vector2{700,767}, PropType::TREE},   // 8     
            Prop{"sprites/props/TreeGreen.png", Vector2{800,767}, PropType::TREE},      // 9 
            Prop{"sprites/props/TreeGreen.png", Vector2{900,767}, PropType::TREE},      // 10
            Prop{"sprites/props/TreeGreen.png", Vector2{1000,767}, PropType::TREE},     // 11
            // Prop{"sprites/props/TreeGreen.png", Vector2{1100,767}, PropType::TREE},  // 12    
            Prop{"sprites/props/TreeGreen.png", Vector2{1200,767}, PropType::TREE},     // 13
            Prop{"sprites/props/TreeGreen.png", Vector2{1300,767}, PropType::TREE},     // 14
            Prop{"sprites/props/TreeGreen.png", Vector2{1400,767}, PropType::TREE},     // 15
            Prop{"sprites/props/TreeGreen.png", Vector2{1500,767}, PropType::TREE},     // 16
            Prop{"sprites/props/TreeGreen.png", Vector2{1600,767}, PropType::TREE},     // 17
            Prop{"sprites/props/TreeGreen.png", Vector2{1700,767}, PropType::TREE},     // 18
            // Prop{"sprites/props/TreeGreen.png", Vector2{1800,767}, PropType::TREE},  // 19    
            Prop{"sprites/props/TreeGreen.png", Vector2{1900,767}, PropType::TREE},     // 20
            // Prop{"sprites/props/TreeGreen.png", Vector2{2000,767}, PropType::TREE},     // 21
            // Prop{"sprites/props/TreeGreen.png", Vector2{2100,767}, PropType::TREE},     // 22
            // Prop{"sprites/props/TreeGreen.png", Vector2{2200,767}, PropType::TREE},     // 23
            // Prop{"sprites/props/TreeGreen.png", Vector2{2300,767}, PropType::TREE},     // 24
            Prop{"sprites/props/TreeGreen.png", Vector2{2400,767}, PropType::TREE},     // 25
            Prop{"sprites/props/TreeGreen.png", Vector2{2500,767}, PropType::TREE},     // 26
            Prop{"sprites/props/TreeGreen.png", Vector2{2600,767}, PropType::TREE},     // 27
            // Prop{"sprites/props/TreeGreen.png", Vector2{2700,767}, PropType::TREE},     // 28    
            // Prop{"sprites/props/TreeGreen.png", Vector2{2800,767}, PropType::TREE},     // 29
            // Prop{"sprites/props/TreeGreen.png", Vector2{2900,767}, PropType::TREE},     // 30
            // Prop{"sprites/props/TreeGreen.png", Vector2{3000,767}, PropType::TREE},     // 31
            // Prop{"sprites/props/TreeGreen.png", Vector2{3100,767}, PropType::TREE},     // 32
            Prop{"sprites/props/TreeGreen.png", Vector2{3200,767}, PropType::TREE},     // 33
            Prop{"sprites/props/TreeGreen.png", Vector2{3300,767}, PropType::TREE},     // 34    
            Prop{"sprites/props/TreeGreen.png", Vector2{3400,767}, PropType::TREE},     // 35

            Prop{"sprites/props/TreeGreen.png", Vector2{3720,767}, PropType::TREE},     // 36    
            Prop{"sprites/props/TreeGreen.png", Vector2{3820,767}, PropType::TREE},     // 37    
            Prop{"sprites/props/TreeGreen.png", Vector2{3920,767}, PropType::TREE},     // 38

            // row9
            Prop{"sprites/props/TreeGreen.png", Vector2{20,867}, PropType::TREE},       // 1 
            Prop{"sprites/props/TreeGreen.png", Vector2{120,867}, PropType::TREE},      // 2 
            Prop{"sprites/props/TreeGreen.png", Vector2{220,867}, PropType::TREE},      // 3 
            Prop{"sprites/props/TreeGreen.png", Vector2{320,867}, PropType::TREE},      // 4 
            Prop{"sprites/props/TreeGreen.png", Vector2{420,867}, PropType::TREE},      // 5 
            Prop{"sprites/props/TreeGreen.png", Vector2{520,867}, PropType::TREE},      // 6 
            // Prop{"sprites/props/TreeGreen.png", Vector2{620,867}, PropType::TREE},      // 7 
            // Prop{"sprites/props/TreeGreen.png", Vector2{720,867}, PropType::TREE},      // 8 
            // Prop{"sprites/props/TreeGreen.png", Vector2{820,867}, PropType::TREE},      // 9 
            // Prop{"sprites/props/TreeGreen.png", Vector2{920,867}, PropType::TREE},      // 10
            Prop{"sprites/props/TreeGreen.png", Vector2{1020,867}, PropType::TREE},     // 11
            Prop{"sprites/props/TreeGreen.png", Vector2{1120,867}, PropType::TREE},     // 12
            Prop{"sprites/props/TreeGreen.png", Vector2{1220,867}, PropType::TREE},     // 13
            Prop{"sprites/props/TreeGreen.png", Vector2{1320,867}, PropType::TREE},     // 14
            Prop{"sprites/props/TreeGreen.png", Vector2{1420,867}, PropType::TREE},     // 15
            Prop{"sprites/props/TreeGreen.png", Vector2{1520,867}, PropType::TREE},     // 16
            Prop{"sprites/props/TreeGreen.png", Vector2{1620,867}, PropType::TREE},     // 17
            Prop{"sprites/props/TreeGreen.png", Vector2{1720,867}, PropType::TREE},     // 18
            Prop{"sprites/props/TreeGreen.png", Vector2{1820,867}, PropType::TREE},     // 19
            Prop{"sprites/props/TreeGreen.png", Vector2{1920,867}, PropType::TREE},     // 20
            Prop{"sprites/props/TreeGreen.png", Vector2{2020,867}, PropType::TREE},     // 21
            Prop{"sprites/props/TreeGreen.png", Vector2{2120,867}, PropType::TREE},     // 22
            Prop{"sprites/props/TreeGreen.png", Vector2{2220,867}, PropType::TREE},     // 23
            Prop{"sprites/props/TreeGreen.png", Vector2{2320,867}, PropType::TREE},     // 24
            Prop{"sprites/props/TreeGreen.png", Vector2{2420,867}, PropType::TREE},     // 25
            Prop{"sprites/props/TreeGreen.png", Vector2{2520,867}, PropType::TREE},     // 26
            Prop{"sprites/props/TreeGreen.png", Vector2{2620,867}, PropType::TREE},     // 27
            // Prop{"sprites/props/TreeGreen.png", Vector2{2720,867}, PropType::TREE},     // 28
            // Prop{"sprites/props/TreeGreen.png", Vector2{2820,867}, PropType::TREE},     // 29
            // Prop{"sprites/props/TreeGreen.png", Vector2{2920,867}, PropType::TREE},     // 30
            // Prop{"sprites/props/TreeGreen.png", Vector2{3020,867}, PropType::TREE},     // 31
            // Prop{"sprites/props/TreeGreen.png", Vector2{3120,867}, PropType::TREE},     // 32
            Prop{"sprites/props/TreeGreen.png", Vector2{3220,867}, PropType::TREE},     // 33
            Prop{"sprites/props/TreeGreen.png", Vector2{3320,867}, PropType::TREE},     // 34
            Prop{"sprites/props/TreeGreen.png", Vector2{3420,867}, PropType::TREE},     // 35

            Prop{"sprites/props/TreeGreen.png", Vector2{3700,867}, PropType::TREE},     // 36
            Prop{"sprites/props/TreeGreen.png", Vector2{3800,867}, PropType::TREE},     // 37
            Prop{"sprites/props/TreeGreen.png", Vector2{3900,867}, PropType::TREE},     // 38
            Prop{"sprites/props/TreeGreen.png", Vector2{4000,867}, PropType::TREE},     // 39

            // row10
            Prop{"sprites/props/TreeGreen.png", Vector2{0,967}, PropType::TREE},        // 1 
            Prop{"sprites/props/TreeGreen.png", Vector2{100,967}, PropType::TREE},      // 2 
            Prop{"sprites/props/TreeGreen.png", Vector2{200,967}, PropType::TREE},      // 3 
            Prop{"sprites/props/TreeGreen.png", Vector2{300,967}, PropType::TREE},      // 4 
            Prop{"sprites/props/TreeGreen.png", Vector2{400,967}, PropType::TREE},      // 5 
            // Prop{"sprites/props/TreeGreen.png", Vector2{500,967}, PropType::TREE},      // 6 
            // Prop{"sprites/props/TreeGreen.png", Vector2{600,967}, PropType::TREE},      // 7 
            // Prop{"sprites/props/TreeGreen.png", Vector2{700,967}, PropType::TREE},      // 8 
            // Prop{"sprites/props/TreeGreen.png", Vector2{800,967}, PropType::TREE},      // 9 
            // Prop{"sprites/props/TreeGreen.png", Vector2{900,967}, PropType::TREE},      // 10
            // Prop{"sprites/props/TreeGreen.png", Vector2{1000,967}, PropType::TREE},     // 11
            Prop{"sprites/props/TreeGreen.png", Vector2{1100,967}, PropType::TREE},     // 12
            Prop{"sprites/props/TreeGreen.png", Vector2{1200,967}, PropType::TREE},     // 13
            Prop{"sprites/props/TreeGreen.png", Vector2{1300,967}, PropType::TREE},     // 14
            Prop{"sprites/props/TreeGreen.png", Vector2{1400,967}, PropType::TREE},     // 15
            Prop{"sprites/props/TreeGreen.png", Vector2{1500,967}, PropType::TREE},     // 16
            Prop{"sprites/props/TreeGreen.png", Vector2{1600,967}, PropType::TREE},     // 17
            Prop{"sprites/props/TreeGreen.png", Vector2{1700,967}, PropType::TREE},     // 18
            Prop{"sprites/props/TreeGreen.png", Vector2{1800,967}, PropType::TREE},     // 19
            Prop{"sprites/props/TreeGreen.png", Vector2{1900,967}, PropType::TREE},     // 20
            Prop{"sprites/props/TreeGreen.png", Vector2{2000,967}, PropType::TREE},     // 21
            Prop{"sprites/props/TreeGreen.png", Vector2{2100,967}, PropType::TREE},     // 22
            Prop{"sprites/props/TreeGreen.png", Vector2{2200,967}, PropType::TREE},     // 23
            Prop{"sprites/props/TreeGreen.png", Vector2{2300,967}, PropType::TREE},     // 24
            Prop{"sprites/props/TreeGreen.png", Vector2{2400,967}, PropType::TREE},     // 25
            Prop{"sprites/props/TreeGreen.png", Vector2{2500,967}, PropType::TREE},     // 26
            Prop{"sprites/props/TreeGreen.png", Vector2{2600,967}, PropType::TREE},     // 27
            // Prop{"sprites/props/TreeGreen.png", Vector2{2700,967}, PropType::TREE},     // 28
            // Prop{"sprites/props/TreeGreen.png", Vector2{2800,967}, PropType::TREE},     // 29
            // Prop{"sprites/props/TreeGreen.png", Vector2{2900,967}, PropType::TREE},     // 30
            // Prop{"sprites/props/TreeGreen.png", Vector2{3000,967}, PropType::TREE},     // 31
            // Prop{"sprites/props/TreeGreen.png", Vector2{3100,967}, PropType::TREE},     // 32
            Prop{"sprites/props/TreeGreen.png", Vector2{3200,967}, PropType::TREE},     // 33
            Prop{"sprites/props/TreeGreen.png", Vector2{3300,967}, PropType::TREE},     // 34
            Prop{"sprites/props/TreeGreen.png", Vector2{3400,967}, PropType::TREE},     // 35

            Prop{"sprites/props/TreeGreen.png", Vector2{3720,967}, PropType::TREE},     // 36
            Prop{"sprites/props/TreeGreen.png", Vector2{3820,967}, PropType::TREE},     // 37
            Prop{"sprites/props/TreeGreen.png", Vector2{3920,967}, PropType::TREE},     // 38

            // row11
            Prop{"sprites/props/TreeGreen.png", Vector2{20,1067}, PropType::TREE},      // 1 
            Prop{"sprites/props/TreeGreen.png", Vector2{120,1067}, PropType::TREE},     // 2     
            Prop{"sprites/props/TreeGreen.png", Vector2{220,1067}, PropType::TREE},     // 3     
            Prop{"sprites/props/TreeGreen.png", Vector2{320,1067}, PropType::TREE},     // 4 
            Prop{"sprites/props/TreeGreen.png", Vector2{420,1067}, PropType::TREE},     // 5 
            // Prop{"sprites/props/TreeGreen.png", Vector2{520,1067}, PropType::TREE},     // 6 
            // Prop{"sprites/props/TreeGreen.png", Vector2{620,1067}, PropType::TREE},     // 7 
            // Prop{"sprites/props/TreeGreen.png", Vector2{720,1067}, PropType::TREE},     // 8 
            // Prop{"sprites/props/TreeGreen.png", Vector2{820,1067}, PropType::TREE},     // 9 
            // Prop{"sprites/props/TreeGreen.png", Vector2{920,1067}, PropType::TREE},     // 10
            // Prop{"sprites/props/TreeGreen.png", Vector2{1020,1067}, PropType::TREE},    // 11    
            // Prop{"sprites/props/TreeGreen.png", Vector2{1120,1067}, PropType::TREE},    // 12    
            // Prop{"sprites/props/TreeGreen.png", Vector2{1220,1067}, PropType::TREE},    // 13    
            Prop{"sprites/props/TreeGreen.png", Vector2{1320,1067}, PropType::TREE},    // 14    
            Prop{"sprites/props/TreeGreen.png", Vector2{1420,1067}, PropType::TREE},    // 15    
            Prop{"sprites/props/TreeGreen.png", Vector2{1520,1067}, PropType::TREE},    // 16    
            Prop{"sprites/props/TreeGreen.png", Vector2{1620,1067}, PropType::TREE},    // 17    
            Prop{"sprites/props/TreeGreen.png", Vector2{1720,1067}, PropType::TREE},    // 18    
            Prop{"sprites/props/TreeGreen.png", Vector2{1820,1067}, PropType::TREE},    // 19    
            Prop{"sprites/props/TreeGreen.png", Vector2{1920,1067}, PropType::TREE},    // 20    
            Prop{"sprites/props/TreeGreen.png", Vector2{2020,1067}, PropType::TREE},    // 21    
            Prop{"sprites/props/TreeGreen.png", Vector2{2120,1067}, PropType::TREE},    // 22    
            Prop{"sprites/props/TreeGreen.png", Vector2{2220,1067}, PropType::TREE},    // 23    
            Prop{"sprites/props/TreeGreen.png", Vector2{2320,1067}, PropType::TREE},    // 24    
            Prop{"sprites/props/TreeGreen.png", Vector2{2420,1067}, PropType::TREE},    // 25    
            Prop{"sprites/props/TreeGreen.png", Vector2{2520,1067}, PropType::TREE},    // 26    
            Prop{"sprites/props/TreeGreen.png", Vector2{2620,1067}, PropType::TREE},    // 27    
            Prop{"sprites/props/TreeGreen.png", Vector2{2720,1067}, PropType::TREE},    // 28    
            // Prop{"sprites/props/TreeGreen.png", Vector2{2820,1067}, PropType::TREE},    // 29    
            // Prop{"sprites/props/TreeGreen.png", Vector2{2920,1067}, PropType::TREE},    // 30    
            // Prop{"sprites/props/TreeGreen.png", Vector2{3020,1067}, PropType::TREE},    // 31    
            Prop{"sprites/props/TreeGreen.png", Vector2{3120,1067}, PropType::TREE},    // 32    
            Prop{"sprites/props/TreeGreen.png", Vector2{3220,1067}, PropType::TREE},    // 33    
            Prop{"sprites/props/TreeGreen.png", Vector2{3320,1067}, PropType::TREE},    // 34    
            Prop{"sprites/props/TreeGreen.png", Vector2{3420,1067}, PropType::TREE},    // 35    

            Prop{"sprites/props/TreeGreen.png", Vector2{3700,1067}, PropType::TREE},    // 36    
            Prop{"sprites/props/TreeGreen.png", Vector2{3800,1067}, PropType::TREE},    // 37    
            Prop{"sprites/props/TreeGreen.png", Vector2{3900,1067}, PropType::TREE},    // 38    
            Prop{"sprites/props/TreeGreen.png", Vector2{4000,1067}, PropType::TREE},    // 39

            // row12
            Prop{"sprites/props/TreeGreen.png", Vector2{0,1167}, PropType::TREE},       // 1 
            Prop{"sprites/props/TreeGreen.png", Vector2{100,1167}, PropType::TREE},     // 2 
            Prop{"sprites/props/TreeGreen.png", Vector2{200,1167}, PropType::TREE},     // 3 
            Prop{"sprites/props/TreeGreen.png", Vector2{300,1167}, PropType::TREE},     // 4 
            Prop{"sprites/props/TreeGreen.png", Vector2{400,1167}, PropType::TREE},     // 5 
            // Prop{"sprites/props/TreeGreen.png", Vector2{500,1167}, PropType::TREE},     // 6 
            // Prop{"sprites/props/TreeGreen.png", Vector2{600,1167}, PropType::TREE},     // 7 
            // Prop{"sprites/props/TreeGreen.png", Vector2{700,1167}, PropType::TREE},     // 8 
            // Prop{"sprites/props/TreeGreen.png", Vector2{800,1167}, PropType::TREE},     // 9 
            // Prop{"sprites/props/TreeGreen.png", Vector2{900,1167}, PropType::TREE},     // 10
            // Prop{"sprites/props/TreeGreen.png", Vector2{1000,1167}, PropType::TREE},    // 11    
            Prop{"sprites/props/TreeGreen.png", Vector2{1100,1167}, PropType::TREE},    // 12    
            Prop{"sprites/props/TreeGreen.png", Vector2{1200,1167}, PropType::TREE},    // 13    
            Prop{"sprites/props/TreeGreen.png", Vector2{1300,1167}, PropType::TREE},    // 14    
            Prop{"sprites/props/TreeGreen.png", Vector2{1400,1167}, PropType::TREE},    // 15    
            Prop{"sprites/props/TreeGreen.png", Vector2{1500,1167}, PropType::TREE},    // 16    
            Prop{"sprites/props/TreeGreen.png", Vector2{1600,1167}, PropType::TREE},    // 17    
            Prop{"sprites/props/TreeGreen.png", Vector2{1700,1167}, PropType::TREE},    // 18    
            Prop{"sprites/props/TreeGreen.png", Vector2{1800,1167}, PropType::TREE},    // 19    
            Prop{"sprites/props/TreeGreen.png", Vector2{1900,1167}, PropType::TREE},    // 20    
            Prop{"sprites/props/TreeGreen.png", Vector2{2000,1167}, PropType::TREE},    // 21    
            Prop{"sprites/props/TreeGreen.png", Vector2{2100,1167}, PropType::TREE},    // 22    
            Prop{"sprites/props/TreeGreen.png", Vector2{2200,1167}, PropType::TREE},    // 23    
            Prop{"sprites/props/TreeGreen.png", Vector2{2300,1167}, PropType::TREE},    // 24    
            Prop{"sprites/props/TreeGreen.png", Vector2{2400,1167}, PropType::TREE},    // 25    
            Prop{"sprites/props/TreeGreen.png", Vector2{2500,1167}, PropType::TREE},    // 26    
            // Prop{"sprites/props/TreeGreen.png", Vector2{2600,1167}, PropType::TREE},    // 27    
            // Prop{"sprites/props/TreeGreen.png", Vector2{2700,1167}, PropType::TREE},    // 28    
            Prop{"sprites/props/TreeGreen.png", Vector2{2800,1167}, PropType::TREE},    // 29    
            Prop{"sprites/props/TreeGreen.png", Vector2{2900,1167}, PropType::TREE},    // 30    
            Prop{"sprites/props/TreeGreen.png", Vector2{3000,1167}, PropType::TREE},    // 31    
            Prop{"sprites/props/TreeGreen.png", Vector2{3100,1167}, PropType::TREE},    // 32    
            // Prop{"sprites/props/TreeGreen.png", Vector2{3200,1167}, PropType::TREE},    // 33    
            Prop{"sprites/props/TreeGreen.png", Vector2{3300,1167}, PropType::TREE},    // 34    
            Prop{"sprites/props/TreeGreen.png", Vector2{3400,1167}, PropType::TREE},    // 35    

            Prop{"sprites/props/TreeGreen.png", Vector2{3720,1167}, PropType::TREE},    // 36    
            Prop{"sprites/props/TreeGreen.png", Vector2{3820,1167}, PropType::TREE},    // 37    
            Prop{"sprites/props/TreeGreen.png", Vector2{3920,1167}, PropType::TREE},    // 38    

            // row13
            Prop{"sprites/props/TreeGreen.png", Vector2{20,1267}, PropType::TREE},      // 1 
            Prop{"sprites/props/TreeGreen.png", Vector2{120,1267}, PropType::TREE},     // 2 
            Prop{"sprites/props/TreeGreen.png", Vector2{220,1267}, PropType::TREE},     // 3 
            Prop{"sprites/props/TreeGreen.png", Vector2{320,1267}, PropType::TREE},     // 4 
            Prop{"sprites/props/TreeGreen.png", Vector2{420,1267}, PropType::TREE},     // 5 
            Prop{"sprites/props/TreeGreen.png", Vector2{520,1267}, PropType::TREE},     // 6 
            // Prop{"sprites/props/TreeGreen.png", Vector2{620,1267}, PropType::TREE},     // 7 
            // Prop{"sprites/props/TreeGreen.png", Vector2{720,1267}, PropType::TREE},     // 8 
            // Prop{"sprites/props/TreeGreen.png", Vector2{820,1267}, PropType::TREE},     // 9 
            // Prop{"sprites/props/TreeGreen.png", Vector2{920,1267}, PropType::TREE},     // 10
            Prop{"sprites/props/TreeGreen.png", Vector2{1020,1267}, PropType::TREE},    // 11
            Prop{"sprites/props/TreeGreen.png", Vector2{1120,1267}, PropType::TREE},    // 12
            Prop{"sprites/props/TreeGreen.png", Vector2{1220,1267}, PropType::TREE},    // 13
            Prop{"sprites/props/TreeGreen.png", Vector2{1320,1267}, PropType::TREE},    // 14
            Prop{"sprites/props/TreeGreen.png", Vector2{1420,1267}, PropType::TREE},    // 15
            Prop{"sprites/props/TreeGreen.png", Vector2{1520,1267}, PropType::TREE},    // 16
            Prop{"sprites/props/TreeGreen.png", Vector2{1620,1267}, PropType::TREE},    // 17
            Prop{"sprites/props/TreeGreen.png", Vector2{1720,1267}, PropType::TREE},    // 18
            Prop{"sprites/props/TreeGreen.png", Vector2{1820,1267}, PropType::TREE},    // 19
            Prop{"sprites/props/TreeGreen.png", Vector2{1920,1267}, PropType::TREE},    // 20
            // Prop{"sprites/props/TreeGreen.png", Vector2{2020,1267}, PropType::TREE},    // 21
            // Prop{"sprites/props/TreeGreen.png", Vector2{2120,1267}, PropType::TREE},    // 22
            // Prop{"sprites/props/TreeGreen.png", Vector2{2220,1267}, PropType::TREE},    // 23
            // Prop{"sprites/props/TreeGreen.png", Vector2{2320,1267}, PropType::TREE},    // 24
            Prop{"sprites/props/TreeGreen.png", Vector2{2420,1267}, PropType::TREE},    // 25
            Prop{"sprites/props/TreeGreen.png", Vector2{2520,1267}, PropType::TREE},    // 26
            Prop{"sprites/props/TreeGreen.png", Vector2{2620,1267}, PropType::TREE},    // 27
            Prop{"sprites/props/TreeGreen.png", Vector2{2720,1267}, PropType::TREE},    // 28
            Prop{"sprites/props/TreeGreen.png", Vector2{2820,1267}, PropType::TREE},    // 29
            Prop{"sprites/props/TreeGreen.png", Vector2{2920,1267}, PropType::TREE},    // 30
            Prop{"sprites/props/TreeGreen.png", Vector2{3020,1267}, PropType::TREE},    // 31
            Prop{"sprites/props/TreeGreen.png", Vector2{3120,1267}, PropType::TREE},    // 32
            Prop{"sprites/props/TreeGreen.png", Vector2{3220,1267}, PropType::TREE},    // 33
            Prop{"sprites/props/TreeGreen.png", Vector2{3320,1267}, PropType::TREE},    // 34
            Prop{"sprites/props/TreeGreen.png", Vector2{3420,1267}, PropType::TREE},    // 35

            Prop{"sprites/props/TreeGreen.png", Vector2{3700,1267}, PropType::TREE},    // 36
            Prop{"sprites/props/TreeGreen.png", Vector2{3800,1267}, PropType::TREE},    // 37
            Prop{"sprites/props/TreeGreen.png", Vector2{3900,1267}, PropType::TREE},    // 38
            Prop{"sprites/props/TreeGreen.png", Vector2{4000,1267}, PropType::TREE},    // 39

            // row14
            Prop{"sprites/props/TreeGreen.png", Vector2{0,1367}, PropType::TREE},       // 1 
            Prop{"sprites/props/TreeGreen.png", Vector2{100,1367}, PropType::TREE},     // 2     
            Prop{"sprites/props/TreeGreen.png", Vector2{200,1367}, PropType::TREE},     // 3     
            Prop{"sprites/props/TreeGreen.png", Vector2{300,1367}, PropType::TREE},     // 4     
            Prop{"sprites/props/TreeGreen.png", Vector2{400,1367}, PropType::TREE},     // 5     
            Prop{"sprites/props/TreeGreen.png", Vector2{500,1367}, PropType::TREE},     // 6     
            // Prop{"sprites/props/TreeGreen.png", Vector2{600,1367}, PropType::TREE},     // 7     
            Prop{"sprites/props/TreeGreen.png", Vector2{700,1367}, PropType::TREE},     // 8     
            Prop{"sprites/props/TreeGreen.png", Vector2{800,1367}, PropType::TREE},     // 9     
            Prop{"sprites/props/TreeGreen.png", Vector2{900,1367}, PropType::TREE},     // 10    
            Prop{"sprites/props/TreeGreen.png", Vector2{1000,1367}, PropType::TREE},    // 11    
            Prop{"sprites/props/TreeGreen.png", Vector2{1100,1367}, PropType::TREE},    // 12    
            Prop{"sprites/props/TreeGreen.png", Vector2{1200,1367}, PropType::TREE},    // 13    
            Prop{"sprites/props/TreeGreen.png", Vector2{1300,1367}, PropType::TREE},    // 14    
            Prop{"sprites/props/TreeGreen.png", Vector2{1400,1367}, PropType::TREE},    // 15    
            Prop{"sprites/props/TreeGreen.png", Vector2{1500,1367}, PropType::TREE},    // 16    
            Prop{"sprites/props/TreeGreen.png", Vector2{1600,1367}, PropType::TREE},    // 17    
            Prop{"sprites/props/TreeGreen.png", Vector2{1700,1367}, PropType::TREE},    // 18    
            Prop{"sprites/props/TreeGreen.png", Vector2{1800,1367}, PropType::TREE},    // 19    
            // Prop{"sprites/props/TreeGreen.png", Vector2{1900,1367}, PropType::TREE},    // 20    
            // Prop{"sprites/props/TreeGreen.png", Vector2{2000,1367}, PropType::TREE},    // 21    
            // Prop{"sprites/props/TreeGreen.png", Vector2{2100,1367}, PropType::TREE},    // 22    
            // Prop{"sprites/props/TreeGreen.png", Vector2{2200,1367}, PropType::TREE},    // 23    
            // Prop{"sprites/props/TreeGreen.png", Vector2{2300,1367}, PropType::TREE},    // 24    
            // Prop{"sprites/props/TreeGreen.png", Vector2{2400,1367}, PropType::TREE},    // 25    
            Prop{"sprites/props/TreeGreen.png", Vector2{2500,1367}, PropType::TREE},    // 26    
            Prop{"sprites/props/TreeGreen.png", Vector2{2600,1367}, PropType::TREE},    // 27    
            Prop{"sprites/props/TreeGreen.png", Vector2{2700,1367}, PropType::TREE},    // 28    
            Prop{"sprites/props/TreeGreen.png", Vector2{2800,1367}, PropType::TREE},    // 29    
            Prop{"sprites/props/TreeGreen.png", Vector2{2900,1367}, PropType::TREE},    // 30    
            // Prop{"sprites/props/TreeGreen.png", Vector2{3000,1367}, PropType::TREE},    // 31    
            Prop{"sprites/props/TreeGreen.png", Vector2{3100,1367}, PropType::TREE},    // 32    
            Prop{"sprites/props/TreeGreen.png", Vector2{3200,1367}, PropType::TREE},    // 33    
            Prop{"sprites/props/TreeGreen.png", Vector2{3300,1367}, PropType::TREE},    // 34    
            Prop{"sprites/props/TreeGreen.png", Vector2{3400,1367}, PropType::TREE},    // 35    

            Prop{"sprites/props/TreeGreen.png", Vector2{3720,1367}, PropType::TREE},    // 36    
            Prop{"sprites/props/TreeGreen.png", Vector2{3820,1367}, PropType::TREE},    // 37    
            Prop{"sprites/props/TreeGreen.png", Vector2{3920,1367}, PropType::TREE},    // 38    

            // row15
            Prop{"sprites/props/TreeGreen.png", Vector2{20,1467}, PropType::TREE},      // 1 
            Prop{"sprites/props/TreeGreen.png", Vector2{120,1467}, PropType::TREE},     // 2 
            Prop{"sprites/props/TreeGreen.png", Vector2{220,1467}, PropType::TREE},     // 3 
            Prop{"sprites/props/TreeGreen.png", Vector2{320,1467}, PropType::TREE},     // 4 
            // Prop{"sprites/props/TreeGreen.png", Vector2{420,1467}, PropType::TREE},     // 5 
            Prop{"sprites/props/TreeGreen.png", Vector2{520,1467}, PropType::TREE},     // 6 
            // Prop{"sprites/props/TreeGreen.png", Vector2{620,1467}, PropType::TREE},     // 7 
            Prop{"sprites/props/TreeGreen.png", Vector2{720,1467}, PropType::TREE},     // 8 
            Prop{"sprites/props/TreeGreen.png", Vector2{820,1467}, PropType::TREE},     // 9 
            Prop{"sprites/props/TreeGreen.png", Vector2{920,1467}, PropType::TREE},     // 10
            // Prop{"sprites/props/TreeGreen.png", Vector2{1020,1467}, PropType::TREE},    // 11
            // Prop{"sprites/props/TreeGreen.png", Vector2{1120,1467}, PropType::TREE},    // 12
            Prop{"sprites/props/TreeGreen.png", Vector2{1220,1467}, PropType::TREE},    // 13
            Prop{"sprites/props/TreeGreen.png", Vector2{1320,1467}, PropType::TREE},    // 14
            Prop{"sprites/props/TreeGreen.png", Vector2{1420,1467}, PropType::TREE},    // 15
            Prop{"sprites/props/TreeGreen.png", Vector2{1520,1467}, PropType::TREE},    // 16
            Prop{"sprites/props/TreeGreen.png", Vector2{1620,1467}, PropType::TREE},    // 17
            Prop{"sprites/props/TreeGreen.png", Vector2{1720,1467}, PropType::TREE},    // 18
            Prop{"sprites/props/TreeGreen.png", Vector2{1820,1467}, PropType::TREE},    // 19
            // Prop{"sprites/props/TreeGreen.png", Vector2{1920,1467}, PropType::TREE},    // 20
            // Prop{"sprites/props/TreeGreen.png", Vector2{2020,1467}, PropType::TREE},    // 21
            // Prop{"sprites/props/TreeGreen.png", Vector2{2120,1467}, PropType::TREE},    // 22
            // Prop{"sprites/props/TreeGreen.png", Vector2{2220,1467}, PropType::TREE},    // 23
            // Prop{"sprites/props/TreeGreen.png", Vector2{2320,1467}, PropType::TREE},    // 24
            Prop{"sprites/props/TreeGreen.png", Vector2{2420,1467}, PropType::TREE},    // 25
            Prop{"sprites/props/TreeGreen.png", Vector2{2520,1467}, PropType::TREE},    // 26
            Prop{"sprites/props/TreeGreen.png", Vector2{2620,1467}, PropType::TREE},    // 27
            Prop{"sprites/props/TreeGreen.png", Vector2{2720,1467}, PropType::TREE},    // 28
            Prop{"sprites/props/TreeGreen.png", Vector2{2820,1467}, PropType::TREE},    // 29
            Prop{"sprites/props/TreeGreen.png", Vector2{2920,1467}, PropType::TREE},    // 30
            Prop{"sprites/props/TreeGreen.png", Vector2{3020,1467}, PropType::TREE},    // 31
            Prop{"sprites/props/TreeGreen.png", Vector2{3120,1467}, PropType::TREE},    // 32
            Prop{"sprites/props/TreeGreen.png", Vector2{3220,1467}, PropType::TREE},    // 33
            Prop{"sprites/props/TreeGreen.png", Vector2{3320,1467}, PropType::TREE},    // 34
            Prop{"sprites/props/TreeGreen.png", Vector2{3420,1467}, PropType::TREE},    // 35

            Prop{"sprites/props/TreeGreen.png", Vector2{3700,1467}, PropType::TREE},    // 36
            Prop{"sprites/props/TreeGreen.png", Vector2{3800,1467}, PropType::TREE},    // 37
            Prop{"sprites/props/TreeGreen.png", Vector2{3900,1467}, PropType::TREE},    // 38
            Prop{"sprites/props/TreeGreen.png", Vector2{4000,1467}, PropType::TREE},    // 39

            // row16
            Prop{"sprites/props/TreeGreen.png", Vector2{0,1567}, PropType::TREE},       // 1 
            Prop{"sprites/props/TreeGreen.png", Vector2{100,1567}, PropType::TREE},     // 2 
            Prop{"sprites/props/TreeGreen.png", Vector2{200,1567}, PropType::TREE},     // 3 
            // Prop{"sprites/props/TreeGreen.png", Vector2{300,1567}, PropType::TREE},     // 4 
            Prop{"sprites/props/TreeGreen.png", Vector2{400,1567}, PropType::TREE},     // 5 
            Prop{"sprites/props/TreeGreen.png", Vector2{500,1567}, PropType::TREE},     // 6 
            Prop{"sprites/props/TreeGreen.png", Vector2{600,1567}, PropType::TREE},     // 7 
            Prop{"sprites/props/TreeGreen.png", Vector2{700,1567}, PropType::TREE},     // 8 
            // Prop{"sprites/props/TreeGreen.png", Vector2{800,1567}, PropType::TREE},     // 9 
            Prop{"sprites/props/TreeGreen.png", Vector2{900,1567}, PropType::TREE},     // 10
            Prop{"sprites/props/TreeGreen.png", Vector2{1000,1567}, PropType::TREE},    // 11    
            Prop{"sprites/props/TreeGreen.png", Vector2{1100,1567}, PropType::TREE},    // 12
            Prop{"sprites/props/TreeGreen.png", Vector2{1200,1567}, PropType::TREE},    // 13
            Prop{"sprites/props/TreeGreen.png", Vector2{1300,1567}, PropType::TREE},    // 14
            Prop{"sprites/props/TreeGreen.png", Vector2{1400,1567}, PropType::TREE},    // 15
            Prop{"sprites/props/TreeGreen.png", Vector2{1500,1567}, PropType::TREE},    // 16
            Prop{"sprites/props/TreeGreen.png", Vector2{1600,1567}, PropType::TREE},    // 17
            // Prop{"sprites/props/TreeGreen.png", Vector2{1700,1567}, PropType::TREE},    // 18
            // Prop{"sprites/props/TreeGreen.png", Vector2{1800,1567}, PropType::TREE},    // 19
            // Prop{"sprites/props/TreeGreen.png", Vector2{1900,1567}, PropType::TREE},    // 20
            // Prop{"sprites/props/TreeGreen.png", Vector2{2000,1567}, PropType::TREE},    // 21
            // Prop{"sprites/props/TreeGreen.png", Vector2{2100,1567}, PropType::TREE},    // 22
            // Prop{"sprites/props/TreeGreen.png", Vector2{2200,1567}, PropType::TREE},    // 23
            // Prop{"sprites/props/TreeGreen.png", Vector2{2300,1567}, PropType::TREE},    // 24
            // Prop{"sprites/props/TreeGreen.png", Vector2{2400,1567}, PropType::TREE},    // 25
            Prop{"sprites/props/TreeGreen.png", Vector2{2500,1567}, PropType::TREE},    // 26
            Prop{"sprites/props/TreeGreen.png", Vector2{2600,1567}, PropType::TREE},    // 27
            Prop{"sprites/props/TreeGreen.png", Vector2{2700,1567}, PropType::TREE},    // 28
            Prop{"sprites/props/TreeGreen.png", Vector2{2800,1567}, PropType::TREE},    // 29
            Prop{"sprites/props/TreeGreen.png", Vector2{2900,1567}, PropType::TREE},    // 30
            Prop{"sprites/props/TreeGreen.png", Vector2{3000,1567}, PropType::TREE},    // 31
            Prop{"sprites/props/TreeGreen.png", Vector2{3100,1567}, PropType::TREE},    // 32
            Prop{"sprites/props/TreeGreen.png", Vector2{3200,1567}, PropType::TREE},    // 33
            Prop{"sprites/props/TreeGreen.png", Vector2{3300,1567}, PropType::TREE},    // 34
            Prop{"sprites/props/TreeGreen.png", Vector2{3400,1567}, PropType::TREE},    // 35

            Prop{"sprites/props/TreeGreen.png", Vector2{3720,1567}, PropType::TREE},    // 36 
            Prop{"sprites/props/TreeGreen.png", Vector2{3820,1567}, PropType::TREE},    // 37
            Prop{"sprites/props/TreeGreen.png", Vector2{3920,1567}, PropType::TREE},    // 38

            // row17
            Prop{"sprites/props/TreeGreen.png", Vector2{20,1667}, PropType::TREE},      // 1 
            Prop{"sprites/props/TreeGreen.png", Vector2{120,1667}, PropType::TREE},     // 2 
            Prop{"sprites/props/TreeGreen.png", Vector2{220,1667}, PropType::TREE},     // 3 
            // Prop{"sprites/props/TreeGreen.png", Vector2{320,1667}, PropType::TREE},     // 4 
            // Prop{"sprites/props/TreeGreen.png", Vector2{420,1667}, PropType::TREE},     // 5 
            Prop{"sprites/props/TreeGreen.png", Vector2{520,1667}, PropType::TREE},     // 6 
            Prop{"sprites/props/TreeGreen.png", Vector2{620,1667}, PropType::TREE},     // 7 
            Prop{"sprites/props/TreeGreen.png", Vector2{720,1667}, PropType::TREE},     // 8 
            Prop{"sprites/props/TreeGreen.png", Vector2{820,1667}, PropType::TREE},     // 9 
            Prop{"sprites/props/TreeGreen.png", Vector2{920,1667}, PropType::TREE},     // 10
            // Prop{"sprites/props/TreeGreen.png", Vector2{1020,1667}, PropType::TREE},    // 11
            Prop{"sprites/props/TreeGreen.png", Vector2{1120,1667}, PropType::TREE},    // 12
            Prop{"sprites/props/TreeGreen.png", Vector2{1220,1667}, PropType::TREE},    // 13
            Prop{"sprites/props/TreeGreen.png", Vector2{1320,1667}, PropType::TREE},    // 14
            // Prop{"sprites/props/TreeGreen.png", Vector2{1420,1667}, PropType::TREE},    // 15
            // Prop{"sprites/props/TreeGreen.png", Vector2{1520,1667}, PropType::TREE},    // 16
            // Prop{"sprites/props/TreeGreen.png", Vector2{1620,1667}, PropType::TREE},    // 17
            // Prop{"sprites/props/TreeGreen.png", Vector2{1720,1667}, PropType::TREE},    // 18
            // Prop{"sprites/props/TreeGreen.png", Vector2{1820,1667}, PropType::TREE},    // 19
            // Prop{"sprites/props/TreeGreen.png", Vector2{1920,1667}, PropType::TREE},    // 20
            // Prop{"sprites/props/TreeGreen.png", Vector2{2020,1667}, PropType::TREE},    // 21
            // Prop{"sprites/props/TreeGreen.png", Vector2{2120,1667}, PropType::TREE},    // 22
            // Prop{"sprites/props/TreeGreen.png", Vector2{2220,1667}, PropType::TREE},    // 23
            // Prop{"sprites/props/TreeGreen.png", Vector2{2320,1667}, PropType::TREE},    // 24
            // Prop{"sprites/props/TreeGreen.png", Vector2{2420,1667}, PropType::TREE},    // 25
            // Prop{"sprites/props/TreeGreen.png", Vector2{2520,1667}, PropType::TREE},    // 26
            // Prop{"sprites/props/TreeGreen.png", Vector2{2620,1667}, PropType::TREE},    // 27
            // Prop{"sprites/props/TreeGreen.png", Vector2{2720,1667}, PropType::TREE},    // 28
            // Prop{"sprites/props/TreeGreen.png", Vector2{2820,1667}, PropType::TREE},    // 29
            Prop{"sprites/props/TreeGreen.png", Vector2{2920,1667}, PropType::TREE},    // 30
            Prop{"sprites/props/TreeGreen.png", Vector2{3020,1667}, PropType::TREE},    // 31
            Prop{"sprites/props/TreeGreen.png", Vector2{3120,1667}, PropType::TREE},    // 32
            Prop{"sprites/props/TreeGreen.png", Vector2{3220,1667}, PropType::TREE},    // 33
            Prop{"sprites/props/TreeGreen.png", Vector2{3320,1667}, PropType::TREE},    // 34
            Prop{"sprites/props/TreeGreen.png", Vector2{3420,1667}, PropType::TREE},    // 35

            Prop{"sprites/props/TreeGreen.png", Vector2{3700,1667}, PropType::TREE},    // 36
            Prop{"sprites/props/TreeGreen.png", Vector2{3800,1667}, PropType::TREE},    // 37
            Prop{"sprites/props/TreeGreen.png", Vector2{3900,1667}, PropType::TREE},    // 38
            Prop{"sprites/props/TreeGreen.png", Vector2{4000,1667}, PropType::TREE},    // 39

            // row18
            Prop{"sprites/props/TreeGreen.png", Vector2{0,1767}, PropType::TREE},       // 1 
            Prop{"sprites/props/TreeGreen.png", Vector2{100,1767}, PropType::TREE},     // 2 
            Prop{"sprites/props/TreeGreen.png", Vector2{200,1767}, PropType::TREE},     // 3 
            Prop{"sprites/props/TreeGreen.png", Vector2{300,1767}, PropType::TREE},     // 4 
            Prop{"sprites/props/TreeGreen.png", Vector2{400,1767}, PropType::TREE},     // 5 
            Prop{"sprites/props/TreeGreen.png", Vector2{500,1767}, PropType::TREE},     // 6 
            Prop{"sprites/props/TreeGreen.png", Vector2{600,1767}, PropType::TREE},     // 7 
            Prop{"sprites/props/TreeGreen.png", Vector2{700,1767}, PropType::TREE},     // 8 
            Prop{"sprites/props/TreeGreen.png", Vector2{800,1767}, PropType::TREE},     // 9 
            Prop{"sprites/props/TreeGreen.png", Vector2{900,1767}, PropType::TREE},     // 10
            Prop{"sprites/props/TreeGreen.png", Vector2{1000,1767}, PropType::TREE},    // 11
            Prop{"sprites/props/TreeGreen.png", Vector2{1100,1767}, PropType::TREE},    // 12
            Prop{"sprites/props/TreeGreen.png", Vector2{1200,1767}, PropType::TREE},    // 13
            Prop{"sprites/props/TreeGreen.png", Vector2{1300,1767}, PropType::TREE},    // 14
            Prop{"sprites/props/TreeGreen.png", Vector2{1400,1767}, PropType::TREE},    // 15
            // Prop{"sprites/props/TreeGreen.png", Vector2{1500,1767}, PropType::TREE},    // 16
            // Prop{"sprites/props/TreeGreen.png", Vector2{1600,1767}, PropType::TREE},    // 17
            // Prop{"sprites/props/TreeGreen.png", Vector2{1700,1767}, PropType::TREE},    // 18
            // Prop{"sprites/props/TreeGreen.png", Vector2{1800,1767}, PropType::TREE},    // 19
            // Prop{"sprites/props/TreeGreen.png", Vector2{1900,1767}, PropType::TREE},    // 20
            // Prop{"sprites/props/TreeGreen.png", Vector2{2000,1767}, PropType::TREE},    // 21
            // Prop{"sprites/props/TreeGreen.png", Vector2{2100,1767}, PropType::TREE},    // 22
            // Prop{"sprites/props/TreeGreen.png", Vector2{2200,1767}, PropType::TREE},    // 23
            // Prop{"sprites/props/TreeGreen.png", Vector2{2300,1767}, PropType::TREE},    // 24
            // Prop{"sprites/props/TreeGreen.png", Vector2{2400,1767}, PropType::TREE},    // 25
            Prop{"sprites/props/TreeGreen.png", Vector2{2500,1767}, PropType::TREE},    // 26
            Prop{"sprites/props/TreeGreen.png", Vector2{2600,1767}, PropType::TREE},    // 27
            Prop{"sprites/props/TreeGreen.png", Vector2{2700,1767}, PropType::TREE},    // 28
            Prop{"sprites/props/TreeGreen.png", Vector2{2800,1767}, PropType::TREE},    // 29
            Prop{"sprites/props/TreeGreen.png", Vector2{2900,1767}, PropType::TREE},    // 30
            Prop{"sprites/props/TreeGreen.png", Vector2{3000,1767}, PropType::TREE},    // 31
            Prop{"sprites/props/TreeGreen.png", Vector2{3100,1767}, PropType::TREE},    // 32
            Prop{"sprites/props/TreeGreen.png", Vector2{3200,1767}, PropType::TREE},    // 33
            Prop{"sprites/props/TreeGreen.png", Vector2{3300,1767}, PropType::TREE},    // 34
            Prop{"sprites/props/TreeGreen.png", Vector2{3400,1767}, PropType::TREE},    // 35

            Prop{"sprites/props/TreeGreen.png", Vector2{3720,1767}, PropType::TREE},    // 36
            Prop{"sprites/props/TreeGreen.png", Vector2{3820,1767}, PropType::TREE},    // 37
            Prop{"sprites/props/TreeGreen.png", Vector2{3920,1767}, PropType::TREE},    // 38

            // row19
            Prop{"sprites/props/TreeGreen.png", Vector2{20,1867}, PropType::TREE},      // 1     
            // Prop{"sprites/props/TreeGreen.png", Vector2{120,1867}, PropType::TREE},     // 2     
            Prop{"sprites/props/TreeGreen.png", Vector2{220,1867}, PropType::TREE},     // 3     
            Prop{"sprites/props/TreeGreen.png", Vector2{320,1867}, PropType::TREE},     // 4 
            Prop{"sprites/props/TreeGreen.png", Vector2{420,1867}, PropType::TREE},     // 5 
            Prop{"sprites/props/TreeGreen.png", Vector2{520,1867}, PropType::TREE},     // 6 
            Prop{"sprites/props/TreeGreen.png", Vector2{620,1867}, PropType::TREE},     // 7 
            // Prop{"sprites/props/TreeGreen.png", Vector2{720,1867}, PropType::TREE},     // 8 
            Prop{"sprites/props/TreeGreen.png", Vector2{820,1867}, PropType::TREE},     // 9 
            Prop{"sprites/props/TreeGreen.png", Vector2{920,1867}, PropType::TREE},     // 10
            Prop{"sprites/props/TreeGreen.png", Vector2{1020,1867}, PropType::TREE},    // 11
            Prop{"sprites/props/TreeGreen.png", Vector2{1120,1867}, PropType::TREE},    // 12
            Prop{"sprites/props/TreeGreen.png", Vector2{1220,1867}, PropType::TREE},    // 13
            Prop{"sprites/props/TreeGreen.png", Vector2{1320,1867}, PropType::TREE},    // 14
            Prop{"sprites/props/TreeGreen.png", Vector2{1420,1867}, PropType::TREE},    // 15
            Prop{"sprites/props/TreeGreen.png", Vector2{1520,1867}, PropType::TREE},    // 16
            Prop{"sprites/props/TreeGreen.png", Vector2{1620,1867}, PropType::TREE},    // 17
            // Prop{"sprites/props/TreeGreen.png", Vector2{1720,1867}, PropType::TREE},    // 18
            // Prop{"sprites/props/TreeGreen.png", Vector2{1820,1867}, PropType::TREE},    // 19
            // Prop{"sprites/props/TreeGreen.png", Vector2{1920,1867}, PropType::TREE},    // 20
            // Prop{"sprites/props/TreeGreen.png", Vector2{2020,1867}, PropType::TREE},    // 21
            // Prop{"sprites/props/TreeGreen.png", Vector2{2120,1867}, PropType::TREE},    // 22
            // Prop{"sprites/props/TreeGreen.png", Vector2{2220,1867}, PropType::TREE},    // 23
            // Prop{"sprites/props/TreeGreen.png", Vector2{2320,1867}, PropType::TREE},    // 24
            // Prop{"sprites/props/TreeGreen.png", Vector2{2420,1867}, PropType::TREE},    // 25
            // Prop{"sprites/props/TreeGreen.png", Vector2{2520,1867}, PropType::TREE},    // 26
            // Prop{"sprites/props/TreeGreen.png", Vector2{2620,1867}, PropType::TREE},    // 27
            Prop{"sprites/props/TreeGreen.png", Vector2{2720,1867}, PropType::TREE},    // 28
            Prop{"sprites/props/TreeGreen.png", Vector2{2820,1867}, PropType::TREE},    // 29
            Prop{"sprites/props/TreeGreen.png", Vector2{2920,1867}, PropType::TREE},    // 30
            Prop{"sprites/props/TreeGreen.png", Vector2{3020,1867}, PropType::TREE},    // 31
            Prop{"sprites/props/TreeGreen.png", Vector2{3120,1867}, PropType::TREE},    // 32
            Prop{"sprites/props/TreeGreen.png", Vector2{3220,1867}, PropType::TREE},    // 33
            Prop{"sprites/props/TreeGreen.png", Vector2{3320,1867}, PropType::TREE},    // 34
            Prop{"sprites/props/TreeGreen.png", Vector2{3420,1867}, PropType::TREE},    // 35

            Prop{"sprites/props/TreeGreen.png", Vector2{3700,1867}, PropType::TREE},    // 36
            Prop{"sprites/props/TreeGreen.png", Vector2{3800,1867}, PropType::TREE},    // 37
            Prop{"sprites/props/TreeGreen.png", Vector2{3900,1867}, PropType::TREE},    // 38
            Prop{"sprites/props/TreeGreen.png", Vector2{4000,1867}, PropType::TREE},    // 39

            // row20
            Prop{"sprites/props/TreeGreen.png", Vector2{0,1967}, PropType::TREE},       // 1     
            Prop{"sprites/props/TreeGreen.png", Vector2{100,1967}, PropType::TREE},     // 2 
            Prop{"sprites/props/TreeGreen.png", Vector2{200,1967}, PropType::TREE},     // 3 
            // Prop{"sprites/props/TreeGreen.png", Vector2{300,1967}, PropType::TREE},     // 4 
            Prop{"sprites/props/TreeGreen.png", Vector2{400,1967}, PropType::TREE},     // 5 
            Prop{"sprites/props/TreeGreen.png", Vector2{500,1967}, PropType::TREE},     // 6 
            Prop{"sprites/props/TreeGreen.png", Vector2{600,1967}, PropType::TREE},     // 7 
            Prop{"sprites/props/TreeGreen.png", Vector2{700,1967}, PropType::TREE},     // 8 
            Prop{"sprites/props/TreeGreen.png", Vector2{800,1967}, PropType::TREE},     // 9 
            // Prop{"sprites/props/TreeGreen.png", Vector2{900,1967}, PropType::TREE},     // 10
            Prop{"sprites/props/TreeGreen.png", Vector2{1000,1967}, PropType::TREE},    // 11
            Prop{"sprites/props/TreeGreen.png", Vector2{1100,1967}, PropType::TREE},    // 12
            // Prop{"sprites/props/TreeGreen.png", Vector2{1200,1967}, PropType::TREE},    // 13
            Prop{"sprites/props/TreeGreen.png", Vector2{1300,1967}, PropType::TREE},    // 14
            Prop{"sprites/props/TreeGreen.png", Vector2{1400,1967}, PropType::TREE},    // 15
            Prop{"sprites/props/TreeGreen.png", Vector2{1500,1967}, PropType::TREE},    // 16
            Prop{"sprites/props/TreeGreen.png", Vector2{1600,1967}, PropType::TREE},    // 17
            // Prop{"sprites/props/TreeGreen.png", Vector2{1700,1967}, PropType::TREE},    // 18
            Prop{"sprites/props/TreeGreen.png", Vector2{1800,1967}, PropType::TREE},    // 19
            Prop{"sprites/props/TreeGreen.png", Vector2{1900,1967}, PropType::TREE},    // 20
            // Prop{"sprites/props/TreeGreen.png", Vector2{2000,1967}, PropType::TREE},    // 21
            // Prop{"sprites/props/TreeGreen.png", Vector2{2100,1967}, PropType::TREE},    // 22
            // Prop{"sprites/props/TreeGreen.png", Vector2{2200,1967}, PropType::TREE},    // 23
            // Prop{"sprites/props/TreeGreen.png", Vector2{2300,1967}, PropType::TREE},    // 24
            // Prop{"sprites/props/TreeGreen.png", Vector2{2400,1967}, PropType::TREE},    // 25
            // Prop{"sprites/props/TreeGreen.png", Vector2{2500,1967}, PropType::TREE},    // 26
            Prop{"sprites/props/TreeGreen.png", Vector2{2600,1967}, PropType::TREE},    // 27
            Prop{"sprites/props/TreeGreen.png", Vector2{2700,1967}, PropType::TREE},    // 28
            // Prop{"sprites/props/TreeGreen.png", Vector2{2800,1967}, PropType::TREE},    // 29
            Prop{"sprites/props/TreeGreen.png", Vector2{2900,1967}, PropType::TREE},    // 30
            Prop{"sprites/props/TreeGreen.png", Vector2{3000,1967}, PropType::TREE},    // 31
            Prop{"sprites/props/TreeGreen.png", Vector2{3100,1967}, PropType::TREE},    // 32
            Prop{"sprites/props/TreeGreen.png", Vector2{3200,1967}, PropType::TREE},    // 33
            // Prop{"sprites/props/TreeGreen.png", Vector2{3300,1967}, PropType::TREE},    // 34
            Prop{"sprites/props/TreeGreen.png", Vector2{3400,1967}, PropType::TREE},    // 35

            Prop{"sprites/props/TreeGreen.png", Vector2{3500,1967}, PropType::TREE},    // 35
            Prop{"sprites/props/TreeGreen.png", Vector2{3600,1967}, PropType::TREE},    // 35

            Prop{"sprites/props/TreeGreen.png", Vector2{3720,1967}, PropType::TREE},    // 36
            Prop{"sprites/props/TreeGreen.png", Vector2{3820,1967}, PropType::TREE},    // 37
            Prop{"sprites/props/TreeGreen.png", Vector2{3920,1967}, PropType::TREE},    // 38

            // row21
            Prop{"sprites/props/TreeGreen.png", Vector2{20,2067}, PropType::TREE},      // 1 
            // Prop{"sprites/props/TreeGreen.png", Vector2{120,2067}, PropType::TREE},     // 2 
            Prop{"sprites/props/TreeGreen.png", Vector2{220,2067}, PropType::TREE},     // 3 
            Prop{"sprites/props/TreeGreen.png", Vector2{320,2067}, PropType::TREE},     // 4 
            Prop{"sprites/props/TreeGreen.png", Vector2{420,2067}, PropType::TREE},     // 5 
            Prop{"sprites/props/TreeGreen.png", Vector2{520,2067}, PropType::TREE},     // 6 
            Prop{"sprites/props/TreeGreen.png", Vector2{620,2067}, PropType::TREE},     // 7 
            Prop{"sprites/props/TreeGreen.png", Vector2{720,2067}, PropType::TREE},     // 8 
            Prop{"sprites/props/TreeGreen.png", Vector2{820,2067}, PropType::TREE},     // 9 
            Prop{"sprites/props/TreeGreen.png", Vector2{920,2067}, PropType::TREE},     // 10
            Prop{"sprites/props/TreeGreen.png", Vector2{1020,2067}, PropType::TREE},    // 11
            Prop{"sprites/props/TreeGreen.png", Vector2{1120,2067}, PropType::TREE},    // 12
            // Prop{"sprites/props/TreeGreen.png", Vector2{1220,2067}, PropType::TREE},    // 13
            Prop{"sprites/props/TreeGreen.png", Vector2{1320,2067}, PropType::TREE},    // 14
            Prop{"sprites/props/TreeGreen.png", Vector2{1420,2067}, PropType::TREE},    // 15
            Prop{"sprites/props/TreeGreen.png", Vector2{1520,2067}, PropType::TREE},    // 16
            // Prop{"sprites/props/TreeGreen.png", Vector2{1620,2067}, PropType::TREE},    // 17
            Prop{"sprites/props/TreeGreen.png", Vector2{1720,2067}, PropType::TREE},    // 18
            Prop{"sprites/props/TreeGreen.png", Vector2{1820,2067}, PropType::TREE},    // 19
            // Prop{"sprites/props/TreeGreen.png", Vector2{1920,2067}, PropType::TREE},    // 20
            // Prop{"sprites/props/TreeGreen.png", Vector2{2020,2067}, PropType::TREE},    // 21
            // Prop{"sprites/props/TreeGreen.png", Vector2{2120,2067}, PropType::TREE},    // 22
            // Prop{"sprites/props/TreeGreen.png", Vector2{2220,2067}, PropType::TREE},    // 23
            // Prop{"sprites/props/TreeGreen.png", Vector2{2320,2067}, PropType::TREE},    // 24
            // Prop{"sprites/props/TreeGreen.png", Vector2{2420,2067}, PropType::TREE},    // 25
            // Prop{"sprites/props/TreeGreen.png", Vector2{2520,2067}, PropType::TREE},    // 26
            // Prop{"sprites/props/TreeGreen.png", Vector2{2620,2067}, PropType::TREE},    // 27
            Prop{"sprites/props/TreeGreen.png", Vector2{2720,2067}, PropType::TREE},    // 28
            Prop{"sprites/props/TreeGreen.png", Vector2{2820,2067}, PropType::TREE},    // 29
            Prop{"sprites/props/TreeGreen.png", Vector2{2920,2067}, PropType::TREE},    // 30
            // Prop{"sprites/props/TreeGreen.png", Vector2{3020,2067}, PropType::TREE},    // 31
            // Prop{"sprites/props/TreeGreen.png", Vector2{3120,2067}, PropType::TREE},    // 32
            Prop{"sprites/props/TreeGreen.png", Vector2{3220,2067}, PropType::TREE},    // 33
            Prop{"sprites/props/TreeGreen.png", Vector2{3320,2067}, PropType::TREE},    // 34
            Prop{"sprites/props/TreeGreen.png", Vector2{3420,2067}, PropType::TREE},    // 35

            Prop{"sprites/props/TreeGreen.png", Vector2{3520,2067}, PropType::TREE},    // 35
            Prop{"sprites/props/TreeGreen.png", Vector2{3620,2067}, PropType::TREE},    // 35

            Prop{"sprites/props/TreeGreen.png", Vector2{3700,2067}, PropType::TREE},    // 36
            Prop{"sprites/props/TreeGreen.png", Vector2{3800,2067}, PropType::TREE},    // 37
            Prop{"sprites/props/TreeGreen.png", Vector2{3900,2067}, PropType::TREE},    // 38
            Prop{"sprites/props/TreeGreen.png", Vector2{4000,2067}, PropType::TREE},    // 39

            // row22
            Prop{"sprites/props/TreeGreen.png", Vector2{0,2167}, PropType::TREE},       // 1 
            Prop{"sprites/props/TreeGreen.png", Vector2{100,2167}, PropType::TREE},     // 2 
            Prop{"sprites/props/TreeGreen.png", Vector2{200,2167}, PropType::TREE},     // 3 
            Prop{"sprites/props/TreeGreen.png", Vector2{300,2167}, PropType::TREE},     // 4 
            Prop{"sprites/props/TreeGreen.png", Vector2{400,2167}, PropType::TREE},     // 5 
            // Prop{"sprites/props/TreeGreen.png", Vector2{500,2167}, PropType::TREE},     // 6 
            Prop{"sprites/props/TreeGreen.png", Vector2{600,2167}, PropType::TREE},     // 7 
            Prop{"sprites/props/TreeGreen.png", Vector2{700,2167}, PropType::TREE},     // 8 
            Prop{"sprites/props/TreeGreen.png", Vector2{800,2167}, PropType::TREE},     // 9 
            Prop{"sprites/props/TreeGreen.png", Vector2{900,2167}, PropType::TREE},     // 10
            Prop{"sprites/props/TreeGreen.png", Vector2{1000,2167}, PropType::TREE},    // 11
            // Prop{"sprites/props/TreeGreen.png", Vector2{1100,2167}, PropType::TREE},    // 12
            // Prop{"sprites/props/TreeGreen.png", Vector2{1200,2167}, PropType::TREE},    // 13
            Prop{"sprites/props/TreeGreen.png", Vector2{1300,2167}, PropType::TREE},    // 14
            Prop{"sprites/props/TreeGreen.png", Vector2{1400,2167}, PropType::TREE},    // 15
            Prop{"sprites/props/TreeGreen.png", Vector2{1500,2167}, PropType::TREE},    // 16
            Prop{"sprites/props/TreeGreen.png", Vector2{1600,2167}, PropType::TREE},    // 17
            Prop{"sprites/props/TreeGreen.png", Vector2{1700,2167}, PropType::TREE},    // 18
            Prop{"sprites/props/TreeGreen.png", Vector2{1800,2167}, PropType::TREE},    // 19
            Prop{"sprites/props/TreeGreen.png", Vector2{1900,2167}, PropType::TREE},    // 20
            // Prop{"sprites/props/TreeGreen.png", Vector2{2000,2167}, PropType::TREE},    // 21
            // Prop{"sprites/props/TreeGreen.png", Vector2{2100,2167}, PropType::TREE},    // 22
            // Prop{"sprites/props/TreeGreen.png", Vector2{2200,2167}, PropType::TREE},    // 23
            // Prop{"sprites/props/TreeGreen.png", Vector2{2300,2167}, PropType::TREE},    // 24
            // Prop{"sprites/props/TreeGreen.png", Vector2{2400,2167}, PropType::TREE},    // 25
            // Prop{"sprites/props/TreeGreen.png", Vector2{2500,2167}, PropType::TREE},    // 26
            Prop{"sprites/props/TreeGreen.png", Vector2{2600,2167}, PropType::TREE},    // 27
            Prop{"sprites/props/TreeGreen.png", Vector2{2700,2167}, PropType::TREE},    // 28
            Prop{"sprites/props/TreeGreen.png", Vector2{2800,2167}, PropType::TREE},    // 29
            Prop{"sprites/props/TreeGreen.png", Vector2{2900,2167}, PropType::TREE},    // 30
            Prop{"sprites/props/TreeGreen.png", Vector2{3000,2167}, PropType::TREE},    // 31
            Prop{"sprites/props/TreeGreen.png", Vector2{3100,2167}, PropType::TREE},    // 32
            Prop{"sprites/props/TreeGreen.png", Vector2{3200,2167}, PropType::TREE},    // 33
            Prop{"sprites/props/TreeGreen.png", Vector2{3300,2167}, PropType::TREE},    // 34
            Prop{"sprites/props/TreeGreen.png", Vector2{3400,2167}, PropType::TREE},    // 35

            Prop{"sprites/props/TreeGreen.png", Vector2{3500,2167}, PropType::TREE},    // 35
            Prop{"sprites/props/TreeGreen.png", Vector2{3600,2167}, PropType::TREE},    // 35

            Prop{"sprites/props/TreeGreen.png", Vector2{3720,2167}, PropType::TREE},    // 36
            Prop{"sprites/props/TreeGreen.png", Vector2{3820,2167}, PropType::TREE},    // 37    
            Prop{"sprites/props/TreeGreen.png", Vector2{3920,2167}, PropType::TREE},    // 38

            // row21
            Prop{"sprites/props/TreeGreen.png", Vector2{20,2267}, PropType::TREE},      // 1
            Prop{"sprites/props/TreeGreen.png", Vector2{120,2267}, PropType::TREE},     // 2
            Prop{"sprites/props/TreeGreen.png", Vector2{220,2267}, PropType::TREE},     // 3   
            Prop{"sprites/props/TreeGreen.png", Vector2{320,2267}, PropType::TREE},     // 4
            Prop{"sprites/props/TreeGreen.png", Vector2{420,2267}, PropType::TREE},     // 5    
            Prop{"sprites/props/TreeGreen.png", Vector2{520,2267}, PropType::TREE},     // 6
            Prop{"sprites/props/TreeGreen.png", Vector2{620,2267}, PropType::TREE},     // 7
            Prop{"sprites/props/TreeGreen.png", Vector2{720,2267}, PropType::TREE},     // 8
            Prop{"sprites/props/TreeGreen.png", Vector2{820,2267}, PropType::TREE},     // 9
            Prop{"sprites/props/TreeGreen.png", Vector2{920,2267}, PropType::TREE},     // 10
            Prop{"sprites/props/TreeGreen.png", Vector2{1020,2267}, PropType::TREE},    // 11
            Prop{"sprites/props/TreeGreen.png", Vector2{1120,2267}, PropType::TREE},    // 12
            Prop{"sprites/props/TreeGreen.png", Vector2{1220,2267}, PropType::TREE},    // 13
            Prop{"sprites/props/TreeGreen.png", Vector2{1320,2267}, PropType::TREE},    // 14
            Prop{"sprites/props/TreeGreen.png", Vector2{1420,2267}, PropType::TREE},    // 15
            Prop{"sprites/props/TreeGreen.png", Vector2{1520,2267}, PropType::TREE},    // 16
            Prop{"sprites/props/TreeGreen.png", Vector2{1620,2267}, PropType::TREE},    // 17
            // Prop{"sprites/props/TreeGreen.png", Vector2{1720,2267}, PropType::TREE}, // 18
            // Prop{"sprites/props/TreeGreen.png", Vector2{1820,2267}, PropType::TREE}, // 19
            // Prop{"sprites/props/TreeGreen.png", Vector2{1920,2267}, PropType::TREE}, // 20  
            // Prop{"sprites/props/TreeGreen.png", Vector2{2020,2267}, PropType::TREE}, // 21
            // Prop{"sprites/props/TreeGreen.png", Vector2{2120,2267}, PropType::TREE}, // 22
            // Prop{"sprites/props/TreeGreen.png", Vector2{2220,2267}, PropType::TREE}, // 23
            // Prop{"sprites/props/TreeGreen.png", Vector2{2320,2267}, PropType::TREE}, // 24
            // Prop{"sprites/props/TreeGreen.png", Vector2{2420,2267}, PropType::TREE}, // 25
            // Prop{"sprites/props/TreeGreen.png", Vector2{2520,2267}, PropType::TREE}, // 26
            Prop{"sprites/props/TreeGreen.png", Vector2{2620,2267}, PropType::TREE},    // 27
            Prop{"sprites/props/TreeGreen.png", Vector2{2720,2267}, PropType::TREE},    // 28
            Prop{"sprites/props/TreeGreen.png", Vector2{2820,2267}, PropType::TREE},    // 29
            Prop{"sprites/props/TreeGreen.png", Vector2{2920,2267}, PropType::TREE},    // 30
            Prop{"sprites/props/TreeGreen.png", Vector2{3020,2267}, PropType::TREE},    // 31
            Prop{"sprites/props/TreeGreen.png", Vector2{3120,2267}, PropType::TREE},    // 32
            Prop{"sprites/props/TreeGreen.png", Vector2{3220,2267}, PropType::TREE},    // 33
            Prop{"sprites/props/TreeGreen.png", Vector2{3320,2267}, PropType::TREE},    // 34
            Prop{"sprites/props/TreeGreen.png", Vector2{3420,2267}, PropType::TREE},    // 35

            Prop{"sprites/props/TreeGreen.png", Vector2{3520,2267}, PropType::TREE},    // 35
            Prop{"sprites/props/TreeGreen.png", Vector2{3620,2267}, PropType::TREE},    // 35

            Prop{"sprites/props/TreeGreen.png", Vector2{3700,2267}, PropType::TREE},    // 36
            Prop{"sprites/props/TreeGreen.png", Vector2{3800,2267}, PropType::TREE},    // 37
            Prop{"sprites/props/TreeGreen.png", Vector2{3900,2267}, PropType::TREE},    // 38
            Prop{"sprites/props/TreeGreen.png", Vector2{4000,2267}, PropType::TREE},    // 39

            // row22
            Prop{"sprites/props/TreeGreen.png", Vector2{50,2367}, PropType::TREE},      // 1 
            // Prop{"sprites/props/TreeGreen.png", Vector2{100,2367}, PropType::TREE},  // 2 
            // Prop{"sprites/props/TreeGreen.png", Vector2{200,2367}, PropType::TREE},  // 3 
            // Prop{"sprites/props/TreeGreen.png", Vector2{300,2367}, PropType::TREE},  // 4 
            // Prop{"sprites/props/TreeGreen.png", Vector2{400,2367}, PropType::TREE},  // 5 
            Prop{"sprites/props/TreeClear.png", Vector2{550,2303}, PropType::TREE},     // 6 Invisible Wall
            Prop{"sprites/props/TreeGreen.png", Vector2{550,2367}, PropType::TREE},     // 6 
            // Prop{"sprites/props/TreeGreen.png", Vector2{600,2367}, PropType::TREE},  // 7     
            Prop{"sprites/props/TreeGreen.png", Vector2{700,2367}, PropType::TREE},  // 8 
            Prop{"sprites/props/TreeGreen.png", Vector2{800,2367}, PropType::TREE},  // 9 
            Prop{"sprites/props/TreeGreen.png", Vector2{900,2367}, PropType::TREE},  // 10
            Prop{"sprites/props/TreeGreen.png", Vector2{1000,2367}, PropType::TREE}, // 11
            Prop{"sprites/props/TreeGreen.png", Vector2{1100,2367}, PropType::TREE}, // 12
            Prop{"sprites/props/TreeGreen.png", Vector2{1200,2367}, PropType::TREE}, // 13
            Prop{"sprites/props/TreeGreen.png", Vector2{1300,2367}, PropType::TREE}, // 14
            Prop{"sprites/props/TreeGreen.png", Vector2{1400,2367}, PropType::TREE}, // 15
            Prop{"sprites/props/TreeGreen.png", Vector2{1500,2367}, PropType::TREE}, // 16
            Prop{"sprites/props/TreeGreen.png", Vector2{1600,2367}, PropType::TREE}, // 17
            Prop{"sprites/props/TreeGreen.png", Vector2{1700,2367}, PropType::TREE}, // 18
            Prop{"sprites/props/TreeGreen.png", Vector2{1800,2367}, PropType::TREE}, // 19
            // Prop{"sprites/props/TreeGreen.png", Vector2{1900,2367}, PropType::TREE}, // 20
            // Prop{"sprites/props/TreeGreen.png", Vector2{2000,2367}, PropType::TREE}, // 21
            // Prop{"sprites/props/TreeGreen.png", Vector2{2100,2367}, PropType::TREE}, // 22
            // Prop{"sprites/props/TreeGreen.png", Vector2{2200,2367}, PropType::TREE}, // 23
            // Prop{"sprites/props/TreeGreen.png", Vector2{2300,2367}, PropType::TREE}, // 24
            // Prop{"sprites/props/TreeGreen.png", Vector2{2400,2367}, PropType::TREE}, // 25
            // Prop{"sprites/props/TreeGreen.png", Vector2{2500,2367}, PropType::TREE}, // 26
            Prop{"sprites/props/TreeGreen.png", Vector2{2600,2340}, PropType::TREE}, // 27
            Prop{"sprites/props/TreeGreen.png", Vector2{2700,2340}, PropType::TREE}, // 28
            Prop{"sprites/props/TreeGreen.png", Vector2{2800,2340}, PropType::TREE}, // 29
            Prop{"sprites/props/TreeGreen.png", Vector2{2900,2340}, PropType::TREE}, // 30
            Prop{"sprites/props/TreeGreen.png", Vector2{3000,2340}, PropType::TREE}, // 31
            Prop{"sprites/props/TreeGreen.png", Vector2{3100,2340}, PropType::TREE}, // 32
            Prop{"sprites/props/TreeGreen.png", Vector2{3200,2340}, PropType::TREE}, // 33
            Prop{"sprites/props/TreeGreen.png", Vector2{3300,2340}, PropType::TREE}, // 34
            Prop{"sprites/props/TreeGreen.png", Vector2{3400,2340}, PropType::TREE}, // 35

            Prop{"sprites/props/TreeGreen.png", Vector2{3500,2340}, PropType::TREE}, // 36
            Prop{"sprites/props/TreeGreen.png", Vector2{3600,2340}, PropType::TREE}, // 37

            Prop{"sprites/props/TreeGreen.png", Vector2{3720,2340}, PropType::TREE}, // 38
            Prop{"sprites/props/TreeGreen.png", Vector2{3820,2340}, PropType::TREE}, // 39
            Prop{"sprites/props/TreeGreen.png", Vector2{3920,2340}, PropType::TREE}, // 40

            // row23
            Prop{"sprites/props/TreeGreen.png", Vector2{20,2467}, PropType::TREE},      // 1 
            // Prop{"sprites/props/TreeGreen.png", Vector2{120,2467}, PropType::TREE},  // 2 
            // Prop{"sprites/props/TreeGreen.png", Vector2{220,2467}, PropType::TREE},  // 3 
            // Prop{"sprites/props/TreeGreen.png", Vector2{320,2467}, PropType::TREE},  // 4 
            // Prop{"sprites/props/TreeGreen.png", Vector2{420,2467}, PropType::TREE},  // 5 
            // Prop{"sprites/props/TreeGreen.png", Vector2{520,2467}, PropType::TREE},  // 6 
            Prop{"sprites/props/TreeClear.png", Vector2{620,2403}, PropType::TREE},     // 7 Invisible Wall
            Prop{"sprites/props/TreeGreen.png", Vector2{620,2467}, PropType::TREE},     // 7 
            Prop{"sprites/props/TreeGreen.png", Vector2{720,2467}, PropType::TREE},     // 8 
            Prop{"sprites/props/TreeGreen.png", Vector2{820,2467}, PropType::TREE},     // 9 
            Prop{"sprites/props/TreeGreen.png", Vector2{920,2467}, PropType::TREE},     // 10
            Prop{"sprites/props/TreeGreen.png", Vector2{1020,2467}, PropType::TREE},    // 11    
            Prop{"sprites/props/TreeGreen.png", Vector2{1120,2467}, PropType::TREE},    // 12    
            Prop{"sprites/props/TreeGreen.png", Vector2{1220,2467}, PropType::TREE},    // 13
            Prop{"sprites/props/TreeGreen.png", Vector2{1320,2467}, PropType::TREE},    // 14
            Prop{"sprites/props/TreeGreen.png", Vector2{1420,2467}, PropType::TREE},    // 15
            Prop{"sprites/props/TreeGreen.png", Vector2{1520,2467}, PropType::TREE},    // 16
            Prop{"sprites/props/TreeGreen.png", Vector2{1620,2467}, PropType::TREE},    // 17
            Prop{"sprites/props/TreeGreen.png", Vector2{1720,2467}, PropType::TREE},    // 18
            Prop{"sprites/props/TreeGreen.png", Vector2{1820,2467}, PropType::TREE},    // 19
            Prop{"sprites/props/TreeGreen.png", Vector2{1920,2467}, PropType::TREE},    // 20
            // Prop{"sprites/props/TreeGreen.png", Vector2{2020,2467}, PropType::TREE}, // 21
            // Prop{"sprites/props/TreeGreen.png", Vector2{2120,2467}, PropType::TREE}, // 22
            // Prop{"sprites/props/TreeGreen.png", Vector2{2220,2467}, PropType::TREE}, // 23
            // Prop{"sprites/props/TreeGreen.png", Vector2{2320,2467}, PropType::TREE}, // 24
            // Prop{"sprites/props/TreeGreen.png", Vector2{2420,2467}, PropType::TREE}, // 25
            // Prop{"sprites/props/TreeGreen.png", Vector2{2520,2467}, PropType::TREE}, // 26
            // Prop{"sprites/props/TreeGreen.png", Vector2{2620,2467}, PropType::TREE}, // 27
            // Prop{"sprites/props/TreeGreen.png", Vector2{2720,2467}, PropType::TREE}, // 28
            // Prop{"sprites/props/TreeGreen.png", Vector2{2820,2467}, PropType::TREE}, // 29
            // Prop{"sprites/props/TreeGreen.png", Vector2{2920,2467}, PropType::TREE}, // 30
            // Prop{"sprites/props/TreeGreen.png", Vector2{3020,2467}, PropType::TREE}, // 31
            // Prop{"sprites/props/TreeGreen.png", Vector2{3120,2467}, PropType::TREE}, // 32
            // Prop{"sprites/props/TreeGreen.png", Vector2{3220,2467}, PropType::TREE}, // 33
            // Prop{"sprites/props/TreeGreen.png", Vector2{3320,2467}, PropType::TREE}, // 34
            // Prop{"sprites/props/TreeGreen.png", Vector2{3420,2467}, PropType::TREE}, // 35

            // Prop{"sprites/props/TreeGreen.png", Vector2{3700,2467}, PropType::TREE}, // 36
            // Prop{"sprites/props/TreeGreen.png", Vector2{3800,2467}, PropType::TREE}, // 37
            // Prop{"sprites/props/TreeGreen.png", Vector2{3900,2467}, PropType::TREE}, // 38
            // Prop{"sprites/props/TreeGreen.png", Vector2{4000,2467}, PropType::TREE}, // 39

            // row24
            Prop{"sprites/props/TreeGreen.png", Vector2{50,2567}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{150,2567}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{250,2567}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{350,2567}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{450,2567}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{550,2567}, PropType::TREE},
            Prop{"sprites/props/TreeGreen.png", Vector2{650,2567}, PropType::TREE},

            Prop{"sprites/props/TreeGreen.png", Vector2{1980,2567}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{2100,2567}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{2200,2567}, PropType::TREE},
            Prop{"sprites/props/TreeGreen.png", Vector2{2490,2567}, PropType::TREE},
            Prop{"sprites/props/TreeGreen.png", Vector2{2590,2567}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{2700,2567}, PropType::TREE},
            Prop{"sprites/props/TreeGreen.png", Vector2{2800,2567}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{2900,2567}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{3000,2567}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{3100,2567}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{3200,2567}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{3300,2567}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{3400,2567}, PropType::TREE},

            // Prop{"sprites/props/TreeGreen.png", Vector2{3720,2567}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{3820,2567}, PropType::TREE},
            Prop{"sprites/props/TreeGreen.png", Vector2{3920,2567}, PropType::TREE},

            // row25
            Prop{"sprites/props/TreeGreen.png", Vector2{20,2667}, PropType::TREE}, 
            // Prop{"sprites/props/TreeGreen.png", Vector2{120,2667}, PropType::TREE}, 
            // Prop{"sprites/props/TreeGreen.png", Vector2{220,2667}, PropType::TREE}, 
            // Prop{"sprites/props/TreeGreen.png", Vector2{320,2667}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{420,2667}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{520,2667}, PropType::TREE},
            Prop{"sprites/props/TreeGreen.png", Vector2{620,2667}, PropType::TREE},

            // Prop{"sprites/props/TreeGreen.png", Vector2{1920,2667}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{2020,2667}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{2120,2667}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{2220,2667}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{2320,2667}, PropType::TREE},
            Prop{"sprites/props/TreeGreen.png", Vector2{2420,2667}, PropType::TREE},
            Prop{"sprites/props/TreeGreen.png", Vector2{2520,2667}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{2620,2667}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{2720,2667}, PropType::TREE},
            Prop{"sprites/props/TreeGreen.png", Vector2{2820,2667}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{2920,2667}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{3020,2667}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{3120,2667}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{3220,2667}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{3320,2667}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{3420,2667}, PropType::TREE},

            // Prop{"sprites/props/TreeGreen.png", Vector2{3700,2667}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{3800,2667}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{3900,2667}, PropType::TREE},
            Prop{"sprites/props/TreeGreen.png", Vector2{4000,2667}, PropType::TREE},

            // row26
            Prop{"sprites/props/TreeGreen.png", Vector2{50,2767}, PropType::TREE},
            Prop{"sprites/props/TreeGreen.png", Vector2{100,2767}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{250,2767}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{350,2767}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{450,2767}, PropType::TREE},
            Prop{"sprites/props/TreeGreen.png", Vector2{550,2767}, PropType::TREE},
            Prop{"sprites/props/TreeGreen.png", Vector2{650,2767}, PropType::TREE},

            Prop{"sprites/props/TreeGreen.png", Vector2{1980,2767}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{2100,2767}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{2200,2767}, PropType::TREE},
            Prop{"sprites/props/TreeGreen.png", Vector2{2490,2767}, PropType::TREE},
            Prop{"sprites/props/TreeGreen.png", Vector2{2590,2767}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{2700,2767}, PropType::TREE},
            Prop{"sprites/props/TreeGreen.png", Vector2{2800,2767}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{2900,2767}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{3000,2767}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{3100,2767}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{3200,2767}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{3300,2767}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{3400,2767}, PropType::TREE},

            // Prop{"sprites/props/TreeGreen.png", Vector2{3720,2767}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{3820,2767}, PropType::TREE},
            Prop{"sprites/props/TreeGreen.png", Vector2{3920,2767}, PropType::TREE},

            // row27
            Prop{"sprites/props/TreeGreen.png", Vector2{20,2867}, PropType::TREE}, 
            Prop{"sprites/props/TreeGreen.png", Vector2{120,2867}, PropType::TREE}, 
            // Prop{"sprites/props/TreeGreen.png", Vector2{220,2867}, PropType::TREE}, 
            // Prop{"sprites/props/TreeGreen.png", Vector2{320,2867}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{420,2867}, PropType::TREE},
            Prop{"sprites/props/TreeGreen.png", Vector2{520,2867}, PropType::TREE},
            Prop{"sprites/props/TreeGreen.png", Vector2{620,2867}, PropType::TREE},

            // Prop{"sprites/props/TreeGreen.png", Vector2{1920,2867}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{2020,2867}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{2120,2867}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{2220,2867}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{2320,2867}, PropType::TREE},
            Prop{"sprites/props/TreeGreen.png", Vector2{2420,2867}, PropType::TREE},
            Prop{"sprites/props/TreeGreen.png", Vector2{2520,2867}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{2620,2867}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{2720,2867}, PropType::TREE},
            Prop{"sprites/props/TreeGreen.png", Vector2{2820,2867}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{2920,2867}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{3020,2867}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{3120,2867}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{3220,2867}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{3320,2867}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{3420,2867}, PropType::TREE},

            // Prop{"sprites/props/TreeGreen.png", Vector2{3700,2867}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{3800,2867}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{3900,2867}, PropType::TREE},
            Prop{"sprites/props/TreeGreen.png", Vector2{4000,2867}, PropType::TREE},

            // row28
            Prop{"sprites/props/TreeGreen.png", Vector2{50,2967}, PropType::TREE},
            Prop{"sprites/props/TreeGreen.png", Vector2{150,2967}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{250,2967}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{350,2967}, PropType::TREE},
            Prop{"sprites/props/TreeGreen.png", Vector2{450,2967}, PropType::TREE},
            Prop{"sprites/props/TreeGreen.png", Vector2{550,2967}, PropType::TREE},
            Prop{"sprites/props/TreeGreen.png", Vector2{650,2967}, PropType::TREE},

            Prop{"sprites/props/TreeGreen.png", Vector2{1980,2967}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{2100,2967}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{2200,2967}, PropType::TREE},
            Prop{"sprites/props/TreeGreen.png", Vector2{2490,2967}, PropType::TREE},
            Prop{"sprites/props/TreeGreen.png", Vector2{2590,2967}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{2700,2967}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{2800,2967}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{2900,2967}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{3000,2967}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{3100,2967}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{3200,2967}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{3300,2967}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{3400,2967}, PropType::TREE},

            // Prop{"sprites/props/TreeGreen.png", Vector2{3720,2967}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{3820,2967}, PropType::TREE},
            Prop{"sprites/props/TreeGreen.png", Vector2{3920,2967}, PropType::TREE},

            // row29
            Prop{"sprites/props/TreeGreen.png", Vector2{20,3067}, PropType::TREE}, 
            Prop{"sprites/props/TreeGreen.png", Vector2{120,3067}, PropType::TREE}, 
            Prop{"sprites/props/TreeGreen.png", Vector2{220,3067}, PropType::TREE}, 
            // Prop{"sprites/props/TreeGreen.png", Vector2{320,3067}, PropType::TREE},
            Prop{"sprites/props/TreeGreen.png", Vector2{420,3067}, PropType::TREE},
            Prop{"sprites/props/TreeGreen.png", Vector2{520,3067}, PropType::TREE},
            Prop{"sprites/props/TreeGreen.png", Vector2{620,3067}, PropType::TREE},

            // Prop{"sprites/props/TreeGreen.png", Vector2{1920,3067}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{2020,3067}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{2120,3067}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{2220,3067}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{2320,3067}, PropType::TREE},
            Prop{"sprites/props/TreeGreen.png", Vector2{2420,3067}, PropType::TREE},
            Prop{"sprites/props/TreeGreen.png", Vector2{2520,3067}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{2620,3067}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{2720,3067}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{2820,3067}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{2920,3067}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{3020,3067}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{3120,3067}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{3220,3067}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{3320,3067}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{3420,3067}, PropType::TREE},

            // Prop{"sprites/props/TreeGreen.png", Vector2{3700,3067}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{3800,3067}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{3900,3067}, PropType::TREE},
            Prop{"sprites/props/TreeGreen.png", Vector2{4000,3067}, PropType::TREE},

            // row30
            Prop{"sprites/props/TreeGreen.png", Vector2{50,3167}, PropType::TREE},
            Prop{"sprites/props/TreeGreen.png", Vector2{150,3167}, PropType::TREE},
            Prop{"sprites/props/TreeGreen.png", Vector2{250,3167}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{350,3167}, PropType::TREE},
            Prop{"sprites/props/TreeGreen.png", Vector2{450,3167}, PropType::TREE},
            Prop{"sprites/props/TreeGreen.png", Vector2{550,3167}, PropType::TREE},
            Prop{"sprites/props/TreeGreen.png", Vector2{650,3167}, PropType::TREE},

            Prop{"sprites/props/TreeGreen.png", Vector2{1980,3167}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{2100,3167}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{2200,3167}, PropType::TREE},
            Prop{"sprites/props/TreeGreen.png", Vector2{2490,3167}, PropType::TREE},
            Prop{"sprites/props/TreeGreen.png", Vector2{2590,3167}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{2700,3167}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{2800,3167}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{2900,3167}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{3000,3167}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{3100,3167}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{3200,3167}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{3300,3167}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{3400,3167}, PropType::TREE},

            // Prop{"sprites/props/TreeGreen.png", Vector2{3720,3167}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{3820,3167}, PropType::TREE},
            Prop{"sprites/props/TreeGreen.png", Vector2{3920,3167}, PropType::TREE},

            // row31
            Prop{"sprites/props/TreeGreen.png", Vector2{20,3267}, PropType::TREE}, 
            Prop{"sprites/props/TreeGreen.png", Vector2{120,3267}, PropType::TREE}, 
            Prop{"sprites/props/TreeGreen.png", Vector2{220,3267}, PropType::TREE}, 
            // Prop{"sprites/props/TreeGreen.png", Vector2{320,3267}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{420,3267}, PropType::TREE},
            Prop{"sprites/props/TreeGreen.png", Vector2{530,3267}, PropType::TREE},
            Prop{"sprites/props/TreeGreen.png", Vector2{620,3267}, PropType::TREE},

            // Prop{"sprites/props/TreeGreen.png", Vector2{1920,3267}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{2020,3267}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{2120,3267}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{2220,3267}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{2320,3267}, PropType::TREE},
            Prop{"sprites/props/TreeGreen.png", Vector2{2420,3267}, PropType::TREE},
            Prop{"sprites/props/TreeGreen.png", Vector2{2520,3267}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{2620,3267}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{2720,3267}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{2820,3267}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{2920,3267}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{3020,3267}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{3120,3267}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{3220,3267}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{3320,3267}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{3420,3267}, PropType::TREE},

            // Prop{"sprites/props/TreeGreen.png", Vector2{3700,3267}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{3800,3267}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{3900,3267}, PropType::TREE},
            Prop{"sprites/props/TreeGreen.png", Vector2{4000,3267}, PropType::TREE},

            // row32
            Prop{"sprites/props/TreeGreen.png", Vector2{50,3367}, PropType::TREE},
            Prop{"sprites/props/TreeGreen.png", Vector2{150,3367}, PropType::TREE},
            Prop{"sprites/props/TreeGreen.png", Vector2{250,3367}, PropType::TREE},
            Prop{"sprites/props/TreeGreen.png", Vector2{350,3367}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{450,3367}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{550,3367}, PropType::TREE},
            Prop{"sprites/props/TreeGreen.png", Vector2{650,3367}, PropType::TREE},

            // Prop{"sprites/props/TreeGreen.png", Vector2{1980,3367}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{2100,3367}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{2200,3367}, PropType::TREE},
            Prop{"sprites/props/TreeGreen.png", Vector2{2490,3367}, PropType::TREE},
            Prop{"sprites/props/TreeGreen.png", Vector2{2590,3367}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{2700,3367}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{2800,3367}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{2900,3367}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{3000,3367}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{3100,3367}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{3200,3367}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{3300,3367}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{3400,3367}, PropType::TREE},

            // Prop{"sprites/props/TreeGreen.png", Vector2{3720,3367}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{3820,3367}, PropType::TREE},
            Prop{"sprites/props/TreeGreen.png", Vector2{3920,3367}, PropType::TREE},

            // row33
            Prop{"sprites/props/TreeGreen.png", Vector2{20,3467}, PropType::TREE},      
            Prop{"sprites/props/TreeGreen.png", Vector2{120,3467}, PropType::TREE},         
            Prop{"sprites/props/TreeGreen.png", Vector2{220,3467}, PropType::TREE},         
            Prop{"sprites/props/TreeGreen.png", Vector2{320,3467}, PropType::TREE},     
            Prop{"sprites/props/TreeGreen.png", Vector2{420,3467}, PropType::TREE},     
            // Prop{"sprites/props/TreeGreen.png", Vector2{520,3467}, PropType::TREE},      
            Prop{"sprites/props/TreeGreen.png", Vector2{620,3467}, PropType::TREE},     

            // Prop{"sprites/props/TreeGreen.png", Vector2{2020,3467}, PropType::TREE},     
            // Prop{"sprites/props/TreeGreen.png", Vector2{2120,3467}, PropType::TREE},     
            // Prop{"sprites/props/TreeGreen.png", Vector2{2220,3467}, PropType::TREE},     
            // Prop{"sprites/props/TreeGreen.png", Vector2{2320,3467}, PropType::TREE},     
            // Prop{"sprites/props/TreeGreen.png", Vector2{2420,3467}, PropType::TREE},     
            // Prop{"sprites/props/TreeGreen.png", Vector2{2520,3467}, PropType::TREE},     
            // Prop{"sprites/props/TreeGreen.png", Vector2{2620,3467}, PropType::TREE},     
            // Prop{"sprites/props/TreeGreen.png", Vector2{2720,3467}, PropType::TREE},     
            // Prop{"sprites/props/TreeGreen.png", Vector2{2820,3467}, PropType::TREE},     
            // Prop{"sprites/props/TreeGreen.png", Vector2{2920,3467}, PropType::TREE},     
            // Prop{"sprites/props/TreeGreen.png", Vector2{3020,3467}, PropType::TREE},     
            // Prop{"sprites/props/TreeGreen.png", Vector2{3120,3467}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{3220,3467}, PropType::TREE},     
            // Prop{"sprites/props/TreeGreen.png", Vector2{3320,3467}, PropType::TREE},     
            // Prop{"sprites/props/TreeGreen.png", Vector2{3420,3467}, PropType::TREE},     

            // Prop{"sprites/props/TreeGreen.png", Vector2{3700,3467}, PropType::TREE}, 
            // Prop{"sprites/props/TreeGreen.png", Vector2{3800,3467}, PropType::TREE}, 
            // Prop{"sprites/props/TreeGreen.png", Vector2{3900,3467}, PropType::TREE}, 
            Prop{"sprites/props/TreeGreen.png", Vector2{4000,3467}, PropType::TREE},    

            // row34
            Prop{"sprites/props/TreeGreen.png", Vector2{0,3567}, PropType::TREE},       // 1 
            Prop{"sprites/props/TreeGreen.png", Vector2{150,3567}, PropType::TREE},     // 2 
            Prop{"sprites/props/TreeGreen.png", Vector2{250,3567}, PropType::TREE},     // 3 
            Prop{"sprites/props/TreeGreen.png", Vector2{350,3567}, PropType::TREE},     // 4 
            // Prop{"sprites/props/TreeGreen.png", Vector2{450,3567}, PropType::TREE},  // 5 
            Prop{"sprites/props/TreeGreen.png", Vector2{665,3530}, PropType::TREE},     // 6 

            // Prop{"sprites/props/TreeGreen.png", Vector2{2100,3567}, PropType::TREE}, // 7 
            // Prop{"sprites/props/TreeGreen.png", Vector2{2200,3567}, PropType::TREE}, // 8 
            // Prop{"sprites/props/TreeGreen.png", Vector2{2490,3567}, PropType::TREE}, // 9 
            // Prop{"sprites/props/TreeGreen.png", Vector2{2590,3567}, PropType::TREE}, // 10
            // Prop{"sprites/props/TreeGreen.png", Vector2{2700,3567}, PropType::TREE}, // 11
            // Prop{"sprites/props/TreeGreen.png", Vector2{2800,3567}, PropType::TREE}, // 12
            // Prop{"sprites/props/TreeGreen.png", Vector2{2900,3567}, PropType::TREE}, // 13
            // Prop{"sprites/props/TreeGreen.png", Vector2{3000,3567}, PropType::TREE}, // 14
            // Prop{"sprites/props/TreeGreen.png", Vector2{3100,3567}, PropType::TREE}, // 15 
            // Prop{"sprites/props/TreeGreen.png", Vector2{3200,3567}, PropType::TREE}, // 16
            // Prop{"sprites/props/TreeGreen.png", Vector2{3300,3567}, PropType::TREE}, // 17
            // Prop{"sprites/props/TreeGreen.png", Vector2{3400,3567}, PropType::TREE}, // 18

            // Prop{"sprites/props/TreeGreen.png", Vector2{3720,3567}, PropType::TREE}, // 19 
            // Prop{"sprites/props/TreeGreen.png", Vector2{3820,3567}, PropType::TREE}, // 20
            Prop{"sprites/props/TreeGreen.png", Vector2{3920,3567}, PropType::TREE},    // 21

            // row35
            Prop{"sprites/props/TreeGreen.png", Vector2{100,3740}, PropType::TREE}, 
            Prop{"sprites/props/TreeGreen.png", Vector2{200,3740}, PropType::TREE}, 
            Prop{"sprites/props/TreeGreen.png", Vector2{300,3740}, PropType::TREE},

            Prop{"sprites/props/TreeGreen.png", Vector2{500,3740}, PropType::TREE},

            Prop{"sprites/props/TreeGreen.png", Vector2{800,3740}, PropType::TREE},
            Prop{"sprites/props/TreeGreen.png", Vector2{900,3740}, PropType::TREE},

            Prop{"sprites/props/TreeGreen.png", Vector2{1600,3740}, PropType::TREE},
            Prop{"sprites/props/TreeGreen.png", Vector2{1700,3740}, PropType::TREE},
            Prop{"sprites/props/TreeGreen.png", Vector2{1800,3740}, PropType::TREE},
            Prop{"sprites/props/TreeGreen.png", Vector2{1900,3740}, PropType::TREE},
            Prop{"sprites/props/TreeGreen.png", Vector2{2000,3740}, PropType::TREE},
    
            Prop{"sprites/props/TreeGreen.png", Vector2{2200,3740}, PropType::TREE},
            Prop{"sprites/props/TreeGreen.png", Vector2{2300,3740}, PropType::TREE},
            Prop{"sprites/props/TreeGreen.png", Vector2{2400,3740}, PropType::TREE},
    
            Prop{"sprites/props/TreeGreen.png", Vector2{2600,3740}, PropType::TREE},
            Prop{"sprites/props/TreeGreen.png", Vector2{2700,3740}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{2800,3740}, PropType::TREE},
            Prop{"sprites/props/TreeGreen.png", Vector2{2900,3740}, PropType::TREE},
            Prop{"sprites/props/TreeGreen.png", Vector2{3000,3740}, PropType::TREE},
            Prop{"sprites/props/TreeGreen.png", Vector2{3100,3740}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{3200,3740}, PropType::TREE},
            Prop{"sprites/props/TreeGreen.png", Vector2{3300,3740}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{3400,3740}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{3500,3740}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{3600,3740}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{3700,3740}, PropType::TREE},
            Prop{"sprites/props/TreeGreen.png", Vector2{3800,3740}, PropType::TREE},
            Prop{"sprites/props/TreeGreen.png", Vector2{3900,3740}, PropType::TREE},
            // Prop{"sprites/props/TreeGreen.png", Vector2{4000,3740}, PropType::TREE},

            // row36
            Prop{"sprites/props/TreeGreen.png", Vector2{20,3840}, PropType::TREE},      // 1 
            Prop{"sprites/props/TreeGreen.png", Vector2{120,3840}, PropType::TREE},     // 2 
            Prop{"sprites/props/TreeGreen.png", Vector2{220,3840}, PropType::TREE},     // 3 
            // Prop{"sprites/props/TreeGreen.png", Vector2{320,3840}, PropType::TREE},  // 4 
            Prop{"sprites/props/TreeGreen.png", Vector2{420,3840}, PropType::TREE},     // 5 
            Prop{"sprites/props/TreeGreen.png", Vector2{520,3840}, PropType::TREE},     // 6 
            // Prop{"sprites/props/TreeGreen.png", Vector2{620,3840}, PropType::TREE},  // 7 
            Prop{"sprites/props/TreeGreen.png", Vector2{720,3840}, PropType::TREE},     // 8 
            Prop{"sprites/props/TreeGreen.png", Vector2{820,3840}, PropType::TREE},     // 9 
            // Prop{"sprites/props/TreeGreen.png", Vector2{920,3840}, PropType::TREE},  // 10
            Prop{"sprites/props/TreeGreen.png", Vector2{1020,3840}, PropType::TREE},    // 11
            Prop{"sprites/props/TreeGreen.png", Vector2{1120,3840}, PropType::TREE},    // 12
            Prop{"sprites/props/TreeGreen.png", Vector2{1220,3840}, PropType::TREE},    // 13
            // Prop{"sprites/props/TreeGreen.png", Vector2{1320,3840}, PropType::TREE}, // 14    
            Prop{"sprites/props/TreeGreen.png", Vector2{1420,3840}, PropType::TREE},    // 15
            Prop{"sprites/props/TreeGreen.png", Vector2{1520,3840}, PropType::TREE},    // 16
            Prop{"sprites/props/TreeGreen.png", Vector2{1620,3840}, PropType::TREE},    // 17
            Prop{"sprites/props/TreeGreen.png", Vector2{1720,3840}, PropType::TREE},    // 18
            // Prop{"sprites/props/TreeGreen.png", Vector2{1820,3840}, PropType::TREE}, // 19    
            Prop{"sprites/props/TreeGreen.png", Vector2{1920,3840}, PropType::TREE},    // 20
            Prop{"sprites/props/TreeGreen.png", Vector2{2020,3840}, PropType::TREE},    // 21
            Prop{"sprites/props/TreeGreen.png", Vector2{2120,3840}, PropType::TREE},    // 22
            // Prop{"sprites/props/TreeGreen.png", Vector2{2220,3840}, PropType::TREE}, // 23    
            Prop{"sprites/props/TreeGreen.png", Vector2{2320,3840}, PropType::TREE},    // 24
            Prop{"sprites/props/TreeGreen.png", Vector2{2420,3840}, PropType::TREE},    // 25
            Prop{"sprites/props/TreeGreen.png", Vector2{2520,3840}, PropType::TREE},    // 26
            // Prop{"sprites/props/TreeGreen.png", Vector2{2620,3840}, PropType::TREE}, // 27    
            Prop{"sprites/props/TreeGreen.png", Vector2{2720,3840}, PropType::TREE},    // 28
            Prop{"sprites/props/TreeGreen.png", Vector2{2820,3840}, PropType::TREE},    // 29
            Prop{"sprites/props/TreeGreen.png", Vector2{2920,3840}, PropType::TREE},    // 30
            Prop{"sprites/props/TreeGreen.png", Vector2{3020,3840}, PropType::TREE},    // 31
            Prop{"sprites/props/TreeGreen.png", Vector2{3120,3840}, PropType::TREE},    // 32
            // Prop{"sprites/props/TreeGreen.png", Vector2{3220,3840}, PropType::TREE}, // 33    
            // Prop{"sprites/props/TreeGreen.png", Vector2{3320,3840}, PropType::TREE}, // 34    
            Prop{"sprites/props/TreeGreen.png", Vector2{3420,3840}, PropType::TREE},    // 35
            Prop{"sprites/props/TreeGreen.png", Vector2{3520,3840}, PropType::TREE},    // 36
            // Prop{"sprites/props/TreeGreen.png", Vector2{3620,3840}, PropType::TREE}, // 37    
            Prop{"sprites/props/TreeGreen.png", Vector2{3720,3840}, PropType::TREE},    // 38
            Prop{"sprites/props/TreeGreen.png", Vector2{3820,3840}, PropType::TREE},    // 39
            Prop{"sprites/props/TreeGreen.png", Vector2{3920,3840}, PropType::TREE},    // 40

            // row37
            Prop{"sprites/props/TreeGreen.png", Vector2{0,3940}, PropType::TREE},       // 1 
            Prop{"sprites/props/TreeGreen.png", Vector2{100,3940}, PropType::TREE},     // 2 
            Prop{"sprites/props/TreeGreen.png", Vector2{200,3940}, PropType::TREE},     // 3 
            Prop{"sprites/props/TreeGreen.png", Vector2{300,3940}, PropType::TREE},     // 4  
            Prop{"sprites/props/TreeGreen.png", Vector2{400,3940}, PropType::TREE},     // 5 
            Prop{"sprites/props/TreeGreen.png", Vector2{500,3940}, PropType::TREE},     // 6 
            Prop{"sprites/props/TreeGreen.png", Vector2{600,3940}, PropType::TREE},     // 7 
            Prop{"sprites/props/TreeGreen.png", Vector2{700,3940}, PropType::TREE},     // 8 
            Prop{"sprites/props/TreeGreen.png", Vector2{800,3940}, PropType::TREE},     // 9 
            Prop{"sprites/props/TreeGreen.png", Vector2{900,3940}, PropType::TREE},     // 10
            Prop{"sprites/props/TreeGreen.png", Vector2{1000,3940}, PropType::TREE},    // 11  
            Prop{"sprites/props/TreeGreen.png", Vector2{1100,3940}, PropType::TREE},    // 12
            Prop{"sprites/props/TreeGreen.png", Vector2{1200,3940}, PropType::TREE},    // 13
            Prop{"sprites/props/TreeGreen.png", Vector2{1300,3940}, PropType::TREE},    // 14
            Prop{"sprites/props/TreeGreen.png", Vector2{1400,3940}, PropType::TREE},    // 15
            Prop{"sprites/props/TreeGreen.png", Vector2{1500,3940}, PropType::TREE},    // 16
            Prop{"sprites/props/TreeGreen.png", Vector2{1600,3940}, PropType::TREE},    // 17
            Prop{"sprites/props/TreeGreen.png", Vector2{1700,3940}, PropType::TREE},    // 18
            Prop{"sprites/props/TreeGreen.png", Vector2{1800,3940}, PropType::TREE},    // 19
            Prop{"sprites/props/TreeGreen.png", Vector2{1900,3940}, PropType::TREE},    // 20
            Prop{"sprites/props/TreeGreen.png", Vector2{2000,3940}, PropType::TREE},    // 21
            Prop{"sprites/props/TreeGreen.png", Vector2{2100,3940}, PropType::TREE},    // 22
            Prop{"sprites/props/TreeGreen.png", Vector2{2200,3940}, PropType::TREE},    // 23
            Prop{"sprites/props/TreeGreen.png", Vector2{2300,3940}, PropType::TREE},    // 24
            Prop{"sprites/props/TreeGreen.png", Vector2{2400,3940}, PropType::TREE},    // 25
            Prop{"sprites/props/TreeGreen.png", Vector2{2500,3940}, PropType::TREE},    // 26
            Prop{"sprites/props/TreeGreen.png", Vector2{2600,3940}, PropType::TREE},    // 27 
            Prop{"sprites/props/TreeGreen.png", Vector2{2700,3940}, PropType::TREE},    // 28
            Prop{"sprites/props/TreeGreen.png", Vector2{2800,3940}, PropType::TREE},    // 29
            Prop{"sprites/props/TreeGreen.png", Vector2{2900,3940}, PropType::TREE},    // 30
            Prop{"sprites/props/TreeGreen.png", Vector2{3000,3940}, PropType::TREE},    // 31
            Prop{"sprites/props/TreeGreen.png", Vector2{3100,3940}, PropType::TREE},    // 32
            Prop{"sprites/props/TreeGreen.png", Vector2{3200,3940}, PropType::TREE},    // 33
            Prop{"sprites/props/TreeGreen.png", Vector2{3300,3940}, PropType::TREE},    // 34
            Prop{"sprites/props/TreeGreen.png", Vector2{3400,3940}, PropType::TREE},    // 35 
            Prop{"sprites/props/TreeGreen.png", Vector2{3500,3940}, PropType::TREE},    // 36 
            Prop{"sprites/props/TreeGreen.png", Vector2{3600,3940}, PropType::TREE},    // 37
            Prop{"sprites/props/TreeGreen.png", Vector2{3700,3940}, PropType::TREE},    // 38
            Prop{"sprites/props/TreeGreen.png", Vector2{3800,3940}, PropType::TREE},    // 39
            Prop{"sprites/props/TreeGreen.png", Vector2{3900,3940}, PropType::TREE},    // 40 
            Prop{"sprites/props/TreeGreen.png", Vector2{4000,3940}, PropType::TREE},    // 41

            // large tree
            Prop{"sprites/props/TreeGreen.png", Vector2{2185,2050}, PropType::TREE, 8.f},
        };
        Props.emplace_back(Trees);

        std::vector<Prop> Grass
        {
            // --------------------------------------------- Far Left Column -----------------------------------------------

            // --------------------------------------------------- -4 -------------------------------------------------------
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1781,1826}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1781,1866}, PropType::GRASS, 4.f, true},

            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1845,1826}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1845,1866}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1781,1890}, PropType::GRASS, 4.f, true},
            // Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1781,1930}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1845,1890}, PropType::GRASS, 4.f, true},
            // Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1845,1930}, PropType::GRASS, 4.f, true},

            // --------------------------------------------------- -3 -------------------------------------------------------
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1781,1698}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1781,1738}, PropType::GRASS, 4.f, true},

            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1845,1698}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1845,1738}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1781,1762}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1781,1802}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1845,1762}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1845,1802}, PropType::GRASS, 4.f, true},

            // --------------------------------------------------- -2 -------------------------------------------------------
            // Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1781,1570}, PropType::GRASS, 4.f, true},
            // Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1781,1610}, PropType::GRASS, 4.f, true},

            // Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1845,1570}, PropType::GRASS, 4.f, true},
            // Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1845,1610}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1781,1634}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1781,1674}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1845,1634}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1845,1674}, PropType::GRASS, 4.f, true},

            // ---------------------------------------------------- -1 ------------------------------------------------------
            // Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1909,1314}, PropType::GRASS, 4.f, true},
            // Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1909,1354}, PropType::GRASS, 4.f, true},

            // Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1973,1314}, PropType::GRASS, 4.f, true},
            // Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1973,1354}, PropType::GRASS, 4.f, true},
            
            // Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1909,1378}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1909,1418}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1973,1378}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1973,1418}, PropType::GRASS, 4.f, true},

            // --------------------------------------------------- 0 -------------------------------------------------------
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1909,1442}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1909,1482}, PropType::GRASS, 4.f, true},

            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1973,1442}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1973,1482}, PropType::GRASS, 4.f, true},
            
            // Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1909,1506}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1909,1546}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1973,1506}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1973,1546}, PropType::GRASS, 4.f, true},

            // --------------------------------------------------- 1 -------------------------------------------------------
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1909,1570}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1909,1610}, PropType::GRASS, 4.f, true},

            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1973,1570}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1973,1610}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1909,1634}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1909,1674}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1973,1634}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1973,1674}, PropType::GRASS, 4.f, true},

            // --------------------------------------------------- 2 -------------------------------------------------------
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1909,1698}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1909,1738}, PropType::GRASS, 4.f, true},

            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1973,1698}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1973,1738}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1909,1762}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1909,1802}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1973,1762}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1973,1802}, PropType::GRASS, 4.f, true},

            // --------------------------------------------------- 3 -------------------------------------------------------
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1909,1826}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1909,1866}, PropType::GRASS, 4.f, true},

            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1973,1826}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1973,1866}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1909,1890}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1909,1930}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1973,1890}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{1973,1930}, PropType::GRASS, 4.f, true},

            // ------------------------------------------------ Left Column ------------------------------------------------

            // ---------------------------------------------------- -1 ------------------------------------------------------
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2037,1314}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2037,1354}, PropType::GRASS, 4.f, true},

            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2101,1314}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2101,1354}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2037,1378}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2037,1418}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2101,1378}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2101,1418}, PropType::GRASS, 4.f, true},

            // ---------------------------------------------------- 0 ------------------------------------------------------
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2037,1442}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2037,1482}, PropType::GRASS, 4.f, true},

            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2101,1442}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2101,1482}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2037,1506}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2037,1546}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2101,1506}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2101,1546}, PropType::GRASS, 4.f, true},

            // --------------------------------------------------- 1 -------------------------------------------------------
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2037,1570}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2037,1610}, PropType::GRASS, 4.f, true},

            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2101,1570}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2101,1610}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2037,1634}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2037,1674}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2101,1634}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2101,1674}, PropType::GRASS, 4.f, true},

            // --------------------------------------------------- 2 -------------------------------------------------------
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2037,1698}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2037,1738}, PropType::GRASS, 4.f, true},

            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2101,1698}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2101,1738}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2037,1762}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2037,1802}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2101,1762}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2101,1802}, PropType::GRASS, 4.f, true},

            // --------------------------------------------------- 3 -------------------------------------------------------        
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2037,1826}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2037,1866}, PropType::GRASS, 4.f, true},

            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2101,1826}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2101,1866}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2037,1890}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2037,1930}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2101,1890}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2101,1930}, PropType::GRASS, 4.f, true},

            // --------------------------------------------------- 4 ------------------------------------------------------- 
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2037,1954}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2037,1994}, PropType::GRASS, 4.f, true},

            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2101,1954}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2101,1994}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2037,2018}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2037,2058}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2101,2018}, PropType::GRASS, 4.f, true},
            // Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2101,2058}, PropType::GRASS, 4.f, true},

            // --------------------------------------------- Far Right Column ----------------------------------------------

            // --------------------------------------------------- -6 -------------------------------------------------------
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2421,1954}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2421,1994}, PropType::GRASS, 4.f, true},

            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2485,1954}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2485,1994}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2421,2018}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2421,2058}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2485,2018}, PropType::GRASS, 4.f, true},
            // Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2485,2058}, PropType::GRASS, 4.f, true},

            // --------------------------------------------------- -5 -------------------------------------------------------
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2421,1826}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2421,1866}, PropType::GRASS, 4.f, true},

            // Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2485,1826}, PropType::GRASS, 4.f, true},
            // Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2485,1866}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2421,1890}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2421,1930}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2485,1890}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2485,1930}, PropType::GRASS, 4.f, true},

            // --------------------------------------------------- -4 -------------------------------------------------------
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2421,1698}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2421,1738}, PropType::GRASS, 4.f, true},

            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2485,1698}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2485,1738}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2421,1762}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2421,1802}, PropType::GRASS, 4.f, true},
            
            // Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2485,1762}, PropType::GRASS, 4.f, true},
            // Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2485,1802}, PropType::GRASS, 4.f, true},

            // --------------------------------------------------- -3 -------------------------------------------------------
            // Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2421,1570}, PropType::GRASS, 4.f, true},
            // Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2421,1610}, PropType::GRASS, 4.f, true},

            // Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2485,1570}, PropType::GRASS, 4.f, true},
            // Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2485,1610}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2421,1634}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2421,1674}, PropType::GRASS, 4.f, true},
            
            // Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2485,1634}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2485,1674}, PropType::GRASS, 4.f, true},

            // ---------------------------------------------------- -2 ------------------------------------------------------
            // Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2421,1314}, PropType::GRASS, 4.f, true},
            // Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2421,1354}, PropType::GRASS, 4.f, true},

            // Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2485,1314}, PropType::GRASS, 4.f, true},
            // Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2485,1354}, PropType::GRASS, 4.f, true},
            
            // Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2421,1378}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2421,1418}, PropType::GRASS, 4.f, true},
            
            // Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2485,1378}, PropType::GRASS, 4.f, true},
            // Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2485,1418}, PropType::GRASS, 4.f, true},

            // ---------------------------------------------------- -1 ------------------------------------------------------
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2293,1314}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2293,1354}, PropType::GRASS, 4.f, true},

            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2357,1314}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2357,1354}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2293,1378}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2293,1418}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2357,1378}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2357,1418}, PropType::GRASS, 4.f, true},

            // ---------------------------------------------------- 0 ------------------------------------------------------
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2293,1442}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2293,1482}, PropType::GRASS, 4.f, true},

            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2357,1442}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2357,1482}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2293,1506}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2293,1546}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2357,1506}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2357,1546}, PropType::GRASS, 4.f, true},

            // --------------------------------------------------- 1 -------------------------------------------------------
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2293,1570}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2293,1610}, PropType::GRASS, 4.f, true},

            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2357,1570}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2357,1610}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2293,1634}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2293,1674}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2357,1634}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2357,1674}, PropType::GRASS, 4.f, true},

            // --------------------------------------------------- 2 -------------------------------------------------------
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2293,1698}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2293,1738}, PropType::GRASS, 4.f, true},

            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2357,1698}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2357,1738}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2293,1762}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2293,1802}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2357,1762}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2357,1802}, PropType::GRASS, 4.f, true},

            // --------------------------------------------------- 3 -------------------------------------------------------
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2293,1826}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2293,1866}, PropType::GRASS, 4.f, true},

            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2357,1826}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2357,1866}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2293,1890}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2293,1930}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2357,1890}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2357,1930}, PropType::GRASS, 4.f, true},

            // --------------------------------------------------- 4 -------------------------------------------------------
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2293,1954}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2293,1994}, PropType::GRASS, 4.f, true},

            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2357,1954}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2357,1994}, PropType::GRASS, 4.f, true},
            
            // Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2293,2018}, PropType::GRASS, 4.f, true},
            // Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2293,2058}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2357,2018}, PropType::GRASS, 4.f, true},
            // Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2357,2058}, PropType::GRASS, 4.f, true},

            // ------------------------------------------------ Right Column -----------------------------------------------

            // --------------------------------------------------- -1 -------------------------------------------------------
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2165,1314}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2165,1354}, PropType::GRASS, 4.f, true},

            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2229,1314}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2229,1354}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2165,1378}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2165,1418}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2229,1378}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2229,1418}, PropType::GRASS, 4.f, true},

            // --------------------------------------------------- 0 -------------------------------------------------------
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2165,1442}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2165,1482}, PropType::GRASS, 4.f, true},

            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2229,1442}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2229,1482}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2165,1506}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2165,1546}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2229,1506}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2229,1546}, PropType::GRASS, 4.f, true},

            // --------------------------------------------------- 1 -------------------------------------------------------
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2165,1570}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2165,1610}, PropType::GRASS, 4.f, true},

            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2229,1570}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2229,1610}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2165,1634}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2165,1674}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2229,1634}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2229,1674}, PropType::GRASS, 4.f, true},

            // --------------------------------------------------- 2 -------------------------------------------------------
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2165,1698}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2165,1738}, PropType::GRASS, 4.f, true},

            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2229,1698}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2229,1738}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2165,1762}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2165,1802}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2229,1762}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2229,1802}, PropType::GRASS, 4.f, true},

            // --------------------------------------------------- 3 -------------------------------------------------------
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2165,1826}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2165,1866}, PropType::GRASS, 4.f, true},

            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2229,1826}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2229,1866}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2165,1890}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2165,1930}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2229,1890}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2229,1930}, PropType::GRASS, 4.f, true},

            // --------------------------------------------------- 4 -------------------------------------------------------
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2165,1954}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2165,1994}, PropType::GRASS, 4.f, true},

            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2229,1954}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2229,1994}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2165,2018}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2165,2058}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2229,2018}, PropType::GRASS, 4.f, true},
            // Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{2229,2058}, PropType::GRASS, 4.f, true},

            // --------------------------------------------------------------------------------------------------------------
            // -----------------------------------------Dungeon Entrance Grass-----------------------------------------------
            // --------------------------------------------------------------------------------------------------------------

            // ------------------------------------------------ Left Column -------------------------------------------------
            // Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3465,150}, PropType::GRASS, 4.f, true},
            // Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3465,190}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3529,150}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3529,190}, PropType::GRASS, 4.f, true},
            
            // Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3465,214}, PropType::GRASS, 4.f, true},
            // Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3465,254}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3529,214}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3529,254}, PropType::GRASS, 4.f, true},

            // --------------------------------------------------------------------------------------------------------------
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3529,278}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3529,318}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3529,342}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3529,382}, PropType::GRASS, 4.f, true},
            // --------------------------------------------------------------------------------------------------------------
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3529,406}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3529,446}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3529,470}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3529,510}, PropType::GRASS, 4.f, true},
            // --------------------------------------------------------------------------------------------------------------
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3529,534}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3529,574}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3529,598}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3529,638}, PropType::GRASS, 4.f, true},
            // --------------------------------------------------------------------------------------------------------------
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3529,662}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3529,702}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3529,726}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3529,766}, PropType::GRASS, 4.f, true},
            // --------------------------------------------------------------------------------------------------------------
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3529,790}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3529,830}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3529,854}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3529,894}, PropType::GRASS, 4.f, true},
            // --------------------------------------------------------------------------------------------------------------
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3529,918}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3529,958}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3529,982}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3529,1022}, PropType::GRASS, 4.f, true},
            // --------------------------------------------------------------------------------------------------------------
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3529,1046}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3529,1086}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3529,1110}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3529,1150}, PropType::GRASS, 4.f, true},
            // --------------------------------------------------------------------------------------------------------------
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3529,1174}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3529,1214}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3529,1238}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3529,1278}, PropType::GRASS, 4.f, true},
            // --------------------------------------------------------------------------------------------------------------
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3529,1302}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3529,1342}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3529,1366}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3529,1406}, PropType::GRASS, 4.f, true},
            // --------------------------------------------------------------------------------------------------------------
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3529,1430}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3529,1470}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3529,1494}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3529,1534}, PropType::GRASS, 4.f, true},
            // --------------------------------------------------------------------------------------------------------------
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3529,1558}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3529,1598}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3529,1622}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3529,1662}, PropType::GRASS, 4.f, true},
            // --------------------------------------------------------------------------------------------------------------
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3529,1686}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3529,1726}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3529,1750}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3529,1790}, PropType::GRASS, 4.f, true},
            // --------------------------------------------------------------------------------------------------------------
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3529,1814}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3529,1854}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3529,1878}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3529,1918}, PropType::GRASS, 4.f, true},

            // ------------------------------------------------ Right Column -------------------------------------------------

            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3593,150}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3593,190}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3657,150}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3657,190}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3593,214}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3593,254}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3657,214}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3657,254}, PropType::GRASS, 4.f, true},

            // --------------------------------------------------------------------------------------------------------------

            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3593,278}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3593,318}, PropType::GRASS, 4.f, true},

            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3657,278}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3657,318}, PropType::GRASS, 4.f, true},

            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3593,342}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3593,382}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3657,342}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3657,382}, PropType::GRASS, 4.f, true},
            // --------------------------------------------------------------------------------------------------------------

            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3593,406}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3593,446}, PropType::GRASS, 4.f, true},

            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3657,406}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3657,446}, PropType::GRASS, 4.f, true},

            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3593,470}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3593,510}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3657,470}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3657,510}, PropType::GRASS, 4.f, true},
            // --------------------------------------------------------------------------------------------------------------

            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3593,534}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3593,574}, PropType::GRASS, 4.f, true},

            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3657,534}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3657,574}, PropType::GRASS, 4.f, true},

            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3593,598}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3593,638}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3657,598}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3657,638}, PropType::GRASS, 4.f, true},
            // --------------------------------------------------------------------------------------------------------------

            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3593,662}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3593,702}, PropType::GRASS, 4.f, true},

            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3657,662}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3657,702}, PropType::GRASS, 4.f, true},

            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3593,726}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3593,766}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3657,726}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3657,766}, PropType::GRASS, 4.f, true},
            // --------------------------------------------------------------------------------------------------------------

            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3593,790}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3593,830}, PropType::GRASS, 4.f, true},

            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3657,790}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3657,830}, PropType::GRASS, 4.f, true},

            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3593,854}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3593,894}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3657,854}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3657,894}, PropType::GRASS, 4.f, true},
            // --------------------------------------------------------------------------------------------------------------
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3593,918}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3593,958}, PropType::GRASS, 4.f, true},

            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3657,918}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3657,958}, PropType::GRASS, 4.f, true},

            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3593,982}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3593,1022}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3657,982}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3657,1022}, PropType::GRASS, 4.f, true},
            // --------------------------------------------------------------------------------------------------------------

            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3593,1046}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3593,1086}, PropType::GRASS, 4.f, true},

            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3657,1046}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3657,1086}, PropType::GRASS, 4.f, true},

            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3593,1110}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3593,1150}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3657,1110}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3657,1150}, PropType::GRASS, 4.f, true},
            // --------------------------------------------------------------------------------------------------------------

            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3593,1174}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3593,1214}, PropType::GRASS, 4.f, true},

            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3657,1174}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3657,1214}, PropType::GRASS, 4.f, true},

            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3593,1238}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3593,1278}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3657,1238}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3657,1278}, PropType::GRASS, 4.f, true},
            // --------------------------------------------------------------------------------------------------------------

            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3593,1302}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3593,1342}, PropType::GRASS, 4.f, true},

            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3657,1302}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3657,1342}, PropType::GRASS, 4.f, true},

            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3593,1366}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3593,1406}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3657,1366}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3657,1406}, PropType::GRASS, 4.f, true},
            // --------------------------------------------------------------------------------------------------------------

            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3593,1430}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3593,1470}, PropType::GRASS, 4.f, true},

            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3657,1430}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3657,1470}, PropType::GRASS, 4.f, true},

            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3593,1494}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3593,1534}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3657,1494}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3657,1534}, PropType::GRASS, 4.f, true},
            // --------------------------------------------------------------------------------------------------------------

            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3593,1558}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3593,1598}, PropType::GRASS, 4.f, true},

            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3657,1558}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3657,1598}, PropType::GRASS, 4.f, true},

            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3593,1622}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3593,1662}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3657,1622}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3657,1662}, PropType::GRASS, 4.f, true},
            // --------------------------------------------------------------------------------------------------------------

            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3593,1686}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3593,1726}, PropType::GRASS, 4.f, true},

            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3657,1686}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3657,1726}, PropType::GRASS, 4.f, true},

            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3593,1750}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3593,1790}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3657,1750}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3657,1790}, PropType::GRASS, 4.f, true},
            // --------------------------------------------------------------------------------------------------------------

            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3593,1814}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3593,1854}, PropType::GRASS, 4.f, true},

            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3657,1814}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3657,1854}, PropType::GRASS, 4.f, true},

            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3593,1878}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3593,1918}, PropType::GRASS, 4.f, true},
            
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3657,1878}, PropType::GRASS, 4.f, true},
            Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{3657,1918}, PropType::GRASS, 4.f, true},


        };
        Props.emplace_back(Grass);

        std::vector<Prop> Treasure
        {
            Prop{Sprite{"sprites/props/TreasureChestBig.png", 4, 1, 1.f/4.f}, Vector2{2198,1677}, PropType::BIGTREASURE, 4.f, false, true, Progress::ACT_O, PropType::NPC_O, Texture2D{LoadTexture("sprites/props/TreasureHeart.png")}, "Life Stone", 4.f},
            Prop{Sprite{"sprites/props/TreasureChest.png", 4, 1, 1.f/4.f}, Vector2{2250,3350}, PropType::TREASURE, 4.f, false, true, Progress::ACT_O, PropType::NPC_O, Texture2D{LoadTexture("sprites/props/AltarTopRight.png")}, "Top Right Altar Piece"},
            Prop{Sprite{"sprites/props/TreasureChest.png", 4, 1, 1.f/4.f}, Vector2{2250,3150}, PropType::TREASURE, 4.f, false, true, Progress::ACT_O, PropType::NPC_O, Texture2D{LoadTexture("sprites/props/AltarTop.png")}, "Top Altar Piece"},
            Prop{Sprite{"sprites/props/TreasureChest.png", 4, 1, 1.f/4.f}, Vector2{130,210}, PropType::TREASURE, 4.f, false, true, Progress::ACT_O, PropType::NPC_O, Texture2D{LoadTexture("sprites/props/AltarTopLeft.png")}, "Top Left Altar Piece"},
            Prop{Sprite{"sprites/props/TreasureChest.png", 4, 1, 1.f/4.f}, Vector2{2250,2950}, PropType::TREASURE, 4.f, false, true, Progress::ACT_O, PropType::NPC_O, Texture2D{LoadTexture("sprites/props/AltarBotLeft.png")}, "Bottom Left Altar Piece"},
            Prop{Sprite{"sprites/props/TreasureChest.png", 4, 1, 1.f/4.f}, Vector2{2250,2750}, PropType::TREASURE, 4.f, false, true, Progress::ACT_O, PropType::NPC_O, Texture2D{LoadTexture("sprites/props/AltarBot.png")}, "Bottom Altar Piece"},
            Prop{Sprite{"sprites/props/TreasureChest.png", 4, 1, 1.f/4.f}, Vector2{2250,2550}, PropType::TREASURE, 4.f, false, true, Progress::ACT_O, PropType::NPC_O, Texture2D{LoadTexture("sprites/props/AltarBotRight.png")}, "Bottom Right Altar Piece"},
        };
        Props.emplace_back(Treasure);

        std::vector<Prop> NPCS
        {
            Prop{Sprite{"sprites/npc/Didi.png", 4, 1, 1.f/8.f}, Vector2{3163, 2853}, PropType::NPC_A, 3.f, false, true},
            Prop{Sprite{"sprites/npc/Jade.png", 4, 1, 1.f/8.f}, Vector2{1546, 2959}, PropType::NPC_B, 3.f, false, true},
            // Prop{Sprite{"sprites/npc/Didi.png", 4, 1, 1.f/8.f}, Vector2{2492, 3634}, PropType::NPC_A, 3.f, false, true},
            // Prop{Sprite{"sprites/npc/Jade.png", 4, 1, 1.f/8.f}, Vector2{2044, 3634}, PropType::NPC_B, 3.f, false, true},
            Prop{Sprite{"sprites/npc/Son.png", 4, 1, 1.f/8.f}, Vector2{1240, 2036}, PropType::NPC_C, 3.f, false, true, Progress::ACT_II, PropType::NPC_B},
        };
        Props.emplace_back(NPCS);

        std::vector<Prop> AnimatedAltarPieces
        {
            Prop{Sprite{"sprites/props/AltarTopLeftAnimated.png", 14, 1, 1/14.f}, Vector2{700,1025}, PropType::ANIMATEDALTAR, "Top Left Altar Piece", true, true},
            Prop{Sprite{"sprites/props/AltarTopAnimated.png", 14, 1, 1/14.f}, Vector2{764,1025}, PropType::ANIMATEDALTAR, "Top Altar Piece", true, true},
            Prop{Sprite{"sprites/props/AltarTopRightAnimated.png", 14, 1, 1/14.f}, Vector2{828,1025}, PropType::ANIMATEDALTAR, "Top Right Altar Piece", true, true},
            Prop{Sprite{"sprites/props/AltarBotLeftAnimated.png", 14, 1, 1/14.f}, Vector2{700,1089}, PropType::ANIMATEDALTAR, "Bottom Left Altar Piece", true, true},
            Prop{Sprite{"sprites/props/AltarBotAnimated.png", 14, 1, 1/14.f}, Vector2{764,1089}, PropType::ANIMATEDALTAR, "Bottom Altar Piece", true, true},
            Prop{Sprite{"sprites/props/AltarBotRightAnimated.png", 14, 1, 1/14.f}, Vector2{828,1089}, PropType::ANIMATEDALTAR, "Bottom Right Altar Piece", true, true},
        };
        Props.emplace_back(AnimatedAltarPieces);
        
        return Props;
    }

    // Initialize enemies
    std::vector<Enemy> InitializeEnemies(Background& MapBG, Window& Window)
    {
        std::vector<Enemy> Enemies{};

        Enemy BrownBearTwo
        {
            Sprite{"sprites/enemies/bear/brown_idle.png", 4, 4},
            Sprite{"sprites/enemies/bear/brown_walk.png", 4, 4},
            Sprite{"sprites/enemies/bear/brown_attack.png", 4, 4},
            Sprite{"sprites/enemies/bear/brown_hurt.png", 1, 4},
            Sprite{"sprites/enemies/bear/brown_death.png", 11, 4},
            EnemyType::BEAR, EnemyType::NORMAL, Vector2{2940, 740}, &Window, &MapBG
        };
        Enemies.emplace_back(BrownBearTwo);

        Enemy BrownBearThree
        {
            Sprite{"sprites/enemies/bear/brown_idle.png", 4, 4},
            Sprite{"sprites/enemies/bear/brown_walk.png", 4, 4},
            Sprite{"sprites/enemies/bear/brown_attack.png", 4, 4},
            Sprite{"sprites/enemies/bear/brown_hurt.png", 1, 4},
            Sprite{"sprites/enemies/bear/brown_death.png", 11, 4},
            EnemyType::BEAR, EnemyType::NORMAL, Vector2{2927, 1090}, &Window, &MapBG
        };
        Enemies.emplace_back(BrownBearThree);

        Enemy LightBrownBearTwo
        {
            Sprite{"sprites/enemies/bear/lightbrown_idle.png", 4, 4},
            Sprite{"sprites/enemies/bear/lightbrown_walk.png", 4, 4},
            Sprite{"sprites/enemies/bear/lightbrown_attack.png", 4, 4},
            Sprite{"sprites/enemies/bear/lightbrown_hurt.png", 1, 4},
            Sprite{"sprites/enemies/bear/lightbrown_death.png", 11, 4},
            EnemyType::BEAR, EnemyType::NORMAL, Vector2{3102, 899}, &Window, &MapBG
        };
        Enemies.emplace_back(LightBrownBearTwo);

        Enemy LightBrownBearThree
        {
            Sprite{"sprites/enemies/bear/lightbrown_idle.png", 4, 4},
            Sprite{"sprites/enemies/bear/lightbrown_walk.png", 4, 4},
            Sprite{"sprites/enemies/bear/lightbrown_attack.png", 4, 4},
            Sprite{"sprites/enemies/bear/lightbrown_hurt.png", 1, 4},
            Sprite{"sprites/enemies/bear/lightbrown_death.png", 11, 4},
            EnemyType::BEAR, EnemyType::NORMAL, Vector2{2835, 905}, &Window, &MapBG
        };
        Enemies.emplace_back(LightBrownBearThree);

        // ----------------------------------- Test enemies below ------------------------------------

        Enemy BrownBear
        {
            Sprite{"sprites/enemies/bear/brown_idle.png", 4, 4},
            Sprite{"sprites/enemies/bear/brown_walk.png", 4, 4},
            Sprite{"sprites/enemies/bear/brown_attack.png", 4, 4},
            Sprite{"sprites/enemies/bear/brown_hurt.png", 1, 4},
            Sprite{"sprites/enemies/bear/brown_death.png", 11, 4},
            EnemyType::BEAR, EnemyType::NORMAL, Vector2{1800, 3566}, &Window, &MapBG, 1
        };
        Enemies.emplace_back(BrownBear);

        Enemy LightBrownBear
        {
            Sprite{"sprites/enemies/bear/lightbrown_idle.png", 4, 4},
            Sprite{"sprites/enemies/bear/lightbrown_walk.png", 4, 4},
            Sprite{"sprites/enemies/bear/lightbrown_attack.png", 4, 4},
            Sprite{"sprites/enemies/bear/lightbrown_hurt.png", 1, 4},
            Sprite{"sprites/enemies/bear/lightbrown_death.png", 11, 4},
            EnemyType::BEAR, EnemyType::BOSS, Vector2{1750, 3366}, &Window, &MapBG, 5, 5.f
        };
        Enemies.emplace_back(LightBrownBear);

        Enemy RedBeholder
        {
            Sprite{"sprites/enemies/beholder/red_idle.png", 4, 4},
            Sprite{"sprites/enemies/beholder/red_walk.png", 4, 4},
            Sprite{"sprites/enemies/beholder/red_attack.png", 4, 4},
            Sprite{"sprites/enemies/beholder/red_hurt.png", 4, 4},
            Sprite{"sprites/enemies/beholder/red_death.png", 11, 4},
            EnemyType::BEHOLDER, EnemyType::NORMAL, Vector2{1550, 3466}, &Window, &MapBG, 3
        };
        Enemies.emplace_back(RedBeholder);

        Enemy WhiteGhost
        {
            Sprite{"sprites/enemies/ghost/white_idle.png", 4, 4},
            Sprite{"sprites/enemies/ghost/white_walk.png", 4, 4},
            Sprite{"sprites/enemies/ghost/white_attack.png", 4, 4},
            Sprite{"sprites/enemies/ghost/white_hurt.png", 1, 4},
            Sprite{"sprites/enemies/ghost/white_death.png", 11, 4},
            EnemyType::GHOST, EnemyType::NORMAL, Vector2{1850, 3566}, &Window, &MapBG, 1
        };
        Enemies.emplace_back(WhiteGhost);
    
        Enemy RedNecro
        {
            Sprite{"sprites/enemies/necromancer/red_idle.png", 4, 4},
            Sprite{"sprites/enemies/necromancer/red_walk.png", 4, 4},
            Sprite{"sprites/enemies/necromancer/red_attack.png", 4, 4},
            Sprite{"sprites/enemies/necromancer/red_hurt.png", 1, 4},
            Sprite{"sprites/enemies/necromancer/red_death.png", 11, 4},
            EnemyType::NECROMANCER, EnemyType::NORMAL, Vector2{1750, 3366}, &Window, &MapBG, 1
        };
        Enemies.emplace_back(RedNecro);

        Enemy PurpleCreature
        {
            Sprite{"sprites/enemies/creature/purple_idle.png", 4, 4},
            Sprite{"sprites/enemies/creature/purple_walk.png", 4, 4},
            Sprite{"sprites/enemies/creature/purple_attack.png", 4, 4},
            Sprite{"sprites/enemies/creature/purple_hurt.png", 1, 4},
            Sprite{"sprites/enemies/creature/purple_death.png", 11, 4},
            EnemyType::CREATURE, EnemyType::NORMAL, Vector2{1660, 3366}, &Window, &MapBG, 1
        };
        Enemies.emplace_back(PurpleCreature);

        Enemy GreenImp
        {
            Sprite{"sprites/enemies/imp/green_idle.png", 4, 4},
            Sprite{"sprites/enemies/imp/green_walk.png", 4, 4},
            Sprite{"sprites/enemies/imp/green_attack.png", 4, 4},
            Sprite{"sprites/enemies/imp/green_hurt.png", 1, 4},
            Sprite{"sprites/enemies/imp/green_death.png", 11, 4},
            EnemyType::IMP, EnemyType::NORMAL, Vector2{1800, 3666}, &Window, &MapBG, 1
        };
        Enemies.emplace_back(GreenImp);

        // Enemy PinkMushroom
        // {
        //     Sprite{"sprites/enemies/mushroom/pink_idle.png", 4, 4},
        //     Sprite{"sprites/enemies/mushroom/pink_walk.png", 4, 4},
        //     Sprite{"sprites/enemies/mushroom/pink_attack.png", 4, 4},
        //     Sprite{"sprites/enemies/mushroom/pink_hurt.png", 4, 4},
        //     Sprite{"sprites/enemies/mushroom/pink_death.png", 11, 4},
        //     EnemyType::MUSHROOM, EnemyType::NORMAL, Vector2{1500, 3666}, &Window, &MapBG, 1
        // };
        // Enemies.emplace_back(PinkMushroom);

        // Enemy BlueShadow
        // {
        //     Sprite{"sprites/enemies/shadow/blue_idle.png", 4, 4},
        //     Sprite{"sprites/enemies/shadow/blue_walk.png", 4, 4},
        //     Sprite{"sprites/enemies/shadow/blue_attack.png", 4, 4},
        //     Sprite{"sprites/enemies/shadow/blue_hurt.png", 1, 4},
        //     Sprite{"sprites/enemies/shadow/blue_death.png", 11, 4},
        //     EnemyType::SHADOW, EnemyType::NORMAL, Vector2{1800, 3666}, &Window, &MapBG
        // };
        // Enemies.emplace_back(BlueShadow);

        // Enemy BrownSpider
        // {
        //     Sprite{"sprites/enemies/spider/brown_idle.png", 4, 4},
        //     Sprite{"sprites/enemies/spider/brown_walk.png", 4, 4},
        //     Sprite{"sprites/enemies/spider/brown_attack.png", 4, 4},
        //     Sprite{"sprites/enemies/spider/brown_hurt.png", 1, 4},
        //     Sprite{"sprites/enemies/spider/brown_death.png", 11, 4},
        //     EnemyType::SPIDER, EnemyType::NORMAL, Vector2{1600, 3666}, &Window, &MapBG
        // };
        // Enemies.emplace_back(BrownSpider);
        
        Enemy BlueToad
        {
            Sprite{"sprites/enemies/toad/blue_idle.png", 4, 4},
            Sprite{"sprites/enemies/toad/blue_walk.png", 4, 4},
            Sprite{"sprites/enemies/toad/blue_attack.png", 4, 4},
            Sprite{"sprites/enemies/toad/blue_hurt.png", 1, 4},
            Sprite{"sprites/enemies/toad/blue_death.png", 11, 4},
            EnemyType::TOAD, EnemyType::NORMAL, Vector2{1600, 3466}, &Window, &MapBG, 10
        };
        Enemies.emplace_back(BlueToad);

        // Wildlife NPCs

        Enemy FoxFamOne
        {
            Sprite{"sprites/npc/wildlife/fox/fox_family_idle.png", 5, 2},
            Sprite{"sprites/npc/wildlife/fox/fox_family_idle2.png", 14, 2},
            Sprite{"sprites/npc/wildlife/fox/fox_family_walk.png", 8, 2},
            Sprite{"sprites/npc/wildlife/fox/fox_family_lazy.png", 13, 2},
            Sprite{"sprites/npc/wildlife/fox/fox_family_sleep.png", 6, 2},
            EnemyType::FOX, Vector2{473, 2524}, &Window, &MapBG
        };
        Enemies.emplace_back(FoxFamOne);
        
        // Enemy FoxFamTwo
        // {
        //     Sprite{"sprites/npc/wildlife/fox/fox_family_idle.png", 5, 2},
        //     Sprite{"sprites/npc/wildlife/fox/fox_family_idle2.png", 14, 2},
        //     Sprite{"sprites/npc/wildlife/fox/fox_family_walk.png", 8, 2},
        //     Sprite{"sprites/npc/wildlife/fox/fox_family_lazy.png", 13, 2},
        //     Sprite{"sprites/npc/wildlife/fox/fox_family_sleep.png", 6, 2},
        //     EnemyType::FOX, Vector2{247, 2524}, &Window, &MapBG
        // };
        // Enemies.emplace_back(FoxFamTwo);

        // Enemy FoxFamThree
        // {
        //     Sprite{"sprites/npc/wildlife/fox/fox_family_idle.png", 5, 2},
        //     Sprite{"sprites/npc/wildlife/fox/fox_family_idle2.png", 14, 2},
        //     Sprite{"sprites/npc/wildlife/fox/fox_family_walk.png", 8, 2},
        //     Sprite{"sprites/npc/wildlife/fox/fox_family_lazy.png", 13, 2},
        //     Sprite{"sprites/npc/wildlife/fox/fox_family_sleep.png", 6, 2},
        //     EnemyType::FOX, Vector2{307, 2724}, &Window, &MapBG, 2.5f
        // };
        // Enemies.emplace_back(FoxFamThree);

        // Enemy FoxFamFour
        // {
        //     Sprite{"sprites/npc/wildlife/fox/fox_family_idle.png", 5, 2},
        //     Sprite{"sprites/npc/wildlife/fox/fox_family_idle2.png", 14, 2},
        //     Sprite{"sprites/npc/wildlife/fox/fox_family_walk.png", 8, 2},
        //     Sprite{"sprites/npc/wildlife/fox/fox_family_lazy.png", 13, 2},
        //     Sprite{"sprites/npc/wildlife/fox/fox_family_sleep.png", 6, 2},
        //     EnemyType::FOX, Vector2{428, 2840}, &Window, &MapBG, 2.5f
        // };
        // Enemies.emplace_back(FoxFamFour);

        // Enemy FoxFamFive
        // {
        //     Sprite{"sprites/npc/wildlife/fox/fox_family_idle.png", 5, 2},
        //     Sprite{"sprites/npc/wildlife/fox/fox_family_idle2.png", 14, 2},
        //     Sprite{"sprites/npc/wildlife/fox/fox_family_walk.png", 8, 2},
        //     Sprite{"sprites/npc/wildlife/fox/fox_family_lazy.png", 13, 2},
        //     Sprite{"sprites/npc/wildlife/fox/fox_family_sleep.png", 6, 2},
        //     EnemyType::FOX, Vector2{354, 2661}, &Window, &MapBG, 2.5f
        // };
        // Enemies.emplace_back(FoxFamFive);
        
        // Enemy Squirrel
        // {
        //     Sprite{"sprites/npc/wildlife/squirrel/squirrel_idle.png", 6, 2},
        //     Sprite{"sprites/npc/wildlife/squirrel/squirrel_idle2.png", 6, 2},
        //     Sprite{"sprites/npc/wildlife/squirrel/squirrel_walk.png", 4, 2},
        //     Sprite{"sprites/npc/wildlife/squirrel/squirrel_eat.png", 2, 2},
        //     Sprite{"sprites/npc/wildlife/squirrel/squirrel_eat.png", 2, 2},
        //     EnemyType::SQUIRREL, Vector2{354, 2661}, &Window, &MapBG, 2.0f
        // };
        // Enemies.emplace_back(Squirrel);

        return Enemies;
    }

    // Initialize crows
    std::vector<Enemy> InitializeCrows(Background& MapBG, Window& Window)
    {
        std::vector<Enemy> Crows{};

        Enemy CrowOne
        {
            Sprite{"sprites/npc/wildlife/crow/crow_idle.png", 8, 2},
            Sprite{"sprites/npc/wildlife/crow/crow_idle2.png", 5, 2},
            Sprite{"sprites/npc/wildlife/crow/crow_flying.png", 5, 2},
            Sprite{"sprites/npc/wildlife/crow/crow_walk.png", 4, 2},
            Sprite{"sprites/npc/wildlife/crow/crow_idle2.png", 5, 2},
            EnemyType::CROW, Vector2{837, 1921}, &Window, &MapBG, 2.5f
        };
        Crows.emplace_back(CrowOne);

        Enemy CrowTwo
        {
            Sprite{"sprites/npc/wildlife/crow/crow_idle.png", 8, 2},
            Sprite{"sprites/npc/wildlife/crow/crow_idle2.png", 5, 2},
            Sprite{"sprites/npc/wildlife/crow/crow_flying.png", 5, 2},
            Sprite{"sprites/npc/wildlife/crow/crow_walk.png", 4, 2},
            Sprite{"sprites/npc/wildlife/crow/crow_idle2.png", 5, 2},
            EnemyType::CROW, Vector2{1042, 2026}, &Window, &MapBG, 2.5f
        };
        Crows.emplace_back(CrowTwo);

        Enemy CrowThree
        {
            Sprite{"sprites/npc/wildlife/crow/crow_idle.png", 8, 2},
            Sprite{"sprites/npc/wildlife/crow/crow_idle2.png", 5, 2},
            Sprite{"sprites/npc/wildlife/crow/crow_flying.png", 5, 2},
            Sprite{"sprites/npc/wildlife/crow/crow_walk.png", 4, 2},
            Sprite{"sprites/npc/wildlife/crow/crow_idle2.png", 5, 2},
            EnemyType::CROW, Vector2{847, 2037}, &Window, &MapBG, 2.5f
        };
        Crows.emplace_back(CrowThree);

        Enemy CrowFour
        {
            Sprite{"sprites/npc/wildlife/crow/crow_idle.png", 8, 2},
            Sprite{"sprites/npc/wildlife/crow/crow_idle2.png", 5, 2},
            Sprite{"sprites/npc/wildlife/crow/crow_flying.png", 5, 2},
            Sprite{"sprites/npc/wildlife/crow/crow_walk.png", 4, 2},
            Sprite{"sprites/npc/wildlife/crow/crow_idle2.png", 5, 2},
            EnemyType::CROW, Vector2{932, 2256}, &Window, &MapBG, 2.5f
        };
        Crows.emplace_back(CrowFour);
        
        Enemy CrowFive
        {
            Sprite{"sprites/npc/wildlife/crow/crow_idle.png", 8, 2},
            Sprite{"sprites/npc/wildlife/crow/crow_idle2.png", 5, 2},
            Sprite{"sprites/npc/wildlife/crow/crow_flying.png", 5, 2},
            Sprite{"sprites/npc/wildlife/crow/crow_walk.png", 4, 2},
            Sprite{"sprites/npc/wildlife/crow/crow_idle2.png", 5, 2},
            EnemyType::CROW, Vector2{1037, 1826}, &Window, &MapBG, 2.5f
        };
        Crows.emplace_back(CrowFive);
        
        Enemy CrowSix
        {
            Sprite{"sprites/npc/wildlife/crow/crow_idle.png", 8, 2},
            Sprite{"sprites/npc/wildlife/crow/crow_idle2.png", 5, 2},
            Sprite{"sprites/npc/wildlife/crow/crow_flying.png", 5, 2},
            Sprite{"sprites/npc/wildlife/crow/crow_walk.png", 4, 2},
            Sprite{"sprites/npc/wildlife/crow/crow_idle2.png", 5, 2},
            EnemyType::CROW, Vector2{1055, 1955}, &Window, &MapBG, 2.5f
        };
        Crows.emplace_back(CrowSix);
        
        return Crows;
    }

} // namespace Game