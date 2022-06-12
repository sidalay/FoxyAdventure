#include "game.hpp"
#include <assert.h>
#include <vector>

namespace Game 
{
    void Run() 
    {
        Window Window {1280, 720};                                                                        
        Game::Initialize(Window, "Cryptex Adventure");      // Set the program's configurations

        if (IsWindowReady())
        {
            // Initialization ---------------------------
            GameTexture Textures;
            Game::Info Info{Background{Textures}, 0, 0.f, 0.f, 0.f};
            Game::Objects Objects{
                Game::InitializeHud(Textures), 
                Game::InitializeFox(Window, Info, Textures),
                {Game::InitializePropsUnder(Textures), Game::InitializePropsOver(Textures)},
                {Game::InitializeEnemies(Info.Map, Window, Textures)},
                {Game::InitializeCrows(Info.Map, Window, Textures)},
                {Game::InitializeTrees(Textures)},
                Game::InitializePauseFox(Textures),
                Game::InitializeButtons(Textures)
            };

            // Start Game Loop
            while (!Info.ExitGame) 
            {
                Game::Tick(Window, Info, Objects, Textures);
            }
        }

        CloseWindow();
    }

    void Initialize(Window& Window, const std::string& Title)
    {
        SetTraceLogLevel(LOG_WARNING);
        InitWindow(Window.x, Window.y, Title.c_str());
        SetTargetFPS(144);
        SetExitKey(0);
        HideCursor();
    }

    void CheckScreenSizing(Window& Window)
    {
        // set window.x / window.y to resized dimensions
        if (IsWindowResized()) 
        {
            Window.x = GetScreenWidth();
            Window.y = GetScreenHeight();
        }

        // check for alt + enter [FULLSCREEN]
        if (IsKeyPressed(KEY_ENTER) && ((IsKeyDown(KEY_RIGHT_ALT) || IsKeyDown(KEY_LEFT_ALT))))
        {
            ToggleFullscreen();
        }
    }

    void Tick(Window& Window, Game::Info& Info, Game::Objects& Objects, GameTexture& Textures)
    {
        Game::CheckScreenSizing(Window);

        BeginDrawing();

        if (Info.State == Game::State::RUNNING) {

            ClearBackground(BLACK);

            Game::Update(Info, Objects);
            Game::Draw(Info, Objects);
        }
        else if (Info.State == Game::State::MAINMENU) {

            ClearBackground(BLACK);

            Game::MainMenuUpdate(Info);
            Game::MainMenuDraw(Info);
        }
        else if (Info.State == Game::State::PAUSED) {

            ClearBackground(BLACK);

            Game::PauseUpdate(Info, Objects);
            Game::PauseDraw(Info, Objects, Textures);
        }
        else if (Info.State == Game::State::EXIT) {

            ClearBackground(BLACK);

            Game::ExitUpdate(Info);
            Game::ExitDraw(Info);
        }
        else if (Info.State == Game::State::GAMEOVER) {

            ClearBackground(BLACK);

            Game::GameOverUpdate(Info);
            Game::GameOverDraw(Info);
        }
        else if (Info.State == Game::State::TRANSITION) {

            Game::Transition(Info);
        }

        EndDrawing();
    }

    void Update(Game::Info& Info, Game::Objects& Objects)
    {
        float DeltaTime{GetFrameTime()};

        Info.Map.Tick(Objects.Fox.GetWorldPos());
        Objects.Fox.Tick(DeltaTime, Objects.Props, Objects.Enemies, Objects.Trees);
        if (!Objects.Fox.IsAlive()) {
            Info.NextState = Game::State::GAMEOVER;
            Info.State = Game::State::TRANSITION;
        }

        for (auto& Enemy:Objects.Enemies) {
            Enemy.Tick(DeltaTime, Objects.Props, Objects.Fox.GetWorldPos(), Objects.Fox.GetCharPos(), Objects.Enemies, Objects.Trees);
        }

        for (auto& Crow:Objects.Crows) {
            Crow.Tick(DeltaTime, Objects.Props, Objects.Fox.GetWorldPos(), Objects.Fox.GetCharPos(), Objects.Enemies, Objects.Trees);
        }

        for (auto& Proptype:Objects.Props.Under) {
            for (auto& Prop:Proptype) {
                Prop.Tick(DeltaTime);
            }
        }

        for (auto& Tree:Objects.Trees) {
            Tree.Tick(DeltaTime);
        }

        for (auto& Proptype:Objects.Props.Over) {
            for (auto& Prop:Proptype) {
                Prop.Tick(DeltaTime);
            }
        }

        if (IsKeyPressed(KEY_L))
            Objects.Fox.SetSleep();

        if (IsKeyPressed(KEY_P)) {
            Info.NextState = Game::State::PAUSED;
            Info.State = Game::State::TRANSITION;
        }
        else if (IsKeyPressed(KEY_F5) || IsKeyPressed(KEY_PERIOD) || IsKeyPressed(KEY_ESCAPE)) {
            Info.NextState = Game::State::EXIT;
            Info.State = Game::State::TRANSITION;
        }

        // Dev Tools--------------------------------------
        if (IsKeyPressed(KEY_GRAVE))
            Info.DevToolsOn = !Info.DevToolsOn;

        if (Info.DevToolsOn) {

            if (IsKeyPressed(KEY_ONE)) {
                Info.NoClipOn = !Info.NoClipOn;
                Objects.Fox.SwitchCollidable();
            }

            if (IsKeyPressed(KEY_TWO)) 
                Info.DrawRectanglesOn = !Info.DrawRectanglesOn;

            if (IsKeyPressed(KEY_THREE))
                Info.ShowFPS = !Info.ShowFPS;

            if (IsKeyPressed(KEY_FOUR))
                Info.ShowPos = !Info.ShowPos;

            if (IsKeyPressed(KEY_ZERO))
                Info.ShowDevTools = !Info.ShowDevTools;

            if (Objects.Fox.GetHealth() < 11)
                if (IsKeyPressed(KEY_EQUAL))
                    Objects.Fox.AddHealth(0.5f);

            if (Objects.Fox.GetHealth() > 0)
                if (IsKeyPressed(KEY_MINUS))
                    Objects.Fox.AddHealth(-0.5f);
        }
        else {
            if (Info.NoClipOn)
                Objects.Fox.SwitchCollidable();
                
            Info.NoClipOn = false;
            Info.DrawRectanglesOn = false;
            Info.ShowFPS = false;
            Info.ShowPos = false;
            Info.ShowDevTools = true;
        }
    }

    void Draw(Game::Info& Info, Game::Objects& Objects)
    {
        Info.Map.Draw();

        for (auto& PropType:Objects.Props.Under) {
            for (auto& Prop:PropType) {
                Prop.Draw(Objects.Fox.GetWorldPos());
                
                // Debugging --------------------
                if (Info.DrawRectanglesOn)
                    if (Prop.WithinScreen(Objects.Fox.GetWorldPos())) 
                        Game::DrawCollisionRecs(Prop, Objects.Fox.GetWorldPos());
            }
        }

        Objects.Fox.Draw();

        // Debugging --------------------
        if (Info.DrawRectanglesOn) {
            Game::DrawCollisionRecs(Objects.Fox);
            Game::DrawAttackRecs(Objects.Fox);
        }

        for (auto& Enemy:Objects.Enemies) {
            Enemy.Draw(Objects.Fox.GetWorldPos());

            // Debugging --------------------
            if (Info.DrawRectanglesOn) 
                if (Enemy.WithinScreen(Objects.Fox.GetWorldPos())) {
                    Game::DrawCollisionRecs(Enemy, CLITERAL(Color){ 205, 0, 255, 150 });
                    Game::DrawAttackRecs(Enemy);
                }
        }

        for (auto& Tree:Objects.Trees) {
            Tree.Draw(Objects.Fox.GetWorldPos());

            // Debugging --------------------
            if (Info.DrawRectanglesOn)
                if (Tree.WithinScreen(Objects.Fox.GetWorldPos())) 
                    Game::DrawCollisionRecs(Tree, Objects.Fox.GetWorldPos());
        }

        for (auto& Crow:Objects.Crows) {
            Crow.Draw(Objects.Fox.GetWorldPos());

            // Debugging --------------------
            if (Info.DrawRectanglesOn)
                if (Crow.WithinScreen(Objects.Fox.GetWorldPos())) 
                    Game::DrawCollisionRecs(Crow);
        }

        for (auto& PropType:Objects.Props.Over) {
            for (auto& Prop:PropType) {
                Prop.Draw(Objects.Fox.GetWorldPos());
                
                // Debugging --------------------
                if (Info.DrawRectanglesOn)
                    if (Prop.WithinScreen(Objects.Fox.GetWorldPos())) 
                        Game::DrawCollisionRecs(Prop, Objects.Fox.GetWorldPos(), Color{ 200, 122, 255, 150 });
            }
        }

        Objects.Fox.DrawIndicator();
        
        Objects.Hud.Draw(Objects.Fox.GetHealth(), Objects.Fox.GetEmotion());

        Info.Map.DrawMiniMap(Objects.Fox.GetWorldPos());

        // Debugging --------------------
        if (Info.DevToolsOn) {
            if (Info.ShowPos) {
                DrawRectangle(15, 145, 150, 50, CLITERAL(Color){0,0,0,170});
                DrawText(TextFormat("Player.x: %i", (int)Objects.Fox.GetWorldPos().x + 615), 20, 150, 20, WHITE);
                DrawText(TextFormat("Player.y: %i", (int)Objects.Fox.GetWorldPos().y + 335), 20, 170, 20, WHITE);
            }

            if (Info.ShowFPS) {
                DrawRectangle(15, 215, 90, 30, CLITERAL(Color){0,0,0,170});
                DrawFPS(20, 221);
            }
            
            if (Info.ShowDevTools) {
                DrawRectangle(15, 270, 220, 25, CLITERAL(Color){0,0,0,170});
                DrawText("   Dev Tools Menu", 20, 273, 20, WHITE);
                DrawRectangle(15, 300, 220, 240, CLITERAL(Color){0,0,0,170});
                DrawText("  ---- Toggles ----", 20, 310, 20, WHITE);
                DrawText("[`] Dev Tools", 20, 335, 20, !Info.DevToolsOn ? WHITE : LIME);
                DrawText("[1] Noclip", 20, 355, 20, !Info.NoClipOn ? WHITE : LIME);
                DrawText("[2] CollisionRecs", 20, 375, 20, !Info.DrawRectanglesOn ? WHITE : LIME);
                DrawText("[3] FPS", 20, 395, 20, !Info.ShowFPS ? WHITE : LIME);
                DrawText("[4] Position", 20, 415, 20, !Info.ShowPos ? WHITE : LIME);
                DrawText("[0] Tools Menu", 20, 435, 20, !Info.ShowDevTools ? WHITE : LIME);
                DrawText("    ---- Misc ----", 20, 460, 20, WHITE);
                DrawText("[-] Decrease HP", 20, 485, 20, !IsKeyDown(KEY_MINUS) ? WHITE : LIME);
                DrawText("[+] Increase HP", 20, 505, 20, !IsKeyDown(KEY_EQUAL) ? WHITE : LIME);
            }
        }
    }

    void PauseUpdate(Game::Info& Info, Game::Objects& Objects)
    {

        if (IsKeyDown(KEY_L)) {
            Info.PauseFoxIndex = 3;
        }
        else if (IsKeyDown(KEY_W) || IsKeyDown(KEY_A) || IsKeyDown(KEY_S) || IsKeyDown(KEY_D)) {
            if (IsKeyDown(KEY_LEFT_SHIFT)) {
                Info.PauseFoxIndex = 2;
            }
            else {
                Info.PauseFoxIndex = 1;
            }
        }
        else if (IsKeyDown(KEY_SPACE) || IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            Info.PauseFoxIndex = 4;
        }
        else {
            Info.PauseFoxIndex = 0;
        }

        for (auto& Fox:Objects.PauseFox) {
            Fox.Tick(GetFrameTime());
        }

        if (IsKeyPressed(KEY_P)) {
            Info.NextState = Game::State::RUNNING;
            Info.State = Game::State::TRANSITION;
            
            // Add audio functionality here later
        }
        else if (IsKeyPressed(KEY_F5) || IsKeyPressed(KEY_PERIOD) || IsKeyPressed(KEY_ESCAPE)) {
            Info.NextState = Game::State::EXIT;
            Info.State = Game::State::TRANSITION;
        }
    }

    void PauseDraw(const Game::Info& Info, Game::Objects& Objects, GameTexture& Textures)
    {
        DrawTextureEx(Textures.PauseBackground, Vector2{0.f,0.f}, 0.0f, 4.f, WHITE);

        // PauseFoxIndex controls which Fox sprite is drawn
        DrawTexturePro(Objects.PauseFox.at(Info.PauseFoxIndex).Texture, Objects.PauseFox.at(Info.PauseFoxIndex).GetSourceRec(), Objects.PauseFox.at(Info.PauseFoxIndex).GetPosRec(Vector2{674.f,396.f}, 4.f), Vector2{}, 0.f, WHITE);

        // Draw Buttons Depending on which are pushed
        if (IsKeyDown(KEY_W)) DrawTextureEx(Objects.Buttons.at(0), Vector2{208.f,124.f}, 0.f, 4.f, WHITE);
        if (IsKeyDown(KEY_A)) DrawTextureEx(Objects.Buttons.at(1), Vector2{160.f,180.f}, 0.f, 4.f, WHITE);
        if (IsKeyDown(KEY_S)) DrawTextureEx(Objects.Buttons.at(2), Vector2{208.f,180.f}, 0.f, 4.f, WHITE);
        if (IsKeyDown(KEY_D)) DrawTextureEx(Objects.Buttons.at(3), Vector2{256.f,180.f}, 0.f, 4.f, WHITE);
        if (IsKeyDown(KEY_L)) DrawTextureEx(Objects.Buttons.at(4), Vector2{160.f,460.f}, 0.f, 4.f, WHITE);
        if (IsKeyDown(KEY_M)) DrawTextureEx(Objects.Buttons.at(5), Vector2{160.f,372.f}, 0.f, 4.f, WHITE);
        if (IsKeyDown(KEY_LEFT_SHIFT)) DrawTextureEx(Objects.Buttons.at(6), Vector2{160.f,276.f}, 0.f, 4.f, WHITE);
        if (IsKeyDown(KEY_SPACE)) DrawTextureEx(Objects.Buttons.at(7), Vector2{152.f,552.f}, 0.f, 4.f, WHITE);
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) DrawTextureEx(Objects.Buttons.at(8), Vector2{264.f,548.f}, 0.f, 4.f, WHITE);
    }

    void ExitUpdate(Game::Info& Info)
    {
        if (IsKeyPressed(KEY_A) || IsKeyPressed(KEY_D) || IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_RIGHT)) {
            Info.ExitIsYes = !Info.ExitIsYes;
        }

        if (Info.ExitIsYes) {
            if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER)) {
                Info.NextState = Game::State::MAINMENU;
                Info.State = Game::State::TRANSITION;
            }
        }
        else {
            if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER)) {
                Info.NextState = Game::State::RUNNING;
                Info.State = Game::State::TRANSITION;
            }
        }

        if (IsKeyPressed(KEY_F5) || IsKeyPressed(KEY_PERIOD) || IsKeyPressed(KEY_ESCAPE)) {
            Info.NextState = Game::State::RUNNING;
            Info.State = Game::State::TRANSITION;
        }
    }

    void ExitDraw(const Game::Info& Info)
    {
        DrawText("Quit the game?", 500, 320, 40, WHITE);
        DrawText("Yes", 570, 380, 20, WHITE);
        DrawText("No", 690, 380, 20, WHITE);
        
        if (Info.ExitIsYes) {
            DrawRectangle(560, 375, 55, 30, Color{ 0, 238, 135, 100 });
        }
        else {
            DrawRectangle(675, 375, 55, 30, Color{ 0, 238, 135, 100 });
        }
    }

    void MainMenuUpdate(Game::Info& Info)
    {
        if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_S) || IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_DOWN)) {
            Info.MainMenuStart = !Info.MainMenuStart;
        }

        if (!Info.MainMenuStart) {
            if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER)) {
                Info.ExitGame = true;
            }
        }
        else {
            if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER)) {
                Info.NextState = Game::State::RUNNING;
                Info.State = Game::State::TRANSITION;
            }
        }
    }

    void MainMenuDraw(const Game::Info& Info)
    {
        DrawText("CRYPTEX ADVENTURE", 385, 240, 40, WHITE);
        DrawText("START", 565, 380, 30, WHITE);
        DrawText("QUIT", 582, 430, 30, WHITE);
        
        if (Info.MainMenuStart) {
            DrawRectangle(557, 370, 123, 47, Color{ 0, 238, 135, 100 });
        }
        else {
            DrawRectangle(557, 420, 123, 47, Color{ 0, 238, 135, 100 });
        }
    }

    void GameOverUpdate(Game::Info& Info)
    {
        Info.NextState = Game::State::EXIT;
        Info.State = Game::State::TRANSITION;
    }

    void GameOverDraw(const Game::Info& Info)
    {

    }

    void Transition(Game::Info& Info)
    {
        const float MaxTransitionTime{0.3f};
        
        if (Info.State != Game::State::TRANSITION) {
            if (Info.TransitionInTime < MaxTransitionTime) {
                    Info.TransitionInTime = GetFrameTime();
                    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, Info.Opacity));
                    Info.Opacity -= 0.01f;
            }
            else {
                Info.TransitionInTime = 0.f;
                Info.Opacity = 1.f;
            }
        }
        else {
            if (Info.TransitionOutTime < MaxTransitionTime) {
                Info.TransitionOutTime += GetFrameTime();
                DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, Info.Opacity));
                Info.Opacity += 0.01f;
            }
            else {
                Info.TransitionOutTime = 0.f;
                Info.Opacity = 0.f;
                Info.State = Info.NextState;
            }
        }
    }

    HUD InitializeHud(GameTexture& Textures)
    {
        return HUD(Textures);
    }

    Character InitializeFox(Window& Window, Game::Info& Info, GameTexture& Textures)
    {
        return Character {
            Sprite{Textures.FoxIdle, 4, 4}, 
            Sprite{Textures.FoxWalk, 4, 4},
            Sprite{Textures.FoxRun, 4, 4}, 
            Sprite{Textures.FoxMelee, 4, 4},
            Sprite{Textures.FoxHit, 2, 4}, 
            Sprite{Textures.FoxDie, 1, 4},
            Sprite{Textures.FoxPush, 4, 4},
            Sprite{Textures.FoxSleeping, 4, 1}, 
            Sprite{Textures.FoxItemGot, 1, 4},
            Textures,
            &Window, &Info.Map
        };
    }

    std::array<Sprite,5> InitializePauseFox(GameTexture& Textures)
    {
        return std::array<Sprite, 5>{
            Sprite{Textures.FoxIdle, 4, 4},
            Sprite{Textures.FoxWalk, 4, 4},
            Sprite{Textures.FoxRun, 4, 4},
            Sprite{Textures.FoxSleeping, 4, 1},
            Sprite{Textures.FoxMelee, 4, 1}
        };
    }

    std::array<const Texture2D,9> InitializeButtons(GameTexture& Textures)
    {
        return std::array<const Texture2D, 9>{
            Textures.ButtonW,
            Textures.ButtonA,
            Textures.ButtonS,
            Textures.ButtonD,
            Textures.ButtonL,
            Textures.ButtonM,
            Textures.Shift,
            Textures.Space,
            Textures.Lmouse
        };
    }

    std::vector<std::vector<Prop>> InitializePropsUnder(GameTexture& Textures)
    {
        std::vector<std::vector<Prop>> Props{};

        /*  Placeholder prop to allow "!" to turn off. Need to figure out a better solution to this later.
            Removing Placeholder will make "!" keep showing even if you walk out of an interactable range.
            It will also introduce many bugs such as getting permanantly stuck in place if space is pressed when not in an interactable range.
        */
        std::vector<Prop> Placeholder
        {
            Prop{Textures.Placeholder, Vector2{0,0}, PropType::PLACEHOLDER, Textures, 4.f, false, true}
        };
        Props.emplace_back(Placeholder);

        std::vector<Prop> DungeonEntrance
        {
            Prop{Textures.DungeonEntrance, Vector2{3520,60}, PropType::DUNGEONLEFT, Textures, 4.f},
            Prop{Textures.DungeonEntrance, Vector2{3520,60}, PropType::DUNGEONRIGHT, Textures, 4.f},
            Prop{Textures.DungeonEntrance, Vector2{3520,60}, PropType::DUNGEON, Textures, 4.f, false, true},
        };
        Props.emplace_back(DungeonEntrance);

        std::vector<Prop> Grass
        {   
            // --------------------------------------------- Far Left Column -----------------------------------------------

            // ---------------------------------------------------- -4 ------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1816,1806}, PropType::GRASS, Textures, Textures.Placeholder, 4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1816,1846}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1880,1806}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1880,1846}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1816,1870}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1816,1910}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1880,1870}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1880,1910}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            // ---------------------------------------------------- -3 ------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1816,1678}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1816,1718}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1880,1678}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1880,1718}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1816,1742}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1816,1782}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1880,1742}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1880,1782}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            // ---------------------------------------------------- -2 ------------------------------------------------------
            // Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1816,1550}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1816,1590}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            // Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1880,1550}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1880,1590}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1816,1614}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1816,1654}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1880,1614}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1880,1654}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            // ---------------------------------------------------- -1 ------------------------------------------------------
            // Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1944,1294}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1944,1334}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            // Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2008,1294}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2008,1334}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            // Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1944,1358}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1944,1398}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2008,1358}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2008,1398}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            // ---------------------------------------------------- 0 ------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1944,1442}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1944,1462}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2008,1442}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2008,1462}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1944,1486}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1944,1526}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2008,1486}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2008,1526}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            // ---------------------------------------------------- 1 ------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1944,1550}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1944,1590}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2008,1550}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2008,1590}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1944,1614}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1944,1654}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2008,1614}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2008,1654}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            // ---------------------------------------------------- 2 ------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1944,1678}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1944,1718}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2008,1678}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2008,1718}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1944,1742}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1944,1782}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2008,1742}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2008,1782}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            // ---------------------------------------------------- 3 ------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1944,1806}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1944,1846}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2008,1806}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2008,1846}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1944,1870}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1944,1910}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2008,1870}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2008,1910}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            // ---------------------------------------------------- 3 ------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1944,1806}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1944,1846}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2008,1806}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2008,1846}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1944,1870}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1944,1910}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2008,1870}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2008,1910}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            // ---------------------------------------------------- 4 ------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1944,1934}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1944,1974}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2008,1934}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2008,1974}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1944,1998}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1944,2038}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2008,1998}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2008,2038}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            // ------------------------------------------------ Left Column ------------------------------------------------

            // ---------------------------------------------------- -1 ------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2072,1294}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2072,1334}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            // Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2136,1294}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2136,1334}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2072,1358}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2072,1398}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2136,1358}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2136,1398}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            // ---------------------------------------------------- 0 ------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2072,1422}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2072,1462}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2136,1422}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2136,1462}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2072,1486}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2072,1526}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2136,1486}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2136,1526}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            // ---------------------------------------------------- 1 ------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2072,1550}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2072,1590}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2136,1550}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2136,1590}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2072,1614}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2072,1654}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2136,1614}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2136,1654}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            // --------------------------------------------------- 2 -------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2072,1678}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2072,1718}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2136,1678}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2136,1718}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2072,1742}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2072,1782}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2136,1742}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2136,1782}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            // --------------------------------------------------- 3 -------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2072,1806}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2072,1846}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2136,1806}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2136,1846}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2072,1870}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2072,1910}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2136,1870}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2136,1910}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            // --------------------------------------------------- 4 -------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2072,1934}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2072,1974}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2136,1934}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2136,1974}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2072,1998}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2072,2038}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2136,1998}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2136,2038}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},



            // --------------------------------------------- Far Right Column ----------------------------------------------

            // --------------------------------------------------- -6 -------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2456,1934}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2456,1974}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2520,1934}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2520,1974}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2456,1998}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2456,2038}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2520,1998}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2520,2038}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            // --------------------------------------------------- -5 -------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2456,1806}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2456,1846}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2520,1806}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2520,1846}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2456,1870}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2456,1910}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2520,1870}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2520,1910}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            // --------------------------------------------------- -4 -------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2456,1678}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2456,1718}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2520,1678}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2520,1718}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2456,1742}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2456,1782}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2520,1742}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2520,1782}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            // --------------------------------------------------- -3 -------------------------------------------------------
            // Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2456,1550}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2456,1590}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            // Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2520,1550}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2520,1590}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2456,1614}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2456,1654}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2520,1614}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2520,1654}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            // --------------------------------------------------- -2 -------------------------------------------------------
            // Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2456,1422}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2456,1462}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            // Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2520,1422}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2520,1462}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            // Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2456,1486}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2456,1526}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2520,1486}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2520,1526}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            // --------------------------------------------------- -1 -------------------------------------------------------
            // Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2328,1294}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2328,1334}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2392,1294}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2392,1334}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2328,1358}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2328,1398}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2392,1358}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2392,1398}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            // --------------------------------------------------- 0 -------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2328,1422}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2328,1462}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2392,1422}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2392,1462}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2328,1486}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2328,1526}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2392,1486}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2392,1526}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            // --------------------------------------------------- 1 -------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2328,1550}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2328,1590}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2392,1550}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2392,1590}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2328,1614}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2328,1654}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2392,1614}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2392,1654}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            // --------------------------------------------------- 2 -------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2328,1678}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2328,1718}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2392,1678}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2392,1718}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2328,1742}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2328,1782}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2392,1742}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2392,1782}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            // --------------------------------------------------- 3 -------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2328,1806}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2328,1846}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2392,1806}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2392,1846}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2328,1870}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2328,1910}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2392,1870}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2392,1910}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            // --------------------------------------------------- 4 -------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2328,1934}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2328,1974}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2392,1934}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2392,1974}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2328,1998}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2328,2038}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2392,1998}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2392,2038}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            // ------------------------------------------------ Right Column -----------------------------------------------

            // --------------------------------------------------- -1 -------------------------------------------------------
            // Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2200,1294}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2200,1334}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            // Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2264,1294}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2264,1334}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2200,1358}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2200,1398}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2264,1358}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2264,1398}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            // --------------------------------------------------- 0 -------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2200,1422}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2200,1462}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2264,1422}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2264,1462}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2200,1486}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2200,1526}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2264,1486}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2264,1526}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},


            // --------------------------------------------------- 1 -------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2200,1550}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2200,1590}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2264,1550}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2264,1590}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2200,1614}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2200,1654}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2264,1614}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2264,1654}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            // --------------------------------------------------- 2 -------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2200,1678}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2200,1718}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2264,1678}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2264,1718}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2200,1742}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2200,1782}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2264,1742}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2264,1782}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            // --------------------------------------------------- 3 -------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2200,1806}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2200,1846}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2264,1806}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2264,1846}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2200,1870}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2200,1910}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2264,1870}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2264,1910}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            // --------------------------------------------------- 4 -------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2200,1934}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2200,1974}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2264,1934}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2264,1974}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2200,1998}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2200,2038}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2264,1998}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2264,2038}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            // --------------------------------------------------------------------------------------------------------------
            // -----------------------------------------Dungeon Entrance Grass-----------------------------------------------
            // --------------------------------------------------------------------------------------------------------------

            // ------------------------------------------------ Left Column -------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500,170}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500,210}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564,170}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564,210}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500,234}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500,274}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564,234}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564,274}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            // --------------------------------------------------------------------------------------------------------------

            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500,298}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500,338}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564,298}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564,338}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500,362}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500,402}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564,362}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564,402}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            // --------------------------------------------------------------------------------------------------------------
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500,426}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500,446}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564,426}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564,446}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500,490}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500,530}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564,490}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564,530}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            // --------------------------------------------------------------------------------------------------------------
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500,554}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500,594}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564,554}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564,594}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500,618}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500,658}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564,618}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564,658}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            // --------------------------------------------------------------------------------------------------------------
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500,682}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500,722}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564,682}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564,722}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500,746}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500,786}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564,746}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564,786}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            // --------------------------------------------------------------------------------------------------------------
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500,810}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500,850}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564,810}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564,850}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500,874}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500,914}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564,874}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564,914}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            // --------------------------------------------------------------------------------------------------------------
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500,938}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500,978}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564,938}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564,978}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500,1002}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500,1042}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564,1002}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564,1042}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            // --------------------------------------------------------------------------------------------------------------
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500,1066}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500,1106}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564,1066}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564,1106}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500,1130}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500,1170}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564,1130}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564,1170}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            // --------------------------------------------------------------------------------------------------------------
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500,1194}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500,1234}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564,1194}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564,1234}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500,1258}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500,1298}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564,1258}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564,1298}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            // --------------------------------------------------------------------------------------------------------------
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500,1322}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500,1362}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564,1322}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564,1362}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500,1386}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500,1426}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564,1386}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564,1426}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            // --------------------------------------------------------------------------------------------------------------
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500,1450}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500,1490}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564,1450}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564,1490}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500,1514}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500,1554}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564,1514}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564,1554}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            // --------------------------------------------------------------------------------------------------------------
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500,1578}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500,1618}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564,1578}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564,1618}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500,1642}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500,1682}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564,1642}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564,1682}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            // --------------------------------------------------------------------------------------------------------------
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500,1706}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500,1746}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564,1706}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564,1746}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500,1770}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500,1810}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564,1770}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564,1810}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            // --------------------------------------------------------------------------------------------------------------
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500,1834}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500,1874}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564,1834}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564,1874}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500,1898}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500,1938}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564,1898}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564,1938}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            // --------------------------------------------------------------------------------------------------------------
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500,1962}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500,2002}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564,1962}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564,2002}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500,2026}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500,2046}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564,2026}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564,2046}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            // ------------------------------------------------ Right Column -------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628,170}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628,210}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692,170}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692,210}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628,234}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628,274}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692,234}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692,274}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            // --------------------------------------------------------------------------------------------------------------

            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628,298}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628,338}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692,298}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692,338}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628,362}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628,402}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692,362}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692,402}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            // --------------------------------------------------------------------------------------------------------------
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628,426}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628,446}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692,426}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692,446}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628,490}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628,530}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692,490}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692,530}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            // --------------------------------------------------------------------------------------------------------------
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628,554}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628,594}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692,554}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692,594}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628,618}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628,658}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692,618}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692,658}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            // --------------------------------------------------------------------------------------------------------------
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628,682}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628,722}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692,682}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692,722}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628,746}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628,786}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692,746}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692,786}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            // --------------------------------------------------------------------------------------------------------------
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628,810}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628,850}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692,810}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692,850}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628,874}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628,914}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692,874}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692,914}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            // --------------------------------------------------------------------------------------------------------------
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628,938}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628,978}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692,938}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692,978}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628,1002}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628,1042}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692,1002}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692,1042}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            // --------------------------------------------------------------------------------------------------------------
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628,1066}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628,1106}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692,1066}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692,1106}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628,1130}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628,1170}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692,1130}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692,1170}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            // --------------------------------------------------------------------------------------------------------------
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628,1194}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628,1234}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692,1194}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692,1234}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628,1258}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628,1298}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692,1258}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692,1298}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            // --------------------------------------------------------------------------------------------------------------
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628,1322}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628,1362}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692,1322}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692,1362}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628,1386}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628,1426}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692,1386}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692,1426}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            // --------------------------------------------------------------------------------------------------------------
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628,1450}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628,1490}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692,1450}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692,1490}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628,1514}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628,1554}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692,1514}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692,1554}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            // --------------------------------------------------------------------------------------------------------------
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628,1578}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628,1618}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692,1578}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692,1618}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628,1642}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628,1682}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692,1642}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692,1682}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            // --------------------------------------------------------------------------------------------------------------
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628,1706}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628,1746}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692,1706}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692,1746}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628,1770}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628,1810}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692,1770}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692,1810}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            // --------------------------------------------------------------------------------------------------------------
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628,1834}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628,1874}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692,1834}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692,1874}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628,1898}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628,1938}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692,1898}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692,1938}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            // --------------------------------------------------------------------------------------------------------------
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628,1962}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628,2002}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692,1962}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692,2002}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628,2026}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628,2046}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692,2026}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692,2046}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            
        };
        Props.emplace_back(Grass);

        std::vector<Prop> Walls
        {
            // ---------------------------------------- Left Side Hill -----------------------------------

            // Left Side Walls
            Prop{Textures.GrassWallLeft, Vector2{768, 2688}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.GrassWallLeft, Vector2{768, 2752}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.GrassWallLeft, Vector2{768, 2816}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.GrassWallLeft, Vector2{768, 2880}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.GrassWallLeft, Vector2{768, 2944}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.GrassWallLeft, Vector2{768, 3008}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.GrassWallLeft, Vector2{768, 3072}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.GrassWallLeft, Vector2{768, 3136}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.GrassWallLeft, Vector2{768, 3200}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.GrassWallLeft, Vector2{768, 3264}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.GrassWallLeft, Vector2{768, 3328}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.GrassWallLeft, Vector2{768, 3392}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.GrassWallLeft, Vector2{768, 3456}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.GrassWallLeft, Vector2{768, 3520}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.GrassWallBotLeft, Vector2{768, 3584}, PropType::BOTTOMWALL, Textures},
            
            // Bottom Walls
            Prop{Textures.GrassWallBottom, Vector2{832, 3584}, PropType::BOTTOMWALL, Textures},
            Prop{Textures.GrassWallBottom, Vector2{896, 3584}, PropType::BOTTOMWALL, Textures},
            Prop{Textures.GrassWallBottom, Vector2{960, 3584}, PropType::BOTTOMWALL, Textures},
            Prop{Textures.GrassWallBottom, Vector2{1024, 3584}, PropType::BOTTOMWALL, Textures},
            Prop{Textures.GrassWallBottom, Vector2{1216, 3584}, PropType::BOTTOMWALL, Textures},
            Prop{Textures.GrassWallBotRight, Vector2{1280, 3584}, PropType::BOTTOMWALL, Textures},
        
            // Bottom Right Walls
            Prop{Textures.GrassWallRight, Vector2{1280, 3520}, PropType::RIGHTSIDEWALL, Textures},
            Prop{Textures.GrassWallRight, Vector2{1280, 3456}, PropType::RIGHTSIDEWALL, Textures},
            Prop{Textures.GrassWallRight, Vector2{1280, 3392}, PropType::RIGHTSIDEWALL, Textures},
            Prop{Textures.GrassWallRight, Vector2{1280, 3328}, PropType::RIGHTSIDEWALL, Textures},
            Prop{Textures.GrassWallInnerRight, Vector2{1280, 3264}, PropType::RIGHTSIDEWALL, Textures},
            
            // Bottom Walls
            Prop{Textures.GrassWallBottom, Vector2{1344, 3264}, PropType::BOTTOMWALL, Textures},
            Prop{Textures.GrassWallBottom, Vector2{1408, 3264}, PropType::BOTTOMWALL, Textures},
            Prop{Textures.GrassWallBottom, Vector2{1472, 3264}, PropType::BOTTOMWALL, Textures},
            Prop{Textures.GrassWallBottom, Vector2{1536, 3264}, PropType::BOTTOMWALL, Textures},
            Prop{Textures.GrassWallBottom, Vector2{1600, 3264}, PropType::BOTTOMWALL, Textures},
            Prop{Textures.GrassWallBottom, Vector2{1664, 3264}, PropType::BOTTOMWALL, Textures},
            Prop{Textures.GrassWallBottom, Vector2{1728, 3264}, PropType::BOTTOMWALL, Textures},
            Prop{Textures.GrassWallBottom, Vector2{1792, 3264}, PropType::BOTTOMWALL, Textures},
            Prop{Textures.GrassWallBottom, Vector2{1856, 3264}, PropType::BOTTOMWALL, Textures},
            Prop{Textures.GrassWallBotRight, Vector2{1920, 3264}, PropType::BOTTOMWALL, Textures},     

            // Right Side Walls
            Prop{Textures.GrassWallRight, Vector2{1920, 3200}, PropType::RIGHTSIDEWALL, Textures},
            Prop{Textures.GrassWallRight, Vector2{1920, 3136}, PropType::RIGHTSIDEWALL, Textures},
            Prop{Textures.GrassWallRight, Vector2{1920, 3072}, PropType::RIGHTSIDEWALL, Textures},
            Prop{Textures.GrassWallRight, Vector2{1920, 3008}, PropType::RIGHTSIDEWALL, Textures},
            Prop{Textures.GrassWallRight, Vector2{1920, 2944}, PropType::RIGHTSIDEWALL, Textures},
            Prop{Textures.GrassWallRight, Vector2{1920, 2880}, PropType::RIGHTSIDEWALL, Textures},
            Prop{Textures.GrassWallRight, Vector2{1920, 2816}, PropType::RIGHTSIDEWALL, Textures},
            Prop{Textures.GrassWallRight, Vector2{1920, 2752}, PropType::RIGHTSIDEWALL, Textures},
            Prop{Textures.GrassWallRight, Vector2{1920, 2688}, PropType::RIGHTSIDEWALL, Textures},
            Prop{Textures.GrassWallTopRight, Vector2{1920, 2624}, PropType::TOPWALL, Textures},

            // Top Walls
            Prop{Textures.GrassWallTop, Vector2{1856, 2624}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{1792, 2624}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{1728, 2624}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{1664, 2624}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{1600, 2624}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{1536, 2624}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{1472, 2624}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{1408, 2624}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{1344, 2624}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{1280, 2624}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{1216, 2624}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{1152, 2624}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{1088, 2624}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{1024, 2624}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{960, 2624}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{896, 2624}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{832, 2624}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTopLeft, Vector2{768, 2624}, PropType::TOPWALL, Textures},

            // Fences
            Prop{Textures.FenceDown, Vector2{832, 3662}, PropType::FENCE, Textures},
            Prop{Textures.FenceDown, Vector2{896, 3662}, PropType::FENCE, Textures},
            Prop{Textures.FenceDown, Vector2{960, 3662}, PropType::FENCE, Textures},
            Prop{Textures.FenceDown, Vector2{1024, 3662}, PropType::FENCE, Textures},
            Prop{Textures.FenceDown, Vector2{1216, 3662}, PropType::FENCE, Textures},

            // ---------------------------------------- Garden Fences -----------------------------------

            // Left Side
            Prop{Textures.FenceLeft, Vector2{1688, 3070}, PropType::FENCE, Textures},
            Prop{Textures.FenceLeft, Vector2{1688, 3198}, PropType::FENCE, Textures},

            // Right Side
            Prop{Textures.FenceRight, Vector2{1925, 3070}, PropType::FENCE, Textures},
            Prop{Textures.FenceRight, Vector2{1925, 3128}, PropType::FENCE, Textures},
            Prop{Textures.FenceRight, Vector2{1925, 3198}, PropType::FENCE, Textures},

            // Top Side
            Prop{Textures.FenceUp, Vector2{1728, 3070}, PropType::FENCE, Textures},
            Prop{Textures.FenceUp, Vector2{1856, 3070}, PropType::FENCE, Textures},
            
            // Bottom Side
            Prop{Textures.FenceDown, Vector2{1728, 3215}, PropType::FENCE, Textures},
            Prop{Textures.FenceDown, Vector2{1792, 3215}, PropType::FENCE, Textures},
            Prop{Textures.FenceDown, Vector2{1856, 3215}, PropType::FENCE, Textures},


            // ---------------------------------------- Left Side Inner Hill -----------------------------------
            
            // Holes
            Prop{Textures.Hole, Vector2{960, 3008}, PropType::HOLE, Textures},
            Prop{Textures.Hole, Vector2{1088, 3008}, PropType::HOLE, Textures},


            // Left Side Walls
            Prop{Textures.WallLeft, Vector2{896, 2816}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.WallLeft, Vector2{896, 2880}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.WallLeft, Vector2{896, 2944}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.WallLeft, Vector2{896, 3008}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.WallLeft, Vector2{896, 3072}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.WallLeft, Vector2{896, 3136}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.WallLeft, Vector2{896, 3200}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.WallBotLeft, Vector2{896, 3264}, PropType::BOTTOMWALL, Textures},

            // Bottom Walls
            Prop{Textures.WallBottom, Vector2{960, 3264}, PropType::BOTTOMWALL, Textures},
            Prop{Textures.WallBottom, Vector2{1024, 3264}, PropType::BOTTOMWALL, Textures},
            Prop{Textures.WallBotRight, Vector2{1152, 3264}, PropType::BOTTOMWALL, Textures},

            // Right Side Walls
            Prop{Textures.WallRight, Vector2{1152, 3200}, PropType::RIGHTSIDEWALL, Textures},
            Prop{Textures.WallRight, Vector2{1152, 3136}, PropType::RIGHTSIDEWALL, Textures},
            Prop{Textures.WallRight, Vector2{1152, 3072}, PropType::RIGHTSIDEWALL, Textures},
            Prop{Textures.WallRight, Vector2{1152, 3008}, PropType::RIGHTSIDEWALL, Textures},
            Prop{Textures.WallRight, Vector2{1152, 2944}, PropType::RIGHTSIDEWALL, Textures},
            Prop{Textures.WallRight, Vector2{1152, 2880}, PropType::RIGHTSIDEWALL, Textures},
            Prop{Textures.WallRight, Vector2{1152, 2816}, PropType::RIGHTSIDEWALL, Textures},
            Prop{Textures.WallTopRight, Vector2{1152, 2752}, PropType::TOPWALL, Textures},

            // Top Walls
            Prop{Textures.GrassWallTop, Vector2{1088, 2752}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{1024, 2752}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{960, 2752}, PropType::TOPWALL, Textures},
            Prop{Textures.WallTopLeft, Vector2{896, 2752}, PropType::TOPWALL, Textures},

            // ---------------------------------------- Right Side Hill -----------------------------------

            // Left Side Walls
            Prop{Textures.GrassWallLeft, Vector2{2688, 2560}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.GrassWallLeft, Vector2{2688, 2624}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.GrassWallLeft, Vector2{2688, 2688}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.GrassWallLeft, Vector2{2688, 2752}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.GrassWallLeft, Vector2{2688, 2816}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.GrassWallLeft, Vector2{2688, 2880}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.GrassWallLeft, Vector2{2688, 2944}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.GrassWallLeft, Vector2{2688, 3008}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.GrassWallLeft, Vector2{2688, 3072}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.GrassWallLeft, Vector2{2688, 3136}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.GrassWallLeft, Vector2{2688, 3200}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.GrassWallLeft, Vector2{2688, 3264}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.GrassWallLeft, Vector2{2688, 3328}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.GrassWallLeft, Vector2{2688, 3392}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.GrassWallBotLeft, Vector2{2688, 3456}, PropType::BOTTOMWALL, Textures},

            // Bottom Walls
            Prop{Textures.GrassWallBottom, Vector2{2752, 3456}, PropType::BOTTOMWALL, Textures},
            Prop{Textures.GrassWallBottom, Vector2{2816, 3456}, PropType::BOTTOMWALL, Textures},
            Prop{Textures.GrassWallBottom, Vector2{2880, 3456}, PropType::BOTTOMWALL, Textures},
            Prop{Textures.GrassWallBottom, Vector2{3264, 3456}, PropType::BOTTOMWALL, Textures},
            Prop{Textures.GrassWallBottom, Vector2{3328, 3456}, PropType::BOTTOMWALL, Textures},
            Prop{Textures.GrassWallBottom, Vector2{3392, 3456}, PropType::BOTTOMWALL, Textures},
            Prop{Textures.GrassWallBottom, Vector2{3456, 3456}, PropType::BOTTOMWALL, Textures},
            Prop{Textures.GrassWallBottom, Vector2{3520, 3456}, PropType::BOTTOMWALL, Textures},
            Prop{Textures.GrassWallBottom, Vector2{3584, 3456}, PropType::BOTTOMWALL, Textures},
            Prop{Textures.GrassWallBottom, Vector2{3648, 3456}, PropType::BOTTOMWALL, Textures},
            Prop{Textures.GrassWallBottom, Vector2{3712, 3456}, PropType::BOTTOMWALL, Textures},
            Prop{Textures.GrassWallBottom, Vector2{3776, 3456}, PropType::BOTTOMWALL, Textures},
            Prop{Textures.GrassWallBotRight, Vector2{3840, 3456}, PropType::BOTTOMWALL, Textures},

            // Right Side Walls
            Prop{Textures.GrassWallRight, Vector2{3840, 3392}, PropType::RIGHTSIDEWALL, Textures},
            Prop{Textures.GrassWallRight, Vector2{3840, 3328}, PropType::RIGHTSIDEWALL, Textures},
            Prop{Textures.GrassWallRight, Vector2{3840, 3264}, PropType::RIGHTSIDEWALL, Textures},
            Prop{Textures.GrassWallRight, Vector2{3840, 3200}, PropType::RIGHTSIDEWALL, Textures},
            Prop{Textures.GrassWallRight, Vector2{3840, 3136}, PropType::RIGHTSIDEWALL, Textures},
            Prop{Textures.GrassWallRight, Vector2{3840, 3072}, PropType::RIGHTSIDEWALL, Textures},
            Prop{Textures.GrassWallRight, Vector2{3840, 3008}, PropType::RIGHTSIDEWALL, Textures},
            Prop{Textures.GrassWallRight, Vector2{3840, 2944}, PropType::RIGHTSIDEWALL, Textures},
            Prop{Textures.GrassWallRight, Vector2{3840, 2880}, PropType::RIGHTSIDEWALL, Textures},
            Prop{Textures.GrassWallRight, Vector2{3840, 2816}, PropType::RIGHTSIDEWALL, Textures},
            Prop{Textures.GrassWallRight, Vector2{3840, 2752}, PropType::RIGHTSIDEWALL, Textures},
            Prop{Textures.GrassWallRight, Vector2{3840, 2688}, PropType::RIGHTSIDEWALL, Textures},
            Prop{Textures.GrassWallRight, Vector2{3840, 2624}, PropType::RIGHTSIDEWALL, Textures},
            Prop{Textures.GrassWallRight, Vector2{3840, 2560}, PropType::RIGHTSIDEWALL, Textures},
            Prop{Textures.GrassWallTopRight, Vector2{3840, 2496}, PropType::TOPWALL, Textures},

            // Top Side Walls
            Prop{Textures.GrassWallTop, Vector2{3776, 2496}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{3712, 2496}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{3648, 2496}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{3584, 2496}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{3520, 2496}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{3456, 2496}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{3392, 2496}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{3328, 2496}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{3264, 2496}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{3200, 2496}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{3136, 2496}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{3072, 2496}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{3008, 2496}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{2944, 2496}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{2880, 2496}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{2816, 2496}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{2752, 2496}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTopLeft, Vector2{2688, 2496}, PropType::TOPWALL, Textures},


            // ---------------------------------------- Right Side Second Hill -----------------------------------
            
            // Left Side Walls
            Prop{Textures.GrassWallLeft, Vector2{2752, 2624}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.GrassWallLeft, Vector2{2752, 2688}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.GrassWallLeft, Vector2{2752, 2752}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.GrassWallLeft, Vector2{2752, 2816}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.GrassWallLeft, Vector2{2752, 2880}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.GrassWallLeft, Vector2{2752, 2944}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.GrassWallLeft, Vector2{2752, 3008}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.GrassWallLeft, Vector2{2752, 3072}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.GrassWallLeft, Vector2{2752, 3136}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.GrassWallLeft, Vector2{2752, 3200}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.GrassWallBotLeft, Vector2{2752, 3264}, PropType::BOTTOMWALL, Textures},

            // Bottom Walls
            Prop{Textures.GrassWallBottom, Vector2{2816, 3264}, PropType::BOTTOMWALL, Textures},
            Prop{Textures.GrassWallBottom, Vector2{2880, 3264}, PropType::BOTTOMWALL, Textures},
            Prop{Textures.GrassWallBottom, Vector2{2944, 3264}, PropType::BOTTOMWALL, Textures},
            Prop{Textures.GrassWallBottom, Vector2{3136, 3264}, PropType::BOTTOMWALL, Textures},
            Prop{Textures.GrassWallBottom, Vector2{3200, 3264}, PropType::BOTTOMWALL, Textures},
            Prop{Textures.GrassWallBottom, Vector2{3264, 3264}, PropType::BOTTOMWALL, Textures},
            Prop{Textures.GrassWallBottom, Vector2{3328, 3264}, PropType::BOTTOMWALL, Textures},
            Prop{Textures.GrassWallBottom, Vector2{3392, 3264}, PropType::BOTTOMWALL, Textures},
            Prop{Textures.GrassWallBottom, Vector2{3456, 3264}, PropType::BOTTOMWALL, Textures},
            Prop{Textures.GrassWallBottom, Vector2{3520, 3264}, PropType::BOTTOMWALL, Textures},
            Prop{Textures.GrassWallBottom, Vector2{3584, 3264}, PropType::BOTTOMWALL, Textures},
            Prop{Textures.GrassWallBottom, Vector2{3648, 3264}, PropType::BOTTOMWALL, Textures},
            Prop{Textures.GrassWallBotRight, Vector2{3712, 3264}, PropType::BOTTOMWALL, Textures},

            // Right Side Walls
            Prop{Textures.GrassWallRight, Vector2{3712, 3200}, PropType::RIGHTSIDEWALL, Textures},
            Prop{Textures.GrassWallRight, Vector2{3712, 3136}, PropType::RIGHTSIDEWALL, Textures},
            Prop{Textures.GrassWallRight, Vector2{3712, 3072}, PropType::RIGHTSIDEWALL, Textures},
            Prop{Textures.GrassWallRight, Vector2{3712, 3008}, PropType::RIGHTSIDEWALL, Textures},
            Prop{Textures.GrassWallRight, Vector2{3712, 2944}, PropType::RIGHTSIDEWALL, Textures},
            Prop{Textures.GrassWallRight, Vector2{3712, 2880}, PropType::RIGHTSIDEWALL, Textures},
            Prop{Textures.GrassWallRight, Vector2{3712, 2816}, PropType::RIGHTSIDEWALL, Textures},
            Prop{Textures.GrassWallRight, Vector2{3712, 2752}, PropType::RIGHTSIDEWALL, Textures},
            Prop{Textures.GrassWallRight, Vector2{3712, 2688}, PropType::RIGHTSIDEWALL, Textures},
            Prop{Textures.GrassWallRight, Vector2{3712, 2624}, PropType::RIGHTSIDEWALL, Textures},
            Prop{Textures.GrassWallTopRight, Vector2{3712, 2560}, PropType::TOPWALL, Textures},

            // Top Walls
            Prop{Textures.GrassWallTop, Vector2{3648, 2560}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{3584, 2560}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{3520, 2560}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{3456, 2560}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{3392, 2560}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{3328, 2560}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{3264, 2560}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{3200, 2560}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{3136, 2560}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{3072, 2560}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{3008, 2560}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{2944, 2560}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{2880, 2560}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{2816, 2560}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTopLeft, Vector2{2752, 2560}, PropType::TOPWALL, Textures},

            // ---------------------------------------- Right Side Third Hill -----------------------------------

            // Left Side Walls
            Prop{Textures.WallLeft, Vector2{2944, 2688}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.WallLeft, Vector2{2944, 2752}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.WallLeft, Vector2{2944, 2816}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.WallLeft, Vector2{2944, 2880}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.WallLeft, Vector2{2944, 2944}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.WallLeft, Vector2{2944, 3008}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.WallBotLeft, Vector2{2944, 3072}, PropType::BOTTOMWALL, Textures},

            // Bottom Walls
            Prop{Textures.WallBottom, Vector2{3008, 3072}, PropType::BOTTOMWALL, Textures},
            Prop{Textures.WallBottom, Vector2{3136, 3072}, PropType::BOTTOMWALL, Textures},
            Prop{Textures.WallBottom, Vector2{3200, 3072}, PropType::BOTTOMWALL, Textures},
            Prop{Textures.WallBottom, Vector2{3264, 3072}, PropType::BOTTOMWALL, Textures},
            Prop{Textures.WallBottom, Vector2{3328, 3072}, PropType::BOTTOMWALL, Textures},
            Prop{Textures.WallBottom, Vector2{3456, 3072}, PropType::BOTTOMWALL, Textures},
            Prop{Textures.WallBotRight, Vector2{3520, 3072}, PropType::BOTTOMWALL, Textures},

            // Right Side Walls
            Prop{Textures.WallRight, Vector2{3520, 3008}, PropType::RIGHTSIDEWALL, Textures},
            Prop{Textures.WallRight, Vector2{3520, 2944}, PropType::RIGHTSIDEWALL, Textures},
            Prop{Textures.WallRight, Vector2{3520, 2880}, PropType::RIGHTSIDEWALL, Textures},
            Prop{Textures.WallRight, Vector2{3520, 2816}, PropType::RIGHTSIDEWALL, Textures},
            Prop{Textures.WallRight, Vector2{3520, 2752}, PropType::RIGHTSIDEWALL, Textures},
            Prop{Textures.WallRight, Vector2{3520, 2688}, PropType::RIGHTSIDEWALL, Textures},
            Prop{Textures.WallTopRight, Vector2{3520, 2624}, PropType::TOPWALL, Textures},

            // Bottom Walls
            Prop{Textures.GrassWallTop, Vector2{3456, 2624}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{3392, 2624}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{3328, 2624}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{3264, 2624}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{3200, 2624}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{3136, 2624}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{3072, 2624}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{3008, 2624}, PropType::TOPWALL, Textures},
            Prop{Textures.WallTopLeft, Vector2{2944, 2624}, PropType::TOPWALL, Textures},

            // Door
            Prop{Textures.DoorRed, Vector2{1612,2815}, PropType::DOOR, Textures, 4.f, false, true}, 
            Prop{Textures.DoorBlue, Vector2{3120,2685}, PropType::DOOR, Textures, 4.f, false, true}
        };
        Props.emplace_back(Walls);

        std::vector<Prop> Moveable
        {
            Prop{Textures.TreeStump, Vector2{3525,3640}, PropType::STUMP, Textures, 4.f, true},
            Prop{Textures.Boulder, Vector2{1025,3050}, PropType::BOULDER, Textures, 4.f, true},
        };
        Props.emplace_back(Moveable);
        
        return Props;
    }

    std::vector<std::vector<Prop>> InitializePropsOver(GameTexture& Textures)
    {
        std::vector<std::vector<Prop>> Props{};

        std::vector<Prop> Houses
        {
            // Red House
            Prop{Textures.HouseRed, Vector2{1472,2688}, PropType::rHOUSELEFT, Textures},   
            Prop{Textures.HouseRed, Vector2{1472,2688}, PropType::rHOUSERIGHT, Textures},

            // Blue House   
            Prop{Textures.HouseBlue, Vector2{3073,2495}, PropType::bHOUSELEFT, Textures},   
            Prop{Textures.HouseBlue, Vector2{3073,2495}, PropType::bHOUSERIGHT, Textures},

        };
        Props.emplace_back(Houses);

        std::vector<Prop> Boulder
        {
            // Prop{Textures.Boulder, Vector2{1000,1000}, PropType::BOULDER}, 
            // Prop{Textures.Boulder, Vector2{1112,1000}, PropType::BOULDER},
            // Prop{Textures.Boulder, Vector2{1172,1000}, PropType::BOULDER, 4.f, true},
            // Prop{Textures.Boulder, Vector2{750,900}, PropType::BOULDER, 4.f, true},
            
            Prop{Textures.Boulder, Vector2{960,2950}, PropType::BOULDER, Textures},
            Prop{Textures.Boulder, Vector2{1090,2950}, PropType::BOULDER, Textures},
            Prop{Textures.Boulder, Vector2{940,3050}, PropType::BOULDER, Textures},
            Prop{Textures.Boulder, Vector2{1110,3050}, PropType::BOULDER, Textures},

        };
        Props.emplace_back(Boulder);

        std::vector<Prop> Moveable
        {
            Prop{Textures.TreeStump, Vector2{3525,3640}, PropType::STUMP, Textures, 4.f, true},
            Prop{Textures.Boulder, Vector2{1025,3050}, PropType::BOULDER, Textures, 4.f, true},
        };
        Props.emplace_back(Moveable);

        std::vector<Prop> Grass
        {
            // --------------------------------------------- Far Left Column -----------------------------------------------

            // --------------------------------------------------- -4 -------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1781,1826}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1781,1866}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1845,1826}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1845,1866}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1781,1890}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1781,1930}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1845,1890}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1845,1930}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            // --------------------------------------------------- -3 -------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1781,1698}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1781,1738}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1845,1698}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1845,1738}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1781,1762}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1781,1802}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1845,1762}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1845,1802}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            // --------------------------------------------------- -2 -------------------------------------------------------
            // Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1781,1570}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1781,1610}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            // Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1845,1570}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1845,1610}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1781,1634}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1781,1674}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1845,1634}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1845,1674}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            // ---------------------------------------------------- -1 ------------------------------------------------------
            // Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1909,1314}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1909,1354}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            // Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1973,1314}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1973,1354}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            // Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1909,1378}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1909,1418}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1973,1378}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1973,1418}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            // --------------------------------------------------- 0 -------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1909,1442}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1909,1482}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1973,1442}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1973,1482}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            // Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1909,1506}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1909,1546}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1973,1506}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1973,1546}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            // --------------------------------------------------- 1 -------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1909,1570}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1909,1610}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1973,1570}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1973,1610}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1909,1634}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1909,1674}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1973,1634}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1973,1674}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            // --------------------------------------------------- 2 -------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1909,1698}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1909,1738}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1973,1698}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1973,1738}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1909,1762}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1909,1802}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1973,1762}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1973,1802}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            // --------------------------------------------------- 3 -------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1909,1826}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1909,1866}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1973,1826}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1973,1866}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1909,1890}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1909,1930}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1973,1890}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1973,1930}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            // ------------------------------------------------ Left Column ------------------------------------------------

            // ---------------------------------------------------- -1 ------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2037,1314}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2037,1354}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2101,1314}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2101,1354}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2037,1378}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2037,1418}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2101,1378}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2101,1418}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            // ---------------------------------------------------- 0 ------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2037,1442}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2037,1482}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2101,1442}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2101,1482}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2037,1506}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2037,1546}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2101,1506}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2101,1546}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            // --------------------------------------------------- 1 -------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2037,1570}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2037,1610}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2101,1570}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2101,1610}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2037,1634}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2037,1674}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2101,1634}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2101,1674}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            // --------------------------------------------------- 2 -------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2037,1698}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2037,1738}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2101,1698}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2101,1738}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2037,1762}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2037,1802}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2101,1762}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2101,1802}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            // --------------------------------------------------- 3 -------------------------------------------------------        
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2037,1826}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2037,1866}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2101,1826}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2101,1866}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2037,1890}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2037,1930}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2101,1890}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2101,1930}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            // --------------------------------------------------- 4 ------------------------------------------------------- 
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2037,1954}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2037,1994}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2101,1954}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2101,1994}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2037,2018}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2037,2058}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2101,2018}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2101,2058}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            // --------------------------------------------- Far Right Column ----------------------------------------------

            // --------------------------------------------------- -6 -------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2421,1954}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2421,1994}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2485,1954}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2485,1994}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2421,2018}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2421,2058}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2485,2018}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2485,2058}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            // --------------------------------------------------- -5 -------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2421,1826}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2421,1866}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            // Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2485,1826}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2485,1866}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2421,1890}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2421,1930}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2485,1890}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2485,1930}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            // --------------------------------------------------- -4 -------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2421,1698}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2421,1738}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2485,1698}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2485,1738}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2421,1762}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2421,1802}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            // Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2485,1762}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2485,1802}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            // --------------------------------------------------- -3 -------------------------------------------------------
            // Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2421,1570}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2421,1610}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            // Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2485,1570}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2485,1610}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2421,1634}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2421,1674}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            // Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2485,1634}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2485,1674}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            // ---------------------------------------------------- -2 ------------------------------------------------------
            // Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2421,1314}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2421,1354}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            // Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2485,1314}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2485,1354}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            // Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2421,1378}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2421,1418}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            // Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2485,1378}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2485,1418}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            // ---------------------------------------------------- -1 ------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2293,1314}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2293,1354}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2357,1314}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2357,1354}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2293,1378}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2293,1418}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2357,1378}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2357,1418}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            // ---------------------------------------------------- 0 ------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2293,1442}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2293,1482}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2357,1442}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2357,1482}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2293,1506}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2293,1546}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2357,1506}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2357,1546}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            // --------------------------------------------------- 1 -------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2293,1570}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2293,1610}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2357,1570}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2357,1610}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2293,1634}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2293,1674}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2357,1634}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2357,1674}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            // --------------------------------------------------- 2 -------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2293,1698}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2293,1738}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2357,1698}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2357,1738}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2293,1762}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2293,1802}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2357,1762}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2357,1802}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            // --------------------------------------------------- 3 -------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2293,1826}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2293,1866}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2357,1826}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2357,1866}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2293,1890}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2293,1930}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2357,1890}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2357,1930}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            // --------------------------------------------------- 4 -------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2293,1954}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2293,1994}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2357,1954}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2357,1994}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            // Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2293,2018}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2293,2058}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2357,2018}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2357,2058}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            // ------------------------------------------------ Right Column -----------------------------------------------

            // --------------------------------------------------- -1 -------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2165,1314}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2165,1354}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2229,1314}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2229,1354}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2165,1378}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2165,1418}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2229,1378}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2229,1418}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            // --------------------------------------------------- 0 -------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2165,1442}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2165,1482}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2229,1442}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2229,1482}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2165,1506}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2165,1546}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2229,1506}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2229,1546}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            // --------------------------------------------------- 1 -------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2165,1570}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2165,1610}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2229,1570}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2229,1610}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2165,1634}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2165,1674}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2229,1634}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2229,1674}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            // --------------------------------------------------- 2 -------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2165,1698}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2165,1738}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2229,1698}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2229,1738}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2165,1762}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2165,1802}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2229,1762}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2229,1802}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            // --------------------------------------------------- 3 -------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2165,1826}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2165,1866}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2229,1826}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2229,1866}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2165,1890}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2165,1930}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2229,1890}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2229,1930}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            // --------------------------------------------------- 4 -------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2165,1954}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2165,1994}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2229,1954}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2229,1994}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2165,2018}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2165,2058}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2229,2018}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2229,2058}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            // --------------------------------------------------------------------------------------------------------------
            // -----------------------------------------Dungeon Entrance Grass-----------------------------------------------
            // --------------------------------------------------------------------------------------------------------------

            // ------------------------------------------------ Left Column -------------------------------------------------
            // Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3465,150}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3465,190}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529,150}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529,190}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            // Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3465,214}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3465,254}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529,214}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529,254}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            // --------------------------------------------------------------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529,278}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529,318}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529,342}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529,382}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------------------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529,406}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529,446}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529,470}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529,510}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------------------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529,534}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529,574}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529,598}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529,638}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------------------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529,662}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529,702}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529,726}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529,766}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------------------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529,790}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529,830}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529,854}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529,894}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------------------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529,918}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529,958}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529,982}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529,1022}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------------------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529,1046}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529,1086}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529,1110}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529,1150}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------------------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529,1174}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529,1214}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529,1238}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529,1278}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------------------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529,1302}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529,1342}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529,1366}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529,1406}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------------------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529,1430}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529,1470}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529,1494}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529,1534}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------------------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529,1558}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529,1598}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529,1622}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529,1662}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------------------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529,1686}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529,1726}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529,1750}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529,1790}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------------------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529,1814}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529,1854}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529,1878}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529,1918}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            // ------------------------------------------------ Right Column -------------------------------------------------

            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593,150}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593,190}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657,150}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657,190}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593,214}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593,254}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657,214}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657,254}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            // --------------------------------------------------------------------------------------------------------------

            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593,278}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593,318}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657,278}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657,318}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593,342}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593,382}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657,342}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657,382}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------------------------------------------------------------------

            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593,406}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593,446}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657,406}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657,446}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593,470}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593,510}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657,470}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657,510}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------------------------------------------------------------------

            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593,534}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593,574}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657,534}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657,574}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593,598}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593,638}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657,598}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657,638}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------------------------------------------------------------------

            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593,662}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593,702}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657,662}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657,702}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593,726}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593,766}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657,726}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657,766}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------------------------------------------------------------------

            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593,790}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593,830}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657,790}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657,830}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593,854}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593,894}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657,854}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657,894}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------------------------------------------------------------------
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593,918}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593,958}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657,918}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657,958}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593,982}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593,1022}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657,982}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657,1022}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------------------------------------------------------------------

            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593,1046}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593,1086}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657,1046}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657,1086}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593,1110}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593,1150}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657,1110}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657,1150}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------------------------------------------------------------------

            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593,1174}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593,1214}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657,1174}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657,1214}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593,1238}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593,1278}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657,1238}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657,1278}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------------------------------------------------------------------

            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593,1302}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593,1342}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657,1302}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657,1342}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593,1366}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593,1406}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657,1366}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657,1406}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------------------------------------------------------------------

            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593,1430}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593,1470}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657,1430}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657,1470}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593,1494}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593,1534}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657,1494}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657,1534}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------------------------------------------------------------------

            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593,1558}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593,1598}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657,1558}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657,1598}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593,1622}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593,1662}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657,1622}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657,1662}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------------------------------------------------------------------

            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593,1686}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593,1726}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657,1686}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657,1726}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593,1750}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593,1790}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657,1750}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657,1790}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------------------------------------------------------------------

            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593,1814}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593,1854}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657,1814}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657,1854}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},

            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593,1878}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593,1918}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657,1878}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657,1918}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},


        };
        Props.emplace_back(Grass);

        std::vector<Prop> Treasure
        {
            Prop{Sprite{Textures.TreasureChestBig, 4, 1, 1.f/4.f}, Vector2{2198,1677}, PropType::BIGTREASURE, Textures, Textures.TreasureHeart,  4.f, false, true, Progress::ACT_O, PropType::NPC_O, "Life Stone", 4.f},
            Prop{Sprite{Textures.TreasureChest, 4, 1, 1.f/4.f}, Vector2{2250,3350}, PropType::TREASURE, Textures, Textures.AltarTopRight,  4.f, false, true, Progress::ACT_O, PropType::NPC_O, "Top Right Altar Piece"},
            Prop{Sprite{Textures.TreasureChest, 4, 1, 1.f/4.f}, Vector2{2250,3150}, PropType::TREASURE, Textures, Textures.AltarTop,  4.f, false, true, Progress::ACT_O, PropType::NPC_O, "Top Altar Piece"},
            Prop{Sprite{Textures.TreasureChest, 4, 1, 1.f/4.f}, Vector2{130,210}, PropType::TREASURE, Textures, Textures.AltarTopLeft,  4.f, false, true, Progress::ACT_O, PropType::NPC_O, "Top Left Altar Piece"},
            Prop{Sprite{Textures.TreasureChest, 4, 1, 1.f/4.f}, Vector2{1025,2765}, PropType::TREASURE, Textures, Textures.AltarBotLeft,  4.f, false, true, Progress::ACT_O, PropType::NPC_O, "Bottom Left Altar Piece"},
            Prop{Sprite{Textures.TreasureChest, 4, 1, 1.f/4.f}, Vector2{2250,2750}, PropType::TREASURE, Textures, Textures.AltarBot,  4.f, false, true, Progress::ACT_O, PropType::NPC_O, "Bottom Altar Piece"},
            Prop{Sprite{Textures.TreasureChest, 4, 1, 1.f/4.f}, Vector2{2250,2550}, PropType::TREASURE, Textures, Textures.AltarTopRight,  4.f, false, true, Progress::ACT_O, PropType::NPC_O, "Bottom Right Altar Piece"},
        };
        Props.emplace_back(Treasure);

        std::vector<Prop> NPCS
        {
            Prop{Sprite{Textures.Didi, 4, 1, 1.f/8.f}, Vector2{3163, 2853}, PropType::NPC_A, Textures, Textures.Placeholder, 3.f, false, true},
            Prop{Sprite{Textures.Jade, 4, 1, 1.f/8.f}, Vector2{1546, 2959}, PropType::NPC_B, Textures, Textures.Placeholder, 3.f, false, true},
            // Prop{Sprite{Textures.Didi, 4, 1, 1.f/8.f}, Vector2{2492, 3634}, PropType::NPC_A, Textures, 3.f, false, true},
            // Prop{Sprite{"sprites/npc/Jade.png", 4, 1, 1.f/8.f}, Vector2{2044, 3634}, PropType::NPC_B, Textures, 3.f, false, true},
            Prop{Sprite{Textures.Son, 4, 1, 1.f/8.f}, Vector2{1240, 2036}, PropType::NPC_C, Textures, Textures.Placeholder, 3.f, false, true, Progress::ACT_II, PropType::NPC_B},
        };
        Props.emplace_back(NPCS);

        std::vector<Prop> AnimatedAltarPieces
        {
            Prop{Sprite{Textures.AltarTopLeftAnimated, 14, 1, 1/14.f}, Vector2{700,1025}, PropType::ANIMATEDALTAR, Textures, "Top Left Altar Piece", true, true},
            Prop{Sprite{Textures.AltarTopAnimated, 14, 1, 1/14.f}, Vector2{764,1025}, PropType::ANIMATEDALTAR, Textures, "Top Altar Piece", true, true},
            Prop{Sprite{Textures.AltarTopRightAnimated, 14, 1, 1/14.f}, Vector2{828,1025}, PropType::ANIMATEDALTAR, Textures, "Top Right Altar Piece", true, true},
            Prop{Sprite{Textures.AltarBotLeftAnimated, 14, 1, 1/14.f}, Vector2{700,1089}, PropType::ANIMATEDALTAR, Textures, "Bottom Left Altar Piece", true, true},
            Prop{Sprite{Textures.AltarBotAnimated, 14, 1, 1/14.f}, Vector2{764,1089}, PropType::ANIMATEDALTAR, Textures, "Bottom Altar Piece", true, true},
            Prop{Sprite{Textures.AltarBotRightAnimated, 14, 1, 1/14.f}, Vector2{828,1089}, PropType::ANIMATEDALTAR, Textures, "Bottom Right Altar Piece", true, true},
        };
        Props.emplace_back(AnimatedAltarPieces);
        
        return Props;
    }

    std::vector<Prop> InitializeTrees(GameTexture& Textures)
    {
        return std::vector<Prop> {

            // row1
            Prop{Textures.TreeGreen, Vector2{20,67}, PropType::TREE, Textures},        // 1
            Prop{Textures.TreeGreen, Vector2{120,67}, PropType::TREE, Textures},       // 2
            Prop{Textures.TreeGreen, Vector2{220,67}, PropType::TREE, Textures},       // 3
            Prop{Textures.TreeGreen, Vector2{320,67}, PropType::TREE, Textures},       // 4
            Prop{Textures.TreeGreen, Vector2{420,67}, PropType::TREE, Textures},       // 5
            Prop{Textures.TreeGreen, Vector2{520,67}, PropType::TREE, Textures},       // 6
            Prop{Textures.TreeGreen, Vector2{620,67}, PropType::TREE, Textures},       // 7
            Prop{Textures.TreeGreen, Vector2{720,67}, PropType::TREE, Textures},       // 8
            Prop{Textures.TreeGreen, Vector2{820,67}, PropType::TREE, Textures},       // 9
            Prop{Textures.TreeGreen, Vector2{920,67}, PropType::TREE, Textures},       // 10
            Prop{Textures.TreeGreen, Vector2{1020,67}, PropType::TREE, Textures},      // 11
            Prop{Textures.TreeGreen, Vector2{1120,67}, PropType::TREE, Textures},      // 12
            Prop{Textures.TreeGreen, Vector2{1220,67}, PropType::TREE, Textures},      // 13
            Prop{Textures.TreeGreen, Vector2{1320,67}, PropType::TREE, Textures},      // 14
            Prop{Textures.TreeGreen, Vector2{1420,67}, PropType::TREE, Textures},      // 15
            Prop{Textures.TreeGreen, Vector2{1520,67}, PropType::TREE, Textures},      // 16
            Prop{Textures.TreeGreen, Vector2{1620,67}, PropType::TREE, Textures},      // 17
            Prop{Textures.TreeGreen, Vector2{1720,67}, PropType::TREE, Textures},      // 18
            Prop{Textures.TreeGreen, Vector2{1820,67}, PropType::TREE, Textures},      // 19
            Prop{Textures.TreeGreen, Vector2{1920,67}, PropType::TREE, Textures},      // 20
            Prop{Textures.TreeGreen, Vector2{2020,67}, PropType::TREE, Textures},      // 21
            Prop{Textures.TreeGreen, Vector2{2120,67}, PropType::TREE, Textures},      // 22
            Prop{Textures.TreeGreen, Vector2{2220,67}, PropType::TREE, Textures},      // 23
            Prop{Textures.TreeGreen, Vector2{2320,67}, PropType::TREE, Textures},      // 24
            Prop{Textures.TreeGreen, Vector2{2420,67}, PropType::TREE, Textures},      // 25
            Prop{Textures.TreeGreen, Vector2{2520,67}, PropType::TREE, Textures},      // 26
            Prop{Textures.TreeGreen, Vector2{2620,67}, PropType::TREE, Textures},      // 27
            Prop{Textures.TreeGreen, Vector2{2720,67}, PropType::TREE, Textures},      // 28
            Prop{Textures.TreeGreen, Vector2{2820,67}, PropType::TREE, Textures},      // 29
            Prop{Textures.TreeGreen, Vector2{2920,67}, PropType::TREE, Textures},      // 30
            Prop{Textures.TreeGreen, Vector2{3020,67}, PropType::TREE, Textures},      // 31
            Prop{Textures.TreeGreen, Vector2{3120,67}, PropType::TREE, Textures},      // 32
            Prop{Textures.TreeGreen, Vector2{3220,67}, PropType::TREE, Textures},      // 33
            Prop{Textures.TreeGreen, Vector2{3320,67}, PropType::TREE, Textures},      // 34
            Prop{Textures.TreeGreen, Vector2{3420,67}, PropType::TREE, Textures},      // 35

            Prop{Textures.TreeGreen, Vector2{3700,67}, PropType::TREE, Textures},      // 36
            Prop{Textures.TreeGreen, Vector2{3800,67}, PropType::TREE, Textures},      // 37
            Prop{Textures.TreeGreen, Vector2{3900,67}, PropType::TREE, Textures},      // 38
            Prop{Textures.TreeGreen, Vector2{4000,67}, PropType::TREE, Textures},      // 39

            // row2
            Prop{Textures.TreeGreen, Vector2{0,167}, PropType::TREE, Textures},        // 1
            // Prop{Textures.TreeGreen, Vector2{100,167}, PropType::TREE, Textures},   // 2
            Prop{Textures.TreeGreen, Vector2{200,167}, PropType::TREE, Textures},      // 3
            Prop{Textures.TreeGreen, Vector2{300,167}, PropType::TREE, Textures},      // 4
            Prop{Textures.TreeGreen, Vector2{400,167}, PropType::TREE, Textures},      // 5
            Prop{Textures.TreeGreen, Vector2{500,167}, PropType::TREE, Textures},      // 6
            // Prop{Textures.TreeGreen, Vector2{600,167}, PropType::TREE, Textures},   // 7
            Prop{Textures.TreeGreen, Vector2{700,167}, PropType::TREE, Textures},      // 8
            Prop{Textures.TreeGreen, Vector2{800,167}, PropType::TREE, Textures},      // 9
            Prop{Textures.TreeGreen, Vector2{900,167}, PropType::TREE, Textures},      // 10
            Prop{Textures.TreeGreen, Vector2{1000,167}, PropType::TREE, Textures},     // 11
            // Prop{Textures.TreeGreen, Vector2{1100,167}, PropType::TREE, Textures},     // 12
            // Prop{Textures.TreeGreen, Vector2{1200,167}, PropType::TREE, Textures},     // 13
            // Prop{Textures.TreeGreen, Vector2{1300,167}, PropType::TREE, Textures},     // 14
            Prop{Textures.TreeGreen, Vector2{1400,167}, PropType::TREE, Textures},     // 15
            Prop{Textures.TreeGreen, Vector2{1500,167}, PropType::TREE, Textures},     // 16
            Prop{Textures.TreeGreen, Vector2{1600,167}, PropType::TREE, Textures},     // 17
            // Prop{Textures.TreeGreen, Vector2{1700,167}, PropType::TREE, Textures},  // 18
            // Prop{Textures.TreeGreen, Vector2{1800,167}, PropType::TREE, Textures},  // 19
            Prop{Textures.TreeGreen, Vector2{1900,167}, PropType::TREE, Textures},     // 20
            Prop{Textures.TreeGreen, Vector2{2000,167}, PropType::TREE, Textures},     // 21
            Prop{Textures.TreeGreen, Vector2{2100,167}, PropType::TREE, Textures},     // 22
            Prop{Textures.TreeGreen, Vector2{2200,167}, PropType::TREE, Textures},     // 23
            // Prop{Textures.TreeGreen, Vector2{2300,167}, PropType::TREE, Textures},  // 24
            // Prop{Textures.TreeGreen, Vector2{2400,167}, PropType::TREE, Textures},  // 25
            Prop{Textures.TreeGreen, Vector2{2500,167}, PropType::TREE, Textures},     // 26
            Prop{Textures.TreeGreen, Vector2{2600,167}, PropType::TREE, Textures},     // 27
            Prop{Textures.TreeGreen, Vector2{2700,167}, PropType::TREE, Textures},     // 28
            Prop{Textures.TreeGreen, Vector2{2800,167}, PropType::TREE, Textures},     // 29
            // Prop{Textures.TreeGreen, Vector2{2900,167}, PropType::TREE, Textures},  // 30
            Prop{Textures.TreeGreen, Vector2{3000,167}, PropType::TREE, Textures},     // 31
            Prop{Textures.TreeGreen, Vector2{3100,167}, PropType::TREE, Textures},     // 32
            Prop{Textures.TreeGreen, Vector2{3200,167}, PropType::TREE, Textures},     // 33
            Prop{Textures.TreeGreen, Vector2{3300,167}, PropType::TREE, Textures},     // 34
            Prop{Textures.TreeGreen, Vector2{3400,167}, PropType::TREE, Textures},     // 35

            Prop{Textures.TreeGreen, Vector2{3720,167}, PropType::TREE, Textures},     // 36
            // Prop{Textures.TreeGreen, Vector2{3820,167}, PropType::TREE, Textures},  // 37
            Prop{Textures.TreeGreen, Vector2{3920,167}, PropType::TREE, Textures},     // 38

            // row3
            Prop{Textures.TreeGreen, Vector2{20,267}, PropType::TREE, Textures},       // 1 
            Prop{Textures.TreeGreen, Vector2{120,267}, PropType::TREE, Textures},      // 2 
            Prop{Textures.TreeGreen, Vector2{220,267}, PropType::TREE, Textures},      // 3 
            Prop{Textures.TreeGreen, Vector2{320,267}, PropType::TREE, Textures},      // 4 
            Prop{Textures.TreeGreen, Vector2{420,267}, PropType::TREE, Textures},      // 5 
            // Prop{Textures.TreeGreen, Vector2{520,267}, PropType::TREE, Textures},   // 6     
            Prop{Textures.TreeGreen, Vector2{620,267}, PropType::TREE, Textures},      // 7 
            Prop{Textures.TreeGreen, Vector2{720,267}, PropType::TREE, Textures},      // 8 
            Prop{Textures.TreeGreen, Vector2{820,267}, PropType::TREE, Textures},      // 9 
            Prop{Textures.TreeGreen, Vector2{920,267}, PropType::TREE, Textures},      // 10
            // Prop{Textures.TreeGreen, Vector2{1020,267}, PropType::TREE, Textures},  // 11
            // Prop{Textures.TreeGreen, Vector2{1120,267}, PropType::TREE, Textures},     // 12
            // Prop{Textures.TreeGreen, Vector2{1220,267}, PropType::TREE, Textures},     // 13
            // Prop{Textures.TreeGreen, Vector2{1320,267}, PropType::TREE, Textures},     // 14
            // Prop{Textures.TreeGreen, Vector2{1420,267}, PropType::TREE, Textures},  // 15
            Prop{Textures.TreeGreen, Vector2{1520,267}, PropType::TREE, Textures},     // 16
            Prop{Textures.TreeGreen, Vector2{1620,267}, PropType::TREE, Textures},     // 17
            Prop{Textures.TreeGreen, Vector2{1720,267}, PropType::TREE, Textures},     // 18
            Prop{Textures.TreeGreen, Vector2{1820,267}, PropType::TREE, Textures},     // 19
            Prop{Textures.TreeGreen, Vector2{1920,267}, PropType::TREE, Textures},     // 20
            Prop{Textures.TreeGreen, Vector2{2020,267}, PropType::TREE, Textures},     // 21
            // Prop{Textures.TreeGreen, Vector2{2120,267}, PropType::TREE, Textures},  // 22
            Prop{Textures.TreeGreen, Vector2{2220,267}, PropType::TREE, Textures},     // 23
            Prop{Textures.TreeGreen, Vector2{2320,267}, PropType::TREE, Textures},     // 24
            Prop{Textures.TreeGreen, Vector2{2420,267}, PropType::TREE, Textures},     // 25
            Prop{Textures.TreeGreen, Vector2{2520,267}, PropType::TREE, Textures},     // 26
            Prop{Textures.TreeGreen, Vector2{2620,267}, PropType::TREE, Textures},     // 27
            // Prop{Textures.TreeGreen, Vector2{2720,267}, PropType::TREE, Textures},  // 28
            Prop{Textures.TreeGreen, Vector2{2820,267}, PropType::TREE, Textures},     // 29
            Prop{Textures.TreeGreen, Vector2{2920,267}, PropType::TREE, Textures},     // 30
            Prop{Textures.TreeGreen, Vector2{3020,267}, PropType::TREE, Textures},     // 31
            Prop{Textures.TreeGreen, Vector2{3120,267}, PropType::TREE, Textures},     // 32
            // Prop{Textures.TreeGreen, Vector2{3220,267}, PropType::TREE, Textures},  // 33
            Prop{Textures.TreeGreen, Vector2{3320,267}, PropType::TREE, Textures},     // 34
            Prop{Textures.TreeGreen, Vector2{3420,267}, PropType::TREE, Textures},     // 35

            Prop{Textures.TreeGreen, Vector2{3700,267}, PropType::TREE, Textures},     // 36
            Prop{Textures.TreeGreen, Vector2{3800,267}, PropType::TREE, Textures},     // 37
            Prop{Textures.TreeGreen, Vector2{3900,267}, PropType::TREE, Textures},     // 38
            Prop{Textures.TreeGreen, Vector2{4000,267}, PropType::TREE, Textures},     // 39

            // row4
            Prop{Textures.TreeGreen, Vector2{0,367}, PropType::TREE, Textures},        // 1 
            Prop{Textures.TreeGreen, Vector2{100,367}, PropType::TREE, Textures},      // 2 
            Prop{Textures.TreeGreen, Vector2{200,367}, PropType::TREE, Textures},      // 3 
            // Prop{Textures.TreeGreen, Vector2{300,367}, PropType::TREE, Textures},   // 4     
            Prop{Textures.TreeGreen, Vector2{400,367}, PropType::TREE, Textures},      // 5 
            Prop{Textures.TreeGreen, Vector2{500,367}, PropType::TREE, Textures},      // 6 
            // Prop{Textures.TreeGreen, Vector2{600,367}, PropType::TREE, Textures},   // 7 
            Prop{Textures.TreeGreen, Vector2{700,367}, PropType::TREE, Textures},      // 8 
            Prop{Textures.TreeGreen, Vector2{800,367}, PropType::TREE, Textures},      // 9 
            Prop{Textures.TreeGreen, Vector2{900,367}, PropType::TREE, Textures},      // 10
            // Prop{Textures.TreeGreen, Vector2{1000,367}, PropType::TREE, Textures},     // 11
            // Prop{Textures.TreeGreen, Vector2{1100,367}, PropType::TREE, Textures},     // 12
            // Prop{Textures.TreeGreen, Vector2{1200,367}, PropType::TREE, Textures},     // 13
            // Prop{Textures.TreeGreen, Vector2{1300,367}, PropType::TREE, Textures},     // 14
            // Prop{Textures.TreeGreen, Vector2{1400,367}, PropType::TREE, Textures},     // 15
            Prop{Textures.TreeGreen, Vector2{1500,367}, PropType::TREE, Textures},     // 16
            Prop{Textures.TreeGreen, Vector2{1600,367}, PropType::TREE, Textures},     // 17
            // Prop{Textures.TreeGreen, Vector2{1700,367}, PropType::TREE, Textures},  // 18
            Prop{Textures.TreeGreen, Vector2{1800,367}, PropType::TREE, Textures},     // 19
            Prop{Textures.TreeGreen, Vector2{1900,367}, PropType::TREE, Textures},     // 20
            // Prop{Textures.TreeGreen, Vector2{2000,367}, PropType::TREE, Textures},     // 21
            // Prop{Textures.TreeGreen, Vector2{2100,367}, PropType::TREE, Textures},     // 22
            // Prop{Textures.TreeGreen, Vector2{2200,367}, PropType::TREE, Textures},     // 23
            // Prop{Textures.TreeGreen, Vector2{2300,367}, PropType::TREE, Textures},     // 24
            Prop{Textures.TreeGreen, Vector2{2400,367}, PropType::TREE, Textures},  // 25
            Prop{Textures.TreeGreen, Vector2{2500,367}, PropType::TREE, Textures},     // 26
            Prop{Textures.TreeGreen, Vector2{2600,367}, PropType::TREE, Textures},     // 27
            Prop{Textures.TreeGreen, Vector2{2700,367}, PropType::TREE, Textures},     // 28
            Prop{Textures.TreeGreen, Vector2{2800,367}, PropType::TREE, Textures},     // 29
            Prop{Textures.TreeGreen, Vector2{2900,367}, PropType::TREE, Textures},     // 30
            Prop{Textures.TreeGreen, Vector2{3000,367}, PropType::TREE, Textures},     // 31
            Prop{Textures.TreeGreen, Vector2{3100,367}, PropType::TREE, Textures},     // 32
            Prop{Textures.TreeGreen, Vector2{3200,367}, PropType::TREE, Textures},     // 33
            Prop{Textures.TreeGreen, Vector2{3300,367}, PropType::TREE, Textures},     // 34
            Prop{Textures.TreeGreen, Vector2{3400,367}, PropType::TREE, Textures},     // 35

            Prop{Textures.TreeGreen, Vector2{3720,367}, PropType::TREE, Textures},     // 36
            Prop{Textures.TreeGreen, Vector2{3820,367}, PropType::TREE, Textures},     // 37
            Prop{Textures.TreeGreen, Vector2{3920,367}, PropType::TREE, Textures},     // 38

            //row5
            Prop{Textures.TreeGreen, Vector2{20,467}, PropType::TREE, Textures},       // 1 
            Prop{Textures.TreeGreen, Vector2{120,467}, PropType::TREE, Textures},      // 2 
            // Prop{Textures.TreeGreen, Vector2{220,467}, PropType::TREE, Textures},   // 3 
            Prop{Textures.TreeGreen, Vector2{320,467}, PropType::TREE, Textures},      // 4 
            // Prop{Textures.TreeGreen, Vector2{420,467}, PropType::TREE, Textures},   // 5 
            Prop{Textures.TreeGreen, Vector2{520,467}, PropType::TREE, Textures},      // 6 
            Prop{Textures.TreeGreen, Vector2{620,467}, PropType::TREE, Textures},      // 7 
            Prop{Textures.TreeGreen, Vector2{720,467}, PropType::TREE, Textures},      // 8 
            // Prop{Textures.TreeGreen, Vector2{820,467}, PropType::TREE, Textures},   // 9 
            Prop{Textures.TreeGreen, Vector2{920,467}, PropType::TREE, Textures},      // 10
            Prop{Textures.TreeGreen, Vector2{1020,467}, PropType::TREE, Textures},     // 11
            // Prop{Textures.TreeGreen, Vector2{1120,467}, PropType::TREE, Textures},     // 12
            // Prop{Textures.TreeGreen, Vector2{1220,467}, PropType::TREE, Textures},     // 13
            // Prop{Textures.TreeGreen, Vector2{1320,467}, PropType::TREE, Textures},     // 14
            Prop{Textures.TreeGreen, Vector2{1420,467}, PropType::TREE, Textures},     // 15
            Prop{Textures.TreeGreen, Vector2{1520,467}, PropType::TREE, Textures},     // 16
            Prop{Textures.TreeGreen, Vector2{1620,467}, PropType::TREE, Textures},     // 17
            // Prop{Textures.TreeGreen, Vector2{1720,467}, PropType::TREE, Textures},  // 18
            Prop{Textures.TreeGreen, Vector2{1820,467}, PropType::TREE, Textures},     // 19
            // Prop{Textures.TreeGreen, Vector2{1920,467}, PropType::TREE, Textures},     // 20
            // Prop{Textures.TreeGreen, Vector2{2020,467}, PropType::TREE, Textures},     // 21
            // Prop{Textures.TreeGreen, Vector2{2120,467}, PropType::TREE, Textures},     // 22
            // Prop{Textures.TreeGreen, Vector2{2220,467}, PropType::TREE, Textures},     // 23
            // Prop{Textures.TreeGreen, Vector2{2320,467}, PropType::TREE, Textures},     // 24
            // Prop{Textures.TreeGreen, Vector2{2420,467}, PropType::TREE, Textures},  // 25
            Prop{Textures.TreeGreen, Vector2{2520,467}, PropType::TREE, Textures},     // 26
            Prop{Textures.TreeGreen, Vector2{2620,467}, PropType::TREE, Textures},     // 27
            Prop{Textures.TreeGreen, Vector2{2720,467}, PropType::TREE, Textures},     // 28
            Prop{Textures.TreeGreen, Vector2{2820,467}, PropType::TREE, Textures},     // 29
            Prop{Textures.TreeGreen, Vector2{2920,467}, PropType::TREE, Textures},     // 30
            // Prop{Textures.TreeGreen, Vector2{3020,467}, PropType::TREE, Textures},  // 31
            Prop{Textures.TreeGreen, Vector2{3120,467}, PropType::TREE, Textures},     // 32
            Prop{Textures.TreeGreen, Vector2{3220,467}, PropType::TREE, Textures},     // 33
            Prop{Textures.TreeGreen, Vector2{3320,467}, PropType::TREE, Textures},     // 34
            Prop{Textures.TreeGreen, Vector2{3420,467}, PropType::TREE, Textures},     // 35

            Prop{Textures.TreeGreen, Vector2{3700,467}, PropType::TREE, Textures},     // 36
            Prop{Textures.TreeGreen, Vector2{3800,467}, PropType::TREE, Textures},     // 37
            Prop{Textures.TreeGreen, Vector2{3900,467}, PropType::TREE, Textures},     // 38
            Prop{Textures.TreeGreen, Vector2{4000,467}, PropType::TREE, Textures},     // 39

            // row6
            Prop{Textures.TreeGreen, Vector2{0,567}, PropType::TREE, Textures},        // 1 
            Prop{Textures.TreeGreen, Vector2{100,567}, PropType::TREE, Textures},      // 2 
            Prop{Textures.TreeGreen, Vector2{200,567}, PropType::TREE, Textures},      // 3 
            Prop{Textures.TreeGreen, Vector2{300,567}, PropType::TREE, Textures},      // 4 
            Prop{Textures.TreeGreen, Vector2{400,567}, PropType::TREE, Textures},      // 5 
            Prop{Textures.TreeGreen, Vector2{500,567}, PropType::TREE, Textures},      // 6 
            Prop{Textures.TreeGreen, Vector2{600,567}, PropType::TREE, Textures},      // 7 
            // Prop{Textures.TreeGreen, Vector2{700,567}, PropType::TREE, Textures},   // 8 
            Prop{Textures.TreeGreen, Vector2{800,567}, PropType::TREE, Textures},      // 9 
            Prop{Textures.TreeGreen, Vector2{900,567}, PropType::TREE, Textures},      // 10
            Prop{Textures.TreeGreen, Vector2{1000,567}, PropType::TREE, Textures},     // 11
            Prop{Textures.TreeGreen, Vector2{1100,567}, PropType::TREE, Textures},     // 12
            Prop{Textures.TreeGreen, Vector2{1200,567}, PropType::TREE, Textures},     // 13
            // Prop{Textures.TreeGreen, Vector2{1300,567}, PropType::TREE, Textures},  // 14
            Prop{Textures.TreeGreen, Vector2{1400,567}, PropType::TREE, Textures},     // 15
            Prop{Textures.TreeGreen, Vector2{1500,567}, PropType::TREE, Textures},     // 16
            Prop{Textures.TreeGreen, Vector2{1600,567}, PropType::TREE, Textures},     // 17
            Prop{Textures.TreeGreen, Vector2{1700,567}, PropType::TREE, Textures},     // 18
            // Prop{Textures.TreeGreen, Vector2{1800,567}, PropType::TREE, Textures},     // 19
            // Prop{Textures.TreeGreen, Vector2{1900,567}, PropType::TREE, Textures},     // 20
            // Prop{Textures.TreeGreen, Vector2{2000,567}, PropType::TREE, Textures},     // 21
            // Prop{Textures.TreeGreen, Vector2{2100,567}, PropType::TREE, Textures},     // 22
            // Prop{Textures.TreeGreen, Vector2{2200,567}, PropType::TREE, Textures},     // 23
            // Prop{Textures.TreeGreen, Vector2{2300,567}, PropType::TREE, Textures},     // 24
            // Prop{Textures.TreeGreen, Vector2{2400,567}, PropType::TREE, Textures},     // 25
            Prop{Textures.TreeGreen, Vector2{2500,567}, PropType::TREE, Textures},     // 26
            // Prop{Textures.TreeGreen, Vector2{2600,567}, PropType::TREE, Textures},  // 27
            Prop{Textures.TreeGreen, Vector2{2700,567}, PropType::TREE, Textures},     // 28
            Prop{Textures.TreeGreen, Vector2{2800,567}, PropType::TREE, Textures},     // 29
            Prop{Textures.TreeGreen, Vector2{2900,567}, PropType::TREE, Textures},     // 30
            Prop{Textures.TreeGreen, Vector2{3000,567}, PropType::TREE, Textures},     // 31
            Prop{Textures.TreeGreen, Vector2{3100,567}, PropType::TREE, Textures},     // 32
            Prop{Textures.TreeGreen, Vector2{3200,567}, PropType::TREE, Textures},     // 33
            Prop{Textures.TreeGreen, Vector2{3300,567}, PropType::TREE, Textures},     // 34
            Prop{Textures.TreeGreen, Vector2{3400,567}, PropType::TREE, Textures},     // 35

            Prop{Textures.TreeGreen, Vector2{3720,567}, PropType::TREE, Textures},     // 36
            Prop{Textures.TreeGreen, Vector2{3820,567}, PropType::TREE, Textures},     // 37
            Prop{Textures.TreeGreen, Vector2{3920,567}, PropType::TREE, Textures},     // 38

            // row7
            Prop{Textures.TreeGreen, Vector2{20,667}, PropType::TREE, Textures},       // 1 
            Prop{Textures.TreeGreen, Vector2{120,667}, PropType::TREE, Textures},      // 2 
            Prop{Textures.TreeGreen, Vector2{220,667}, PropType::TREE, Textures},      // 3 
            Prop{Textures.TreeGreen, Vector2{320,667}, PropType::TREE, Textures},      // 4 
            Prop{Textures.TreeGreen, Vector2{420,667}, PropType::TREE, Textures},      // 5 
            Prop{Textures.TreeGreen, Vector2{520,667}, PropType::TREE, Textures},      // 6 
            // Prop{Textures.TreeGreen, Vector2{620,667}, PropType::TREE, Textures},   // 7     
            Prop{Textures.TreeGreen, Vector2{720,667}, PropType::TREE, Textures},      // 8 
            Prop{Textures.TreeGreen, Vector2{820,667}, PropType::TREE, Textures},      // 9 
            Prop{Textures.TreeGreen, Vector2{920,667}, PropType::TREE, Textures},      // 10
            Prop{Textures.TreeGreen, Vector2{1020,667}, PropType::TREE, Textures},     // 11
            // Prop{Textures.TreeGreen, Vector2{1120,667}, PropType::TREE, Textures},  // 12    
            Prop{Textures.TreeGreen, Vector2{1220,667}, PropType::TREE, Textures},     // 13
            Prop{Textures.TreeGreen, Vector2{1320,667}, PropType::TREE, Textures},     // 14
            Prop{Textures.TreeGreen, Vector2{1420,667}, PropType::TREE, Textures},     // 15
            Prop{Textures.TreeGreen, Vector2{1520,667}, PropType::TREE, Textures},     // 16
            // Prop{Textures.TreeGreen, Vector2{1620,667}, PropType::TREE, Textures},  // 17    
            Prop{Textures.TreeGreen, Vector2{1720,667}, PropType::TREE, Textures},     // 18
            Prop{Textures.TreeGreen, Vector2{1820,667}, PropType::TREE, Textures},     // 19
            // Prop{Textures.TreeGreen, Vector2{1920,667}, PropType::TREE, Textures},     // 20
            // Prop{Textures.TreeGreen, Vector2{2020,667}, PropType::TREE, Textures},     // 21
            // Prop{Textures.TreeGreen, Vector2{2120,667}, PropType::TREE, Textures},     // 22
            // Prop{Textures.TreeGreen, Vector2{2220,667}, PropType::TREE, Textures},     // 23
            // // Prop{Textures.TreeGreen, Vector2{2320,667}, PropType::TREE, Textures},  // 24    
            Prop{Textures.TreeGreen, Vector2{2420,667}, PropType::TREE, Textures},     // 25
            Prop{Textures.TreeGreen, Vector2{2520,667}, PropType::TREE, Textures},     // 26
            Prop{Textures.TreeGreen, Vector2{2620,667}, PropType::TREE, Textures},     // 27
            Prop{Textures.TreeGreen, Vector2{2720,667}, PropType::TREE, Textures},     // 28    
            // Prop{Textures.TreeGreen, Vector2{2820,667}, PropType::TREE, Textures},     // 29
            // Prop{Textures.TreeGreen, Vector2{2920,667}, PropType::TREE, Textures},     // 30
            // Prop{Textures.TreeGreen, Vector2{3020,667}, PropType::TREE, Textures},     // 31
            Prop{Textures.TreeGreen, Vector2{3120,667}, PropType::TREE, Textures},     // 32    
            Prop{Textures.TreeGreen, Vector2{3220,667}, PropType::TREE, Textures},     // 33
            Prop{Textures.TreeGreen, Vector2{3320,667}, PropType::TREE, Textures},     // 34
            Prop{Textures.TreeGreen, Vector2{3420,667}, PropType::TREE, Textures},     // 35

            Prop{Textures.TreeGreen, Vector2{3700,667}, PropType::TREE, Textures},     // 36    
            Prop{Textures.TreeGreen, Vector2{3800,667}, PropType::TREE, Textures},     // 37
            Prop{Textures.TreeGreen, Vector2{3900,667}, PropType::TREE, Textures},     // 38
            Prop{Textures.TreeGreen, Vector2{4000,667}, PropType::TREE, Textures},     // 39

            // row8
            Prop{Textures.TreeGreen, Vector2{0,767}, PropType::TREE, Textures},        // 1 
            Prop{Textures.TreeGreen, Vector2{100,767}, PropType::TREE, Textures},      // 2 
            Prop{Textures.TreeGreen, Vector2{200,767}, PropType::TREE, Textures},      // 3 
            Prop{Textures.TreeGreen, Vector2{300,767}, PropType::TREE, Textures},      // 4 
            Prop{Textures.TreeGreen, Vector2{400,767}, PropType::TREE, Textures},      // 5 
            Prop{Textures.TreeGreen, Vector2{500,767}, PropType::TREE, Textures},      // 6 
            Prop{Textures.TreeGreen, Vector2{600,767}, PropType::TREE, Textures},      // 7 
            // Prop{Textures.TreeGreen, Vector2{700,767}, PropType::TREE, Textures},   // 8     
            Prop{Textures.TreeGreen, Vector2{800,767}, PropType::TREE, Textures},      // 9 
            Prop{Textures.TreeGreen, Vector2{900,767}, PropType::TREE, Textures},      // 10
            Prop{Textures.TreeGreen, Vector2{1000,767}, PropType::TREE, Textures},     // 11
            // Prop{Textures.TreeGreen, Vector2{1100,767}, PropType::TREE, Textures},  // 12    
            Prop{Textures.TreeGreen, Vector2{1200,767}, PropType::TREE, Textures},     // 13
            Prop{Textures.TreeGreen, Vector2{1300,767}, PropType::TREE, Textures},     // 14
            Prop{Textures.TreeGreen, Vector2{1400,767}, PropType::TREE, Textures},     // 15
            Prop{Textures.TreeGreen, Vector2{1500,767}, PropType::TREE, Textures},     // 16
            Prop{Textures.TreeGreen, Vector2{1600,767}, PropType::TREE, Textures},     // 17
            Prop{Textures.TreeGreen, Vector2{1700,767}, PropType::TREE, Textures},     // 18
            // Prop{Textures.TreeGreen, Vector2{1800,767}, PropType::TREE, Textures},  // 19    
            Prop{Textures.TreeGreen, Vector2{1900,767}, PropType::TREE, Textures},     // 20
            // Prop{Textures.TreeGreen, Vector2{2000,767}, PropType::TREE, Textures},     // 21
            // Prop{Textures.TreeGreen, Vector2{2100,767}, PropType::TREE, Textures},     // 22
            // Prop{Textures.TreeGreen, Vector2{2200,767}, PropType::TREE, Textures},     // 23
            // Prop{Textures.TreeGreen, Vector2{2300,767}, PropType::TREE, Textures},     // 24
            Prop{Textures.TreeGreen, Vector2{2400,767}, PropType::TREE, Textures},     // 25
            Prop{Textures.TreeGreen, Vector2{2500,767}, PropType::TREE, Textures},     // 26
            Prop{Textures.TreeGreen, Vector2{2600,767}, PropType::TREE, Textures},     // 27
            // Prop{Textures.TreeGreen, Vector2{2700,767}, PropType::TREE, Textures},     // 28    
            // Prop{Textures.TreeGreen, Vector2{2800,767}, PropType::TREE, Textures},     // 29
            // Prop{Textures.TreeGreen, Vector2{2900,767}, PropType::TREE, Textures},     // 30
            // Prop{Textures.TreeGreen, Vector2{3000,767}, PropType::TREE, Textures},     // 31
            // Prop{Textures.TreeGreen, Vector2{3100,767}, PropType::TREE, Textures},     // 32
            Prop{Textures.TreeGreen, Vector2{3200,767}, PropType::TREE, Textures},     // 33
            Prop{Textures.TreeGreen, Vector2{3300,767}, PropType::TREE, Textures},     // 34    
            Prop{Textures.TreeGreen, Vector2{3400,767}, PropType::TREE, Textures},     // 35

            Prop{Textures.TreeGreen, Vector2{3720,767}, PropType::TREE, Textures},     // 36    
            Prop{Textures.TreeGreen, Vector2{3820,767}, PropType::TREE, Textures},     // 37    
            Prop{Textures.TreeGreen, Vector2{3920,767}, PropType::TREE, Textures},     // 38

            // row9
            Prop{Textures.TreeGreen, Vector2{20,867}, PropType::TREE, Textures},       // 1 
            Prop{Textures.TreeGreen, Vector2{120,867}, PropType::TREE, Textures},      // 2 
            Prop{Textures.TreeGreen, Vector2{220,867}, PropType::TREE, Textures},      // 3 
            Prop{Textures.TreeGreen, Vector2{320,867}, PropType::TREE, Textures},      // 4 
            Prop{Textures.TreeGreen, Vector2{420,867}, PropType::TREE, Textures},      // 5 
            Prop{Textures.TreeGreen, Vector2{520,867}, PropType::TREE, Textures},      // 6 
            // Prop{Textures.TreeGreen, Vector2{620,867}, PropType::TREE, Textures},      // 7 
            // Prop{Textures.TreeGreen, Vector2{720,867}, PropType::TREE, Textures},      // 8 
            // Prop{Textures.TreeGreen, Vector2{820,867}, PropType::TREE, Textures},      // 9 
            // Prop{Textures.TreeGreen, Vector2{920,867}, PropType::TREE, Textures},      // 10
            Prop{Textures.TreeGreen, Vector2{1020,867}, PropType::TREE, Textures},     // 11
            Prop{Textures.TreeGreen, Vector2{1120,867}, PropType::TREE, Textures},     // 12
            Prop{Textures.TreeGreen, Vector2{1220,867}, PropType::TREE, Textures},     // 13
            Prop{Textures.TreeGreen, Vector2{1320,867}, PropType::TREE, Textures},     // 14
            Prop{Textures.TreeGreen, Vector2{1420,867}, PropType::TREE, Textures},     // 15
            Prop{Textures.TreeGreen, Vector2{1520,867}, PropType::TREE, Textures},     // 16
            Prop{Textures.TreeGreen, Vector2{1620,867}, PropType::TREE, Textures},     // 17
            Prop{Textures.TreeGreen, Vector2{1720,867}, PropType::TREE, Textures},     // 18
            Prop{Textures.TreeGreen, Vector2{1820,867}, PropType::TREE, Textures},     // 19
            Prop{Textures.TreeGreen, Vector2{1920,867}, PropType::TREE, Textures},     // 20
            Prop{Textures.TreeGreen, Vector2{2020,867}, PropType::TREE, Textures},     // 21
            Prop{Textures.TreeGreen, Vector2{2120,867}, PropType::TREE, Textures},     // 22
            Prop{Textures.TreeGreen, Vector2{2220,867}, PropType::TREE, Textures},     // 23
            Prop{Textures.TreeGreen, Vector2{2320,867}, PropType::TREE, Textures},     // 24
            Prop{Textures.TreeGreen, Vector2{2420,867}, PropType::TREE, Textures},     // 25
            Prop{Textures.TreeGreen, Vector2{2520,867}, PropType::TREE, Textures},     // 26
            Prop{Textures.TreeGreen, Vector2{2620,867}, PropType::TREE, Textures},     // 27
            // Prop{Textures.TreeGreen, Vector2{2720,867}, PropType::TREE, Textures},     // 28
            // Prop{Textures.TreeGreen, Vector2{2820,867}, PropType::TREE, Textures},     // 29
            // Prop{Textures.TreeGreen, Vector2{2920,867}, PropType::TREE, Textures},     // 30
            // Prop{Textures.TreeGreen, Vector2{3020,867}, PropType::TREE, Textures},     // 31
            // Prop{Textures.TreeGreen, Vector2{3120,867}, PropType::TREE, Textures},     // 32
            Prop{Textures.TreeGreen, Vector2{3220,867}, PropType::TREE, Textures},     // 33
            Prop{Textures.TreeGreen, Vector2{3320,867}, PropType::TREE, Textures},     // 34
            Prop{Textures.TreeGreen, Vector2{3420,867}, PropType::TREE, Textures},     // 35

            Prop{Textures.TreeGreen, Vector2{3700,867}, PropType::TREE, Textures},     // 36
            Prop{Textures.TreeGreen, Vector2{3800,867}, PropType::TREE, Textures},     // 37
            Prop{Textures.TreeGreen, Vector2{3900,867}, PropType::TREE, Textures},     // 38
            Prop{Textures.TreeGreen, Vector2{4000,867}, PropType::TREE, Textures},     // 39

            // row10
            Prop{Textures.TreeGreen, Vector2{0,967}, PropType::TREE, Textures},        // 1 
            Prop{Textures.TreeGreen, Vector2{100,967}, PropType::TREE, Textures},      // 2 
            Prop{Textures.TreeGreen, Vector2{200,967}, PropType::TREE, Textures},      // 3 
            Prop{Textures.TreeGreen, Vector2{300,967}, PropType::TREE, Textures},      // 4 
            Prop{Textures.TreeGreen, Vector2{400,967}, PropType::TREE, Textures},      // 5 
            // Prop{Textures.TreeGreen, Vector2{500,967}, PropType::TREE, Textures},      // 6 
            // Prop{Textures.TreeGreen, Vector2{600,967}, PropType::TREE, Textures},      // 7 
            // Prop{Textures.TreeGreen, Vector2{700,967}, PropType::TREE, Textures},      // 8 
            // Prop{Textures.TreeGreen, Vector2{800,967}, PropType::TREE, Textures},      // 9 
            // Prop{Textures.TreeGreen, Vector2{900,967}, PropType::TREE, Textures},      // 10
            // Prop{Textures.TreeGreen, Vector2{1000,967}, PropType::TREE, Textures},     // 11
            Prop{Textures.TreeGreen, Vector2{1100,967}, PropType::TREE, Textures},     // 12
            Prop{Textures.TreeGreen, Vector2{1200,967}, PropType::TREE, Textures},     // 13
            Prop{Textures.TreeGreen, Vector2{1300,967}, PropType::TREE, Textures},     // 14
            Prop{Textures.TreeGreen, Vector2{1400,967}, PropType::TREE, Textures},     // 15
            Prop{Textures.TreeGreen, Vector2{1500,967}, PropType::TREE, Textures},     // 16
            Prop{Textures.TreeGreen, Vector2{1600,967}, PropType::TREE, Textures},     // 17
            Prop{Textures.TreeGreen, Vector2{1700,967}, PropType::TREE, Textures},     // 18
            Prop{Textures.TreeGreen, Vector2{1800,967}, PropType::TREE, Textures},     // 19
            Prop{Textures.TreeGreen, Vector2{1900,967}, PropType::TREE, Textures},     // 20
            Prop{Textures.TreeGreen, Vector2{2000,967}, PropType::TREE, Textures},     // 21
            Prop{Textures.TreeGreen, Vector2{2100,967}, PropType::TREE, Textures},     // 22
            Prop{Textures.TreeGreen, Vector2{2200,967}, PropType::TREE, Textures},     // 23
            Prop{Textures.TreeGreen, Vector2{2300,967}, PropType::TREE, Textures},     // 24
            Prop{Textures.TreeGreen, Vector2{2400,967}, PropType::TREE, Textures},     // 25
            Prop{Textures.TreeGreen, Vector2{2500,967}, PropType::TREE, Textures},     // 26
            Prop{Textures.TreeGreen, Vector2{2600,967}, PropType::TREE, Textures},     // 27
            // Prop{Textures.TreeGreen, Vector2{2700,967}, PropType::TREE, Textures},     // 28
            // Prop{Textures.TreeGreen, Vector2{2800,967}, PropType::TREE, Textures},     // 29
            // Prop{Textures.TreeGreen, Vector2{2900,967}, PropType::TREE, Textures},     // 30
            // Prop{Textures.TreeGreen, Vector2{3000,967}, PropType::TREE, Textures},     // 31
            // Prop{Textures.TreeGreen, Vector2{3100,967}, PropType::TREE, Textures},     // 32
            Prop{Textures.TreeGreen, Vector2{3200,967}, PropType::TREE, Textures},     // 33
            Prop{Textures.TreeGreen, Vector2{3300,967}, PropType::TREE, Textures},     // 34
            Prop{Textures.TreeGreen, Vector2{3400,967}, PropType::TREE, Textures},     // 35

            Prop{Textures.TreeGreen, Vector2{3720,967}, PropType::TREE, Textures},     // 36
            Prop{Textures.TreeGreen, Vector2{3820,967}, PropType::TREE, Textures},     // 37
            Prop{Textures.TreeGreen, Vector2{3920,967}, PropType::TREE, Textures},     // 38

            // row11
            Prop{Textures.TreeGreen, Vector2{20,1067}, PropType::TREE, Textures},      // 1 
            Prop{Textures.TreeGreen, Vector2{120,1067}, PropType::TREE, Textures},     // 2     
            Prop{Textures.TreeGreen, Vector2{220,1067}, PropType::TREE, Textures},     // 3     
            Prop{Textures.TreeGreen, Vector2{320,1067}, PropType::TREE, Textures},     // 4 
            Prop{Textures.TreeGreen, Vector2{420,1067}, PropType::TREE, Textures},     // 5 
            // Prop{Textures.TreeGreen, Vector2{520,1067}, PropType::TREE, Textures},     // 6 
            // Prop{Textures.TreeGreen, Vector2{620,1067}, PropType::TREE, Textures},     // 7 
            // Prop{Textures.TreeGreen, Vector2{720,1067}, PropType::TREE, Textures},     // 8 
            // Prop{Textures.TreeGreen, Vector2{820,1067}, PropType::TREE, Textures},     // 9 
            // Prop{Textures.TreeGreen, Vector2{920,1067}, PropType::TREE, Textures},     // 10
            // Prop{Textures.TreeGreen, Vector2{1020,1067}, PropType::TREE, Textures},    // 11    
            // Prop{Textures.TreeGreen, Vector2{1120,1067}, PropType::TREE, Textures},    // 12    
            // Prop{Textures.TreeGreen, Vector2{1220,1067}, PropType::TREE, Textures},    // 13    
            Prop{Textures.TreeGreen, Vector2{1320,1067}, PropType::TREE, Textures},    // 14    
            Prop{Textures.TreeGreen, Vector2{1420,1067}, PropType::TREE, Textures},    // 15    
            Prop{Textures.TreeGreen, Vector2{1520,1067}, PropType::TREE, Textures},    // 16    
            Prop{Textures.TreeGreen, Vector2{1620,1067}, PropType::TREE, Textures},    // 17    
            Prop{Textures.TreeGreen, Vector2{1720,1067}, PropType::TREE, Textures},    // 18    
            Prop{Textures.TreeGreen, Vector2{1820,1067}, PropType::TREE, Textures},    // 19    
            Prop{Textures.TreeGreen, Vector2{1920,1067}, PropType::TREE, Textures},    // 20    
            Prop{Textures.TreeGreen, Vector2{2020,1067}, PropType::TREE, Textures},    // 21    
            Prop{Textures.TreeGreen, Vector2{2120,1067}, PropType::TREE, Textures},    // 22    
            Prop{Textures.TreeGreen, Vector2{2220,1067}, PropType::TREE, Textures},    // 23    
            Prop{Textures.TreeGreen, Vector2{2320,1067}, PropType::TREE, Textures},    // 24    
            Prop{Textures.TreeGreen, Vector2{2420,1067}, PropType::TREE, Textures},    // 25    
            Prop{Textures.TreeGreen, Vector2{2520,1067}, PropType::TREE, Textures},    // 26    
            Prop{Textures.TreeGreen, Vector2{2620,1067}, PropType::TREE, Textures},    // 27    
            Prop{Textures.TreeGreen, Vector2{2720,1067}, PropType::TREE, Textures},    // 28    
            // Prop{Textures.TreeGreen, Vector2{2820,1067}, PropType::TREE, Textures},    // 29    
            // Prop{Textures.TreeGreen, Vector2{2920,1067}, PropType::TREE, Textures},    // 30    
            // Prop{Textures.TreeGreen, Vector2{3020,1067}, PropType::TREE, Textures},    // 31    
            Prop{Textures.TreeGreen, Vector2{3120,1067}, PropType::TREE, Textures},    // 32    
            Prop{Textures.TreeGreen, Vector2{3220,1067}, PropType::TREE, Textures},    // 33    
            Prop{Textures.TreeGreen, Vector2{3320,1067}, PropType::TREE, Textures},    // 34    
            Prop{Textures.TreeGreen, Vector2{3420,1067}, PropType::TREE, Textures},    // 35    

            Prop{Textures.TreeGreen, Vector2{3700,1067}, PropType::TREE, Textures},    // 36    
            Prop{Textures.TreeGreen, Vector2{3800,1067}, PropType::TREE, Textures},    // 37    
            Prop{Textures.TreeGreen, Vector2{3900,1067}, PropType::TREE, Textures},    // 38    
            Prop{Textures.TreeGreen, Vector2{4000,1067}, PropType::TREE, Textures},    // 39

            // row12
            Prop{Textures.TreeGreen, Vector2{0,1167}, PropType::TREE, Textures},       // 1 
            Prop{Textures.TreeGreen, Vector2{100,1167}, PropType::TREE, Textures},     // 2 
            Prop{Textures.TreeGreen, Vector2{200,1167}, PropType::TREE, Textures},     // 3 
            Prop{Textures.TreeGreen, Vector2{300,1167}, PropType::TREE, Textures},     // 4 
            Prop{Textures.TreeGreen, Vector2{400,1167}, PropType::TREE, Textures},     // 5 
            // Prop{Textures.TreeGreen, Vector2{500,1167}, PropType::TREE, Textures},     // 6 
            // Prop{Textures.TreeGreen, Vector2{600,1167}, PropType::TREE, Textures},     // 7 
            // Prop{Textures.TreeGreen, Vector2{700,1167}, PropType::TREE, Textures},     // 8 
            // Prop{Textures.TreeGreen, Vector2{800,1167}, PropType::TREE, Textures},     // 9 
            // Prop{Textures.TreeGreen, Vector2{900,1167}, PropType::TREE, Textures},     // 10
            // Prop{Textures.TreeGreen, Vector2{1000,1167}, PropType::TREE, Textures},    // 11    
            Prop{Textures.TreeGreen, Vector2{1100,1167}, PropType::TREE, Textures},    // 12    
            Prop{Textures.TreeGreen, Vector2{1200,1167}, PropType::TREE, Textures},    // 13    
            Prop{Textures.TreeGreen, Vector2{1300,1167}, PropType::TREE, Textures},    // 14    
            Prop{Textures.TreeGreen, Vector2{1400,1167}, PropType::TREE, Textures},    // 15    
            Prop{Textures.TreeGreen, Vector2{1500,1167}, PropType::TREE, Textures},    // 16    
            Prop{Textures.TreeGreen, Vector2{1600,1167}, PropType::TREE, Textures},    // 17    
            Prop{Textures.TreeGreen, Vector2{1700,1167}, PropType::TREE, Textures},    // 18    
            Prop{Textures.TreeGreen, Vector2{1800,1167}, PropType::TREE, Textures},    // 19    
            Prop{Textures.TreeGreen, Vector2{1900,1167}, PropType::TREE, Textures},    // 20    
            Prop{Textures.TreeGreen, Vector2{2000,1167}, PropType::TREE, Textures},    // 21    
            Prop{Textures.TreeGreen, Vector2{2100,1167}, PropType::TREE, Textures},    // 22    
            Prop{Textures.TreeGreen, Vector2{2200,1167}, PropType::TREE, Textures},    // 23    
            Prop{Textures.TreeGreen, Vector2{2300,1167}, PropType::TREE, Textures},    // 24    
            Prop{Textures.TreeGreen, Vector2{2400,1167}, PropType::TREE, Textures},    // 25    
            Prop{Textures.TreeGreen, Vector2{2500,1167}, PropType::TREE, Textures},    // 26    
            // Prop{Textures.TreeGreen, Vector2{2600,1167}, PropType::TREE, Textures},    // 27    
            // Prop{Textures.TreeGreen, Vector2{2700,1167}, PropType::TREE, Textures},    // 28    
            Prop{Textures.TreeGreen, Vector2{2800,1167}, PropType::TREE, Textures},    // 29    
            Prop{Textures.TreeGreen, Vector2{2900,1167}, PropType::TREE, Textures},    // 30    
            Prop{Textures.TreeGreen, Vector2{3000,1167}, PropType::TREE, Textures},    // 31    
            Prop{Textures.TreeGreen, Vector2{3100,1167}, PropType::TREE, Textures},    // 32    
            // Prop{Textures.TreeGreen, Vector2{3200,1167}, PropType::TREE, Textures},    // 33    
            Prop{Textures.TreeGreen, Vector2{3300,1167}, PropType::TREE, Textures},    // 34    
            Prop{Textures.TreeGreen, Vector2{3400,1167}, PropType::TREE, Textures},    // 35    

            Prop{Textures.TreeGreen, Vector2{3720,1167}, PropType::TREE, Textures},    // 36    
            Prop{Textures.TreeGreen, Vector2{3820,1167}, PropType::TREE, Textures},    // 37    
            Prop{Textures.TreeGreen, Vector2{3920,1167}, PropType::TREE, Textures},    // 38    

            // row13
            Prop{Textures.TreeGreen, Vector2{20,1267}, PropType::TREE, Textures},      // 1 
            Prop{Textures.TreeGreen, Vector2{120,1267}, PropType::TREE, Textures},     // 2 
            Prop{Textures.TreeGreen, Vector2{220,1267}, PropType::TREE, Textures},     // 3 
            Prop{Textures.TreeGreen, Vector2{320,1267}, PropType::TREE, Textures},     // 4 
            Prop{Textures.TreeGreen, Vector2{420,1267}, PropType::TREE, Textures},     // 5 
            Prop{Textures.TreeGreen, Vector2{520,1267}, PropType::TREE, Textures},     // 6 
            // Prop{Textures.TreeGreen, Vector2{620,1267}, PropType::TREE, Textures},     // 7 
            // Prop{Textures.TreeGreen, Vector2{720,1267}, PropType::TREE, Textures},     // 8 
            // Prop{Textures.TreeGreen, Vector2{820,1267}, PropType::TREE, Textures},     // 9 
            // Prop{Textures.TreeGreen, Vector2{920,1267}, PropType::TREE, Textures},     // 10
            Prop{Textures.TreeGreen, Vector2{1020,1267}, PropType::TREE, Textures},    // 11
            Prop{Textures.TreeGreen, Vector2{1120,1267}, PropType::TREE, Textures},    // 12
            Prop{Textures.TreeGreen, Vector2{1220,1267}, PropType::TREE, Textures},    // 13
            Prop{Textures.TreeGreen, Vector2{1320,1267}, PropType::TREE, Textures},    // 14
            Prop{Textures.TreeGreen, Vector2{1420,1267}, PropType::TREE, Textures},    // 15
            Prop{Textures.TreeGreen, Vector2{1520,1267}, PropType::TREE, Textures},    // 16
            Prop{Textures.TreeGreen, Vector2{1620,1267}, PropType::TREE, Textures},    // 17
            Prop{Textures.TreeGreen, Vector2{1720,1267}, PropType::TREE, Textures},    // 18
            Prop{Textures.TreeGreen, Vector2{1820,1267}, PropType::TREE, Textures},    // 19
            Prop{Textures.TreeGreen, Vector2{1920,1267}, PropType::TREE, Textures},    // 20
            // Prop{Textures.TreeGreen, Vector2{2020,1267}, PropType::TREE, Textures},    // 21
            // Prop{Textures.TreeGreen, Vector2{2120,1267}, PropType::TREE, Textures},    // 22
            // Prop{Textures.TreeGreen, Vector2{2220,1267}, PropType::TREE, Textures},    // 23
            // Prop{Textures.TreeGreen, Vector2{2320,1267}, PropType::TREE, Textures},    // 24
            Prop{Textures.TreeGreen, Vector2{2420,1267}, PropType::TREE, Textures},    // 25
            Prop{Textures.TreeGreen, Vector2{2520,1267}, PropType::TREE, Textures},    // 26
            Prop{Textures.TreeGreen, Vector2{2620,1267}, PropType::TREE, Textures},    // 27
            Prop{Textures.TreeGreen, Vector2{2720,1267}, PropType::TREE, Textures},    // 28
            Prop{Textures.TreeGreen, Vector2{2820,1267}, PropType::TREE, Textures},    // 29
            Prop{Textures.TreeGreen, Vector2{2920,1267}, PropType::TREE, Textures},    // 30
            Prop{Textures.TreeGreen, Vector2{3020,1267}, PropType::TREE, Textures},    // 31
            Prop{Textures.TreeGreen, Vector2{3120,1267}, PropType::TREE, Textures},    // 32
            Prop{Textures.TreeGreen, Vector2{3220,1267}, PropType::TREE, Textures},    // 33
            Prop{Textures.TreeGreen, Vector2{3320,1267}, PropType::TREE, Textures},    // 34
            Prop{Textures.TreeGreen, Vector2{3420,1267}, PropType::TREE, Textures},    // 35

            Prop{Textures.TreeGreen, Vector2{3700,1267}, PropType::TREE, Textures},    // 36
            Prop{Textures.TreeGreen, Vector2{3800,1267}, PropType::TREE, Textures},    // 37
            Prop{Textures.TreeGreen, Vector2{3900,1267}, PropType::TREE, Textures},    // 38
            Prop{Textures.TreeGreen, Vector2{4000,1267}, PropType::TREE, Textures},    // 39

            // row14
            Prop{Textures.TreeGreen, Vector2{0,1367}, PropType::TREE, Textures},       // 1 
            Prop{Textures.TreeGreen, Vector2{100,1367}, PropType::TREE, Textures},     // 2     
            Prop{Textures.TreeGreen, Vector2{200,1367}, PropType::TREE, Textures},     // 3     
            Prop{Textures.TreeGreen, Vector2{300,1367}, PropType::TREE, Textures},     // 4     
            Prop{Textures.TreeGreen, Vector2{400,1367}, PropType::TREE, Textures},     // 5     
            Prop{Textures.TreeGreen, Vector2{500,1367}, PropType::TREE, Textures},     // 6     
            // Prop{Textures.TreeGreen, Vector2{600,1367}, PropType::TREE, Textures},     // 7     
            Prop{Textures.TreeGreen, Vector2{700,1367}, PropType::TREE, Textures},     // 8     
            Prop{Textures.TreeGreen, Vector2{800,1367}, PropType::TREE, Textures},     // 9     
            Prop{Textures.TreeGreen, Vector2{900,1367}, PropType::TREE, Textures},     // 10    
            Prop{Textures.TreeGreen, Vector2{1000,1367}, PropType::TREE, Textures},    // 11    
            Prop{Textures.TreeGreen, Vector2{1100,1367}, PropType::TREE, Textures},    // 12    
            Prop{Textures.TreeGreen, Vector2{1200,1367}, PropType::TREE, Textures},    // 13    
            Prop{Textures.TreeGreen, Vector2{1300,1367}, PropType::TREE, Textures},    // 14    
            Prop{Textures.TreeGreen, Vector2{1400,1367}, PropType::TREE, Textures},    // 15    
            Prop{Textures.TreeGreen, Vector2{1500,1367}, PropType::TREE, Textures},    // 16    
            Prop{Textures.TreeGreen, Vector2{1600,1367}, PropType::TREE, Textures},    // 17    
            Prop{Textures.TreeGreen, Vector2{1700,1367}, PropType::TREE, Textures},    // 18    
            Prop{Textures.TreeGreen, Vector2{1800,1367}, PropType::TREE, Textures},    // 19    
            // Prop{Textures.TreeGreen, Vector2{1900,1367}, PropType::TREE, Textures},    // 20    
            // Prop{Textures.TreeGreen, Vector2{2000,1367}, PropType::TREE, Textures},    // 21    
            // Prop{Textures.TreeGreen, Vector2{2100,1367}, PropType::TREE, Textures},    // 22    
            // Prop{Textures.TreeGreen, Vector2{2200,1367}, PropType::TREE, Textures},    // 23    
            // Prop{Textures.TreeGreen, Vector2{2300,1367}, PropType::TREE, Textures},    // 24    
            // Prop{Textures.TreeGreen, Vector2{2400,1367}, PropType::TREE, Textures},    // 25    
            Prop{Textures.TreeGreen, Vector2{2500,1367}, PropType::TREE, Textures},    // 26    
            Prop{Textures.TreeGreen, Vector2{2600,1367}, PropType::TREE, Textures},    // 27    
            Prop{Textures.TreeGreen, Vector2{2700,1367}, PropType::TREE, Textures},    // 28    
            Prop{Textures.TreeGreen, Vector2{2800,1367}, PropType::TREE, Textures},    // 29    
            Prop{Textures.TreeGreen, Vector2{2900,1367}, PropType::TREE, Textures},    // 30    
            // Prop{Textures.TreeGreen, Vector2{3000,1367}, PropType::TREE, Textures},    // 31    
            Prop{Textures.TreeGreen, Vector2{3100,1367}, PropType::TREE, Textures},    // 32    
            Prop{Textures.TreeGreen, Vector2{3200,1367}, PropType::TREE, Textures},    // 33    
            Prop{Textures.TreeGreen, Vector2{3300,1367}, PropType::TREE, Textures},    // 34    
            Prop{Textures.TreeGreen, Vector2{3400,1367}, PropType::TREE, Textures},    // 35    

            Prop{Textures.TreeGreen, Vector2{3720,1367}, PropType::TREE, Textures},    // 36    
            Prop{Textures.TreeGreen, Vector2{3820,1367}, PropType::TREE, Textures},    // 37    
            Prop{Textures.TreeGreen, Vector2{3920,1367}, PropType::TREE, Textures},    // 38    

            // row15
            Prop{Textures.TreeGreen, Vector2{20,1467}, PropType::TREE, Textures},      // 1 
            Prop{Textures.TreeGreen, Vector2{120,1467}, PropType::TREE, Textures},     // 2 
            Prop{Textures.TreeGreen, Vector2{220,1467}, PropType::TREE, Textures},     // 3 
            Prop{Textures.TreeGreen, Vector2{320,1467}, PropType::TREE, Textures},     // 4 
            // Prop{Textures.TreeGreen, Vector2{420,1467}, PropType::TREE, Textures},     // 5 
            Prop{Textures.TreeGreen, Vector2{520,1467}, PropType::TREE, Textures},     // 6 
            // Prop{Textures.TreeGreen, Vector2{620,1467}, PropType::TREE, Textures},     // 7 
            Prop{Textures.TreeGreen, Vector2{720,1467}, PropType::TREE, Textures},     // 8 
            Prop{Textures.TreeGreen, Vector2{820,1467}, PropType::TREE, Textures},     // 9 
            Prop{Textures.TreeGreen, Vector2{920,1467}, PropType::TREE, Textures},     // 10
            // Prop{Textures.TreeGreen, Vector2{1020,1467}, PropType::TREE, Textures},    // 11
            // Prop{Textures.TreeGreen, Vector2{1120,1467}, PropType::TREE, Textures},    // 12
            Prop{Textures.TreeGreen, Vector2{1220,1467}, PropType::TREE, Textures},    // 13
            Prop{Textures.TreeGreen, Vector2{1320,1467}, PropType::TREE, Textures},    // 14
            Prop{Textures.TreeGreen, Vector2{1420,1467}, PropType::TREE, Textures},    // 15
            Prop{Textures.TreeGreen, Vector2{1520,1467}, PropType::TREE, Textures},    // 16
            Prop{Textures.TreeGreen, Vector2{1620,1467}, PropType::TREE, Textures},    // 17
            Prop{Textures.TreeGreen, Vector2{1720,1467}, PropType::TREE, Textures},    // 18
            Prop{Textures.TreeGreen, Vector2{1820,1467}, PropType::TREE, Textures},    // 19
            // Prop{Textures.TreeGreen, Vector2{1920,1467}, PropType::TREE, Textures},    // 20
            // Prop{Textures.TreeGreen, Vector2{2020,1467}, PropType::TREE, Textures},    // 21
            // Prop{Textures.TreeGreen, Vector2{2120,1467}, PropType::TREE, Textures},    // 22
            // Prop{Textures.TreeGreen, Vector2{2220,1467}, PropType::TREE, Textures},    // 23
            // Prop{Textures.TreeGreen, Vector2{2320,1467}, PropType::TREE, Textures},    // 24
            Prop{Textures.TreeGreen, Vector2{2420,1467}, PropType::TREE, Textures},    // 25
            Prop{Textures.TreeGreen, Vector2{2520,1467}, PropType::TREE, Textures},    // 26
            Prop{Textures.TreeGreen, Vector2{2620,1467}, PropType::TREE, Textures},    // 27
            Prop{Textures.TreeGreen, Vector2{2720,1467}, PropType::TREE, Textures},    // 28
            Prop{Textures.TreeGreen, Vector2{2820,1467}, PropType::TREE, Textures},    // 29
            Prop{Textures.TreeGreen, Vector2{2920,1467}, PropType::TREE, Textures},    // 30
            Prop{Textures.TreeGreen, Vector2{3020,1467}, PropType::TREE, Textures},    // 31
            Prop{Textures.TreeGreen, Vector2{3120,1467}, PropType::TREE, Textures},    // 32
            Prop{Textures.TreeGreen, Vector2{3220,1467}, PropType::TREE, Textures},    // 33
            Prop{Textures.TreeGreen, Vector2{3320,1467}, PropType::TREE, Textures},    // 34
            Prop{Textures.TreeGreen, Vector2{3420,1467}, PropType::TREE, Textures},    // 35

            Prop{Textures.TreeGreen, Vector2{3700,1467}, PropType::TREE, Textures},    // 36
            Prop{Textures.TreeGreen, Vector2{3800,1467}, PropType::TREE, Textures},    // 37
            Prop{Textures.TreeGreen, Vector2{3900,1467}, PropType::TREE, Textures},    // 38
            Prop{Textures.TreeGreen, Vector2{4000,1467}, PropType::TREE, Textures},    // 39

            // row16
            Prop{Textures.TreeGreen, Vector2{0,1567}, PropType::TREE, Textures},       // 1 
            Prop{Textures.TreeGreen, Vector2{100,1567}, PropType::TREE, Textures},     // 2 
            Prop{Textures.TreeGreen, Vector2{200,1567}, PropType::TREE, Textures},     // 3 
            // Prop{Textures.TreeGreen, Vector2{300,1567}, PropType::TREE, Textures},     // 4 
            Prop{Textures.TreeGreen, Vector2{400,1567}, PropType::TREE, Textures},     // 5 
            Prop{Textures.TreeGreen, Vector2{500,1567}, PropType::TREE, Textures},     // 6 
            Prop{Textures.TreeGreen, Vector2{600,1567}, PropType::TREE, Textures},     // 7 
            Prop{Textures.TreeGreen, Vector2{700,1567}, PropType::TREE, Textures},     // 8 
            // Prop{Textures.TreeGreen, Vector2{800,1567}, PropType::TREE, Textures},     // 9 
            Prop{Textures.TreeGreen, Vector2{900,1567}, PropType::TREE, Textures},     // 10
            Prop{Textures.TreeGreen, Vector2{1000,1567}, PropType::TREE, Textures},    // 11    
            Prop{Textures.TreeGreen, Vector2{1100,1567}, PropType::TREE, Textures},    // 12
            Prop{Textures.TreeGreen, Vector2{1200,1567}, PropType::TREE, Textures},    // 13
            Prop{Textures.TreeGreen, Vector2{1300,1567}, PropType::TREE, Textures},    // 14
            Prop{Textures.TreeGreen, Vector2{1400,1567}, PropType::TREE, Textures},    // 15
            Prop{Textures.TreeGreen, Vector2{1500,1567}, PropType::TREE, Textures},    // 16
            Prop{Textures.TreeGreen, Vector2{1600,1567}, PropType::TREE, Textures},    // 17
            // Prop{Textures.TreeGreen, Vector2{1700,1567}, PropType::TREE, Textures},    // 18
            // Prop{Textures.TreeGreen, Vector2{1800,1567}, PropType::TREE, Textures},    // 19
            // Prop{Textures.TreeGreen, Vector2{1900,1567}, PropType::TREE, Textures},    // 20
            // Prop{Textures.TreeGreen, Vector2{2000,1567}, PropType::TREE, Textures},    // 21
            // Prop{Textures.TreeGreen, Vector2{2100,1567}, PropType::TREE, Textures},    // 22
            // Prop{Textures.TreeGreen, Vector2{2200,1567}, PropType::TREE, Textures},    // 23
            // Prop{Textures.TreeGreen, Vector2{2300,1567}, PropType::TREE, Textures},    // 24
            // Prop{Textures.TreeGreen, Vector2{2400,1567}, PropType::TREE, Textures},    // 25
            Prop{Textures.TreeGreen, Vector2{2500,1567}, PropType::TREE, Textures},    // 26
            Prop{Textures.TreeGreen, Vector2{2600,1567}, PropType::TREE, Textures},    // 27
            Prop{Textures.TreeGreen, Vector2{2700,1567}, PropType::TREE, Textures},    // 28
            Prop{Textures.TreeGreen, Vector2{2800,1567}, PropType::TREE, Textures},    // 29
            Prop{Textures.TreeGreen, Vector2{2900,1567}, PropType::TREE, Textures},    // 30
            Prop{Textures.TreeGreen, Vector2{3000,1567}, PropType::TREE, Textures},    // 31
            Prop{Textures.TreeGreen, Vector2{3100,1567}, PropType::TREE, Textures},    // 32
            Prop{Textures.TreeGreen, Vector2{3200,1567}, PropType::TREE, Textures},    // 33
            Prop{Textures.TreeGreen, Vector2{3300,1567}, PropType::TREE, Textures},    // 34
            Prop{Textures.TreeGreen, Vector2{3400,1567}, PropType::TREE, Textures},    // 35

            Prop{Textures.TreeGreen, Vector2{3720,1567}, PropType::TREE, Textures},    // 36 
            Prop{Textures.TreeGreen, Vector2{3820,1567}, PropType::TREE, Textures},    // 37
            Prop{Textures.TreeGreen, Vector2{3920,1567}, PropType::TREE, Textures},    // 38

            // row17
            Prop{Textures.TreeGreen, Vector2{20,1667}, PropType::TREE, Textures},      // 1 
            Prop{Textures.TreeGreen, Vector2{120,1667}, PropType::TREE, Textures},     // 2 
            Prop{Textures.TreeGreen, Vector2{220,1667}, PropType::TREE, Textures},     // 3 
            // Prop{Textures.TreeGreen, Vector2{320,1667}, PropType::TREE, Textures},     // 4 
            // Prop{Textures.TreeGreen, Vector2{420,1667}, PropType::TREE, Textures},     // 5 
            Prop{Textures.TreeGreen, Vector2{520,1667}, PropType::TREE, Textures},     // 6 
            Prop{Textures.TreeGreen, Vector2{620,1667}, PropType::TREE, Textures},     // 7 
            Prop{Textures.TreeGreen, Vector2{720,1667}, PropType::TREE, Textures},     // 8 
            Prop{Textures.TreeGreen, Vector2{820,1667}, PropType::TREE, Textures},     // 9 
            Prop{Textures.TreeGreen, Vector2{920,1667}, PropType::TREE, Textures},     // 10
            // Prop{Textures.TreeGreen, Vector2{1020,1667}, PropType::TREE, Textures},    // 11
            Prop{Textures.TreeGreen, Vector2{1120,1667}, PropType::TREE, Textures},    // 12
            Prop{Textures.TreeGreen, Vector2{1220,1667}, PropType::TREE, Textures},    // 13
            Prop{Textures.TreeGreen, Vector2{1320,1667}, PropType::TREE, Textures},    // 14
            // Prop{Textures.TreeGreen, Vector2{1420,1667}, PropType::TREE, Textures},    // 15
            // Prop{Textures.TreeGreen, Vector2{1520,1667}, PropType::TREE, Textures},    // 16
            // Prop{Textures.TreeGreen, Vector2{1620,1667}, PropType::TREE, Textures},    // 17
            // Prop{Textures.TreeGreen, Vector2{1720,1667}, PropType::TREE, Textures},    // 18
            // Prop{Textures.TreeGreen, Vector2{1820,1667}, PropType::TREE, Textures},    // 19
            // Prop{Textures.TreeGreen, Vector2{1920,1667}, PropType::TREE, Textures},    // 20
            // Prop{Textures.TreeGreen, Vector2{2020,1667}, PropType::TREE, Textures},    // 21
            // Prop{Textures.TreeGreen, Vector2{2120,1667}, PropType::TREE, Textures},    // 22
            // Prop{Textures.TreeGreen, Vector2{2220,1667}, PropType::TREE, Textures},    // 23
            // Prop{Textures.TreeGreen, Vector2{2320,1667}, PropType::TREE, Textures},    // 24
            // Prop{Textures.TreeGreen, Vector2{2420,1667}, PropType::TREE, Textures},    // 25
            // Prop{Textures.TreeGreen, Vector2{2520,1667}, PropType::TREE, Textures},    // 26
            // Prop{Textures.TreeGreen, Vector2{2620,1667}, PropType::TREE, Textures},    // 27
            // Prop{Textures.TreeGreen, Vector2{2720,1667}, PropType::TREE, Textures},    // 28
            // Prop{Textures.TreeGreen, Vector2{2820,1667}, PropType::TREE, Textures},    // 29
            Prop{Textures.TreeGreen, Vector2{2920,1667}, PropType::TREE, Textures},    // 30
            Prop{Textures.TreeGreen, Vector2{3020,1667}, PropType::TREE, Textures},    // 31
            Prop{Textures.TreeGreen, Vector2{3120,1667}, PropType::TREE, Textures},    // 32
            Prop{Textures.TreeGreen, Vector2{3220,1667}, PropType::TREE, Textures},    // 33
            Prop{Textures.TreeGreen, Vector2{3320,1667}, PropType::TREE, Textures},    // 34
            Prop{Textures.TreeGreen, Vector2{3420,1667}, PropType::TREE, Textures},    // 35

            Prop{Textures.TreeGreen, Vector2{3700,1667}, PropType::TREE, Textures},    // 36
            Prop{Textures.TreeGreen, Vector2{3800,1667}, PropType::TREE, Textures},    // 37
            Prop{Textures.TreeGreen, Vector2{3900,1667}, PropType::TREE, Textures},    // 38
            Prop{Textures.TreeGreen, Vector2{4000,1667}, PropType::TREE, Textures},    // 39

            // row18
            Prop{Textures.TreeGreen, Vector2{0,1767}, PropType::TREE, Textures},       // 1 
            Prop{Textures.TreeGreen, Vector2{100,1767}, PropType::TREE, Textures},     // 2 
            Prop{Textures.TreeGreen, Vector2{200,1767}, PropType::TREE, Textures},     // 3 
            Prop{Textures.TreeGreen, Vector2{300,1767}, PropType::TREE, Textures},     // 4 
            Prop{Textures.TreeGreen, Vector2{400,1767}, PropType::TREE, Textures},     // 5 
            Prop{Textures.TreeGreen, Vector2{500,1767}, PropType::TREE, Textures},     // 6 
            Prop{Textures.TreeGreen, Vector2{600,1767}, PropType::TREE, Textures},     // 7 
            Prop{Textures.TreeGreen, Vector2{700,1767}, PropType::TREE, Textures},     // 8 
            Prop{Textures.TreeGreen, Vector2{800,1767}, PropType::TREE, Textures},     // 9 
            Prop{Textures.TreeGreen, Vector2{900,1767}, PropType::TREE, Textures},     // 10
            Prop{Textures.TreeGreen, Vector2{1000,1767}, PropType::TREE, Textures},    // 11
            Prop{Textures.TreeGreen, Vector2{1100,1767}, PropType::TREE, Textures},    // 12
            Prop{Textures.TreeGreen, Vector2{1200,1767}, PropType::TREE, Textures},    // 13
            Prop{Textures.TreeGreen, Vector2{1300,1767}, PropType::TREE, Textures},    // 14
            Prop{Textures.TreeGreen, Vector2{1400,1767}, PropType::TREE, Textures},    // 15
            // Prop{Textures.TreeGreen, Vector2{1500,1767}, PropType::TREE, Textures},    // 16
            // Prop{Textures.TreeGreen, Vector2{1600,1767}, PropType::TREE, Textures},    // 17
            // Prop{Textures.TreeGreen, Vector2{1700,1767}, PropType::TREE, Textures},    // 18
            // Prop{Textures.TreeGreen, Vector2{1800,1767}, PropType::TREE, Textures},    // 19
            // Prop{Textures.TreeGreen, Vector2{1900,1767}, PropType::TREE, Textures},    // 20
            // Prop{Textures.TreeGreen, Vector2{2000,1767}, PropType::TREE, Textures},    // 21
            // Prop{Textures.TreeGreen, Vector2{2100,1767}, PropType::TREE, Textures},    // 22
            // Prop{Textures.TreeGreen, Vector2{2200,1767}, PropType::TREE, Textures},    // 23
            // Prop{Textures.TreeGreen, Vector2{2300,1767}, PropType::TREE, Textures},    // 24
            // Prop{Textures.TreeGreen, Vector2{2400,1767}, PropType::TREE, Textures},    // 25
            Prop{Textures.TreeGreen, Vector2{2500,1767}, PropType::TREE, Textures},    // 26
            Prop{Textures.TreeGreen, Vector2{2600,1767}, PropType::TREE, Textures},    // 27
            Prop{Textures.TreeGreen, Vector2{2700,1767}, PropType::TREE, Textures},    // 28
            Prop{Textures.TreeGreen, Vector2{2800,1767}, PropType::TREE, Textures},    // 29
            Prop{Textures.TreeGreen, Vector2{2900,1767}, PropType::TREE, Textures},    // 30
            Prop{Textures.TreeGreen, Vector2{3000,1767}, PropType::TREE, Textures},    // 31
            Prop{Textures.TreeGreen, Vector2{3100,1767}, PropType::TREE, Textures},    // 32
            Prop{Textures.TreeGreen, Vector2{3200,1767}, PropType::TREE, Textures},    // 33
            Prop{Textures.TreeGreen, Vector2{3300,1767}, PropType::TREE, Textures},    // 34
            Prop{Textures.TreeGreen, Vector2{3400,1767}, PropType::TREE, Textures},    // 35

            Prop{Textures.TreeGreen, Vector2{3720,1767}, PropType::TREE, Textures},    // 36
            Prop{Textures.TreeGreen, Vector2{3820,1767}, PropType::TREE, Textures},    // 37
            Prop{Textures.TreeGreen, Vector2{3920,1767}, PropType::TREE, Textures},    // 38

            // row19
            Prop{Textures.TreeGreen, Vector2{20,1867}, PropType::TREE, Textures},      // 1     
            // Prop{Textures.TreeGreen, Vector2{120,1867}, PropType::TREE, Textures},     // 2     
            Prop{Textures.TreeGreen, Vector2{220,1867}, PropType::TREE, Textures},     // 3     
            Prop{Textures.TreeGreen, Vector2{320,1867}, PropType::TREE, Textures},     // 4 
            Prop{Textures.TreeGreen, Vector2{420,1867}, PropType::TREE, Textures},     // 5 
            Prop{Textures.TreeGreen, Vector2{520,1867}, PropType::TREE, Textures},     // 6 
            Prop{Textures.TreeGreen, Vector2{620,1867}, PropType::TREE, Textures},     // 7 
            // Prop{Textures.TreeGreen, Vector2{720,1867}, PropType::TREE, Textures},     // 8 
            Prop{Textures.TreeGreen, Vector2{820,1867}, PropType::TREE, Textures},     // 9 
            Prop{Textures.TreeGreen, Vector2{920,1867}, PropType::TREE, Textures},     // 10
            Prop{Textures.TreeGreen, Vector2{1020,1867}, PropType::TREE, Textures},    // 11
            Prop{Textures.TreeGreen, Vector2{1120,1867}, PropType::TREE, Textures},    // 12
            Prop{Textures.TreeGreen, Vector2{1220,1867}, PropType::TREE, Textures},    // 13
            Prop{Textures.TreeGreen, Vector2{1320,1867}, PropType::TREE, Textures},    // 14
            Prop{Textures.TreeGreen, Vector2{1420,1867}, PropType::TREE, Textures},    // 15
            Prop{Textures.TreeGreen, Vector2{1520,1867}, PropType::TREE, Textures},    // 16
            Prop{Textures.TreeGreen, Vector2{1620,1867}, PropType::TREE, Textures},    // 17
            // Prop{Textures.TreeGreen, Vector2{1720,1867}, PropType::TREE, Textures},    // 18
            // Prop{Textures.TreeGreen, Vector2{1820,1867}, PropType::TREE, Textures},    // 19
            // Prop{Textures.TreeGreen, Vector2{1920,1867}, PropType::TREE, Textures},    // 20
            // Prop{Textures.TreeGreen, Vector2{2020,1867}, PropType::TREE, Textures},    // 21
            // Prop{Textures.TreeGreen, Vector2{2120,1867}, PropType::TREE, Textures},    // 22
            // Prop{Textures.TreeGreen, Vector2{2220,1867}, PropType::TREE, Textures},    // 23
            // Prop{Textures.TreeGreen, Vector2{2320,1867}, PropType::TREE, Textures},    // 24
            // Prop{Textures.TreeGreen, Vector2{2420,1867}, PropType::TREE, Textures},    // 25
            // Prop{Textures.TreeGreen, Vector2{2520,1867}, PropType::TREE, Textures},    // 26
            // Prop{Textures.TreeGreen, Vector2{2620,1867}, PropType::TREE, Textures},    // 27
            Prop{Textures.TreeGreen, Vector2{2720,1867}, PropType::TREE, Textures},    // 28
            Prop{Textures.TreeGreen, Vector2{2820,1867}, PropType::TREE, Textures},    // 29
            Prop{Textures.TreeGreen, Vector2{2920,1867}, PropType::TREE, Textures},    // 30
            Prop{Textures.TreeGreen, Vector2{3020,1867}, PropType::TREE, Textures},    // 31
            Prop{Textures.TreeGreen, Vector2{3120,1867}, PropType::TREE, Textures},    // 32
            Prop{Textures.TreeGreen, Vector2{3220,1867}, PropType::TREE, Textures},    // 33
            Prop{Textures.TreeGreen, Vector2{3320,1867}, PropType::TREE, Textures},    // 34
            Prop{Textures.TreeGreen, Vector2{3420,1867}, PropType::TREE, Textures},    // 35

            Prop{Textures.TreeGreen, Vector2{3700,1867}, PropType::TREE, Textures},    // 36
            Prop{Textures.TreeGreen, Vector2{3800,1867}, PropType::TREE, Textures},    // 37
            Prop{Textures.TreeGreen, Vector2{3900,1867}, PropType::TREE, Textures},    // 38
            Prop{Textures.TreeGreen, Vector2{4000,1867}, PropType::TREE, Textures},    // 39

            // row20
            Prop{Textures.TreeGreen, Vector2{0,1967}, PropType::TREE, Textures},       // 1     
            Prop{Textures.TreeGreen, Vector2{100,1967}, PropType::TREE, Textures},     // 2 
            Prop{Textures.TreeGreen, Vector2{200,1967}, PropType::TREE, Textures},     // 3 
            // Prop{Textures.TreeGreen, Vector2{300,1967}, PropType::TREE, Textures},     // 4 
            Prop{Textures.TreeGreen, Vector2{400,1967}, PropType::TREE, Textures},     // 5 
            Prop{Textures.TreeGreen, Vector2{500,1967}, PropType::TREE, Textures},     // 6 
            Prop{Textures.TreeGreen, Vector2{600,1967}, PropType::TREE, Textures},     // 7 
            Prop{Textures.TreeGreen, Vector2{700,1967}, PropType::TREE, Textures},     // 8 
            Prop{Textures.TreeGreen, Vector2{800,1967}, PropType::TREE, Textures},     // 9 
            // Prop{Textures.TreeGreen, Vector2{900,1967}, PropType::TREE, Textures},     // 10
            Prop{Textures.TreeGreen, Vector2{1000,1967}, PropType::TREE, Textures},    // 11
            Prop{Textures.TreeGreen, Vector2{1100,1967}, PropType::TREE, Textures},    // 12
            // Prop{Textures.TreeGreen, Vector2{1200,1967}, PropType::TREE, Textures},    // 13
            Prop{Textures.TreeGreen, Vector2{1300,1967}, PropType::TREE, Textures},    // 14
            Prop{Textures.TreeGreen, Vector2{1400,1967}, PropType::TREE, Textures},    // 15
            Prop{Textures.TreeGreen, Vector2{1500,1967}, PropType::TREE, Textures},    // 16
            Prop{Textures.TreeGreen, Vector2{1600,1967}, PropType::TREE, Textures},    // 17
            // Prop{Textures.TreeGreen, Vector2{1700,1967}, PropType::TREE, Textures},    // 18
            Prop{Textures.TreeGreen, Vector2{1800,1967}, PropType::TREE, Textures},    // 19
            Prop{Textures.TreeGreen, Vector2{1900,1967}, PropType::TREE, Textures},    // 20
            // Prop{Textures.TreeGreen, Vector2{2000,1967}, PropType::TREE, Textures},    // 21
            // Prop{Textures.TreeGreen, Vector2{2100,1967}, PropType::TREE, Textures},    // 22
            // Prop{Textures.TreeGreen, Vector2{2200,1967}, PropType::TREE, Textures},    // 23
            // Prop{Textures.TreeGreen, Vector2{2300,1967}, PropType::TREE, Textures},    // 24
            // Prop{Textures.TreeGreen, Vector2{2400,1967}, PropType::TREE, Textures},    // 25
            // Prop{Textures.TreeGreen, Vector2{2500,1967}, PropType::TREE, Textures},    // 26
            Prop{Textures.TreeGreen, Vector2{2600,1967}, PropType::TREE, Textures},    // 27
            Prop{Textures.TreeGreen, Vector2{2700,1967}, PropType::TREE, Textures},    // 28
            // Prop{Textures.TreeGreen, Vector2{2800,1967}, PropType::TREE, Textures},    // 29
            Prop{Textures.TreeGreen, Vector2{2900,1967}, PropType::TREE, Textures},    // 30
            Prop{Textures.TreeGreen, Vector2{3000,1967}, PropType::TREE, Textures},    // 31
            Prop{Textures.TreeGreen, Vector2{3100,1967}, PropType::TREE, Textures},    // 32
            Prop{Textures.TreeGreen, Vector2{3200,1967}, PropType::TREE, Textures},    // 33
            // Prop{Textures.TreeGreen, Vector2{3300,1967}, PropType::TREE, Textures},    // 34
            Prop{Textures.TreeGreen, Vector2{3400,1967}, PropType::TREE, Textures},    // 35

            Prop{Textures.TreeGreen, Vector2{3500,1967}, PropType::TREE, Textures},    // 35
            Prop{Textures.TreeGreen, Vector2{3600,1967}, PropType::TREE, Textures},    // 35

            Prop{Textures.TreeGreen, Vector2{3720,1967}, PropType::TREE, Textures},    // 36
            Prop{Textures.TreeGreen, Vector2{3820,1967}, PropType::TREE, Textures},    // 37
            Prop{Textures.TreeGreen, Vector2{3920,1967}, PropType::TREE, Textures},    // 38

            // row21
            Prop{Textures.TreeGreen, Vector2{20,2067}, PropType::TREE, Textures},      // 1 
            // Prop{Textures.TreeGreen, Vector2{120,2067}, PropType::TREE, Textures},     // 2 
            Prop{Textures.TreeGreen, Vector2{220,2067}, PropType::TREE, Textures},     // 3 
            Prop{Textures.TreeGreen, Vector2{320,2067}, PropType::TREE, Textures},     // 4 
            Prop{Textures.TreeGreen, Vector2{420,2067}, PropType::TREE, Textures},     // 5 
            Prop{Textures.TreeGreen, Vector2{520,2067}, PropType::TREE, Textures},     // 6 
            Prop{Textures.TreeGreen, Vector2{620,2067}, PropType::TREE, Textures},     // 7 
            Prop{Textures.TreeGreen, Vector2{720,2067}, PropType::TREE, Textures},     // 8 
            Prop{Textures.TreeGreen, Vector2{820,2067}, PropType::TREE, Textures},     // 9 
            Prop{Textures.TreeGreen, Vector2{920,2067}, PropType::TREE, Textures},     // 10
            Prop{Textures.TreeGreen, Vector2{1020,2067}, PropType::TREE, Textures},    // 11
            Prop{Textures.TreeGreen, Vector2{1120,2067}, PropType::TREE, Textures},    // 12
            // Prop{Textures.TreeGreen, Vector2{1220,2067}, PropType::TREE, Textures},    // 13
            Prop{Textures.TreeGreen, Vector2{1320,2067}, PropType::TREE, Textures},    // 14
            Prop{Textures.TreeGreen, Vector2{1420,2067}, PropType::TREE, Textures},    // 15
            Prop{Textures.TreeGreen, Vector2{1520,2067}, PropType::TREE, Textures},    // 16
            // Prop{Textures.TreeGreen, Vector2{1620,2067}, PropType::TREE, Textures},    // 17
            Prop{Textures.TreeGreen, Vector2{1720,2067}, PropType::TREE, Textures},    // 18
            Prop{Textures.TreeGreen, Vector2{1820,2067}, PropType::TREE, Textures},    // 19
            // Prop{Textures.TreeGreen, Vector2{1920,2067}, PropType::TREE, Textures},    // 20
            // Prop{Textures.TreeGreen, Vector2{2020,2067}, PropType::TREE, Textures},    // 21
            // Prop{Textures.TreeGreen, Vector2{2120,2067}, PropType::TREE, Textures},    // 22
            // Prop{Textures.TreeGreen, Vector2{2220,2067}, PropType::TREE, Textures},    // 23
            // Prop{Textures.TreeGreen, Vector2{2320,2067}, PropType::TREE, Textures},    // 24
            // Prop{Textures.TreeGreen, Vector2{2420,2067}, PropType::TREE, Textures},    // 25
            // Prop{Textures.TreeGreen, Vector2{2520,2067}, PropType::TREE, Textures},    // 26
            // Prop{Textures.TreeGreen, Vector2{2620,2067}, PropType::TREE, Textures},    // 27
            Prop{Textures.TreeGreen, Vector2{2720,2067}, PropType::TREE, Textures},    // 28
            Prop{Textures.TreeGreen, Vector2{2820,2067}, PropType::TREE, Textures},    // 29
            Prop{Textures.TreeGreen, Vector2{2920,2067}, PropType::TREE, Textures},    // 30
            // Prop{Textures.TreeGreen, Vector2{3020,2067}, PropType::TREE, Textures},    // 31
            // Prop{Textures.TreeGreen, Vector2{3120,2067}, PropType::TREE, Textures},    // 32
            Prop{Textures.TreeGreen, Vector2{3220,2067}, PropType::TREE, Textures},    // 33
            Prop{Textures.TreeGreen, Vector2{3320,2067}, PropType::TREE, Textures},    // 34
            Prop{Textures.TreeGreen, Vector2{3420,2067}, PropType::TREE, Textures},    // 35

            Prop{Textures.TreeGreen, Vector2{3520,2067}, PropType::TREE, Textures},    // 35
            Prop{Textures.TreeGreen, Vector2{3620,2067}, PropType::TREE, Textures},    // 35

            Prop{Textures.TreeGreen, Vector2{3700,2067}, PropType::TREE, Textures},    // 36
            Prop{Textures.TreeGreen, Vector2{3800,2067}, PropType::TREE, Textures},    // 37
            Prop{Textures.TreeGreen, Vector2{3900,2067}, PropType::TREE, Textures},    // 38
            Prop{Textures.TreeGreen, Vector2{4000,2067}, PropType::TREE, Textures},    // 39

            // row22
            Prop{Textures.TreeGreen, Vector2{0,2167}, PropType::TREE, Textures},       // 1 
            Prop{Textures.TreeGreen, Vector2{100,2167}, PropType::TREE, Textures},     // 2 
            Prop{Textures.TreeGreen, Vector2{200,2167}, PropType::TREE, Textures},     // 3 
            Prop{Textures.TreeGreen, Vector2{300,2167}, PropType::TREE, Textures},     // 4 
            Prop{Textures.TreeGreen, Vector2{400,2167}, PropType::TREE, Textures},     // 5 
            // Prop{Textures.TreeGreen, Vector2{500,2167}, PropType::TREE, Textures},     // 6 
            Prop{Textures.TreeGreen, Vector2{600,2167}, PropType::TREE, Textures},     // 7 
            Prop{Textures.TreeGreen, Vector2{700,2167}, PropType::TREE, Textures},     // 8 
            Prop{Textures.TreeGreen, Vector2{800,2167}, PropType::TREE, Textures},     // 9 
            Prop{Textures.TreeGreen, Vector2{900,2167}, PropType::TREE, Textures},     // 10
            Prop{Textures.TreeGreen, Vector2{1000,2167}, PropType::TREE, Textures},    // 11
            // Prop{Textures.TreeGreen, Vector2{1100,2167}, PropType::TREE, Textures},    // 12
            // Prop{Textures.TreeGreen, Vector2{1200,2167}, PropType::TREE, Textures},    // 13
            Prop{Textures.TreeGreen, Vector2{1300,2167}, PropType::TREE, Textures},    // 14
            Prop{Textures.TreeGreen, Vector2{1400,2167}, PropType::TREE, Textures},    // 15
            Prop{Textures.TreeGreen, Vector2{1500,2167}, PropType::TREE, Textures},    // 16
            Prop{Textures.TreeGreen, Vector2{1600,2167}, PropType::TREE, Textures},    // 17
            Prop{Textures.TreeGreen, Vector2{1700,2167}, PropType::TREE, Textures},    // 18
            Prop{Textures.TreeGreen, Vector2{1800,2167}, PropType::TREE, Textures},    // 19
            Prop{Textures.TreeGreen, Vector2{1900,2167}, PropType::TREE, Textures},    // 20
            // Prop{Textures.TreeGreen, Vector2{2000,2167}, PropType::TREE, Textures},    // 21
            // Prop{Textures.TreeGreen, Vector2{2100,2167}, PropType::TREE, Textures},    // 22
            // Prop{Textures.TreeGreen, Vector2{2200,2167}, PropType::TREE, Textures},    // 23
            // Prop{Textures.TreeGreen, Vector2{2300,2167}, PropType::TREE, Textures},    // 24
            // Prop{Textures.TreeGreen, Vector2{2400,2167}, PropType::TREE, Textures},    // 25
            // Prop{Textures.TreeGreen, Vector2{2500,2167}, PropType::TREE, Textures},    // 26
            Prop{Textures.TreeGreen, Vector2{2600,2167}, PropType::TREE, Textures},    // 27
            Prop{Textures.TreeGreen, Vector2{2700,2167}, PropType::TREE, Textures},    // 28
            Prop{Textures.TreeGreen, Vector2{2800,2167}, PropType::TREE, Textures},    // 29
            Prop{Textures.TreeGreen, Vector2{2900,2167}, PropType::TREE, Textures},    // 30
            Prop{Textures.TreeGreen, Vector2{3000,2167}, PropType::TREE, Textures},    // 31
            Prop{Textures.TreeGreen, Vector2{3100,2167}, PropType::TREE, Textures},    // 32
            Prop{Textures.TreeGreen, Vector2{3200,2167}, PropType::TREE, Textures},    // 33
            Prop{Textures.TreeGreen, Vector2{3300,2167}, PropType::TREE, Textures},    // 34
            Prop{Textures.TreeGreen, Vector2{3400,2167}, PropType::TREE, Textures},    // 35

            Prop{Textures.TreeGreen, Vector2{3500,2167}, PropType::TREE, Textures},    // 35
            Prop{Textures.TreeGreen, Vector2{3600,2167}, PropType::TREE, Textures},    // 35

            Prop{Textures.TreeGreen, Vector2{3720,2167}, PropType::TREE, Textures},    // 36
            Prop{Textures.TreeGreen, Vector2{3820,2167}, PropType::TREE, Textures},    // 37    
            Prop{Textures.TreeGreen, Vector2{3920,2167}, PropType::TREE, Textures},    // 38

            // row21
            Prop{Textures.TreeGreen, Vector2{20,2267}, PropType::TREE, Textures},      // 1
            Prop{Textures.TreeGreen, Vector2{120,2267}, PropType::TREE, Textures},     // 2
            Prop{Textures.TreeGreen, Vector2{220,2267}, PropType::TREE, Textures},     // 3   
            Prop{Textures.TreeGreen, Vector2{320,2267}, PropType::TREE, Textures},     // 4
            Prop{Textures.TreeGreen, Vector2{420,2267}, PropType::TREE, Textures},     // 5    
            Prop{Textures.TreeGreen, Vector2{520,2267}, PropType::TREE, Textures},     // 6
            Prop{Textures.TreeGreen, Vector2{620,2267}, PropType::TREE, Textures},     // 7
            Prop{Textures.TreeGreen, Vector2{720,2267}, PropType::TREE, Textures},     // 8
            Prop{Textures.TreeGreen, Vector2{820,2267}, PropType::TREE, Textures},     // 9
            Prop{Textures.TreeGreen, Vector2{920,2267}, PropType::TREE, Textures},     // 10
            Prop{Textures.TreeGreen, Vector2{1020,2267}, PropType::TREE, Textures},    // 11
            Prop{Textures.TreeGreen, Vector2{1120,2267}, PropType::TREE, Textures},    // 12
            Prop{Textures.TreeGreen, Vector2{1220,2267}, PropType::TREE, Textures},    // 13
            Prop{Textures.TreeGreen, Vector2{1320,2267}, PropType::TREE, Textures},    // 14
            Prop{Textures.TreeGreen, Vector2{1420,2267}, PropType::TREE, Textures},    // 15
            Prop{Textures.TreeGreen, Vector2{1520,2267}, PropType::TREE, Textures},    // 16
            Prop{Textures.TreeGreen, Vector2{1620,2267}, PropType::TREE, Textures},    // 17
            // Prop{Textures.TreeGreen, Vector2{1720,2267}, PropType::TREE, Textures}, // 18
            // Prop{Textures.TreeGreen, Vector2{1820,2267}, PropType::TREE, Textures}, // 19
            // Prop{Textures.TreeGreen, Vector2{1920,2267}, PropType::TREE, Textures}, // 20  
            // Prop{Textures.TreeGreen, Vector2{2020,2267}, PropType::TREE, Textures}, // 21
            // Prop{Textures.TreeGreen, Vector2{2120,2267}, PropType::TREE, Textures}, // 22
            // Prop{Textures.TreeGreen, Vector2{2220,2267}, PropType::TREE, Textures}, // 23
            // Prop{Textures.TreeGreen, Vector2{2320,2267}, PropType::TREE, Textures}, // 24
            // Prop{Textures.TreeGreen, Vector2{2420,2267}, PropType::TREE, Textures}, // 25
            // Prop{Textures.TreeGreen, Vector2{2520,2267}, PropType::TREE, Textures}, // 26
            Prop{Textures.TreeGreen, Vector2{2620,2267}, PropType::TREE, Textures},    // 27
            Prop{Textures.TreeGreen, Vector2{2720,2267}, PropType::TREE, Textures},    // 28
            Prop{Textures.TreeGreen, Vector2{2820,2267}, PropType::TREE, Textures},    // 29
            Prop{Textures.TreeGreen, Vector2{2920,2267}, PropType::TREE, Textures},    // 30
            Prop{Textures.TreeGreen, Vector2{3020,2267}, PropType::TREE, Textures},    // 31
            Prop{Textures.TreeGreen, Vector2{3120,2267}, PropType::TREE, Textures},    // 32
            Prop{Textures.TreeGreen, Vector2{3220,2267}, PropType::TREE, Textures},    // 33
            Prop{Textures.TreeGreen, Vector2{3320,2267}, PropType::TREE, Textures},    // 34
            Prop{Textures.TreeGreen, Vector2{3420,2267}, PropType::TREE, Textures},    // 35

            Prop{Textures.TreeGreen, Vector2{3520,2267}, PropType::TREE, Textures},    // 35
            Prop{Textures.TreeGreen, Vector2{3620,2267}, PropType::TREE, Textures},    // 35

            Prop{Textures.TreeGreen, Vector2{3700,2267}, PropType::TREE, Textures},    // 36
            Prop{Textures.TreeGreen, Vector2{3800,2267}, PropType::TREE, Textures},    // 37
            Prop{Textures.TreeGreen, Vector2{3900,2267}, PropType::TREE, Textures},    // 38
            Prop{Textures.TreeGreen, Vector2{4000,2267}, PropType::TREE, Textures},    // 39

            // row22
            Prop{Textures.TreeGreen, Vector2{50,2367}, PropType::TREE, Textures},      // 1 
            // Prop{Textures.TreeGreen, Vector2{100,2367}, PropType::TREE, Textures},  // 2 
            // Prop{Textures.TreeGreen, Vector2{200,2367}, PropType::TREE, Textures},  // 3 
            // Prop{Textures.TreeGreen, Vector2{300,2367}, PropType::TREE, Textures},  // 4 
            // Prop{Textures.TreeGreen, Vector2{400,2367}, PropType::TREE, Textures},  // 5 
            Prop{Textures.TreeClear, Vector2{550,2303}, PropType::TREE, Textures},     // 6 Invisible Wall
            Prop{Textures.TreeGreen, Vector2{550,2367}, PropType::TREE, Textures},     // 6 
            // Prop{Textures.TreeGreen, Vector2{600,2367}, PropType::TREE, Textures},  // 7     
            Prop{Textures.TreeGreen, Vector2{700,2367}, PropType::TREE, Textures},  // 8 
            Prop{Textures.TreeGreen, Vector2{800,2367}, PropType::TREE, Textures},  // 9 
            Prop{Textures.TreeGreen, Vector2{900,2367}, PropType::TREE, Textures},  // 10
            Prop{Textures.TreeGreen, Vector2{1000,2367}, PropType::TREE, Textures}, // 11
            Prop{Textures.TreeGreen, Vector2{1100,2367}, PropType::TREE, Textures}, // 12
            Prop{Textures.TreeGreen, Vector2{1200,2367}, PropType::TREE, Textures}, // 13
            Prop{Textures.TreeGreen, Vector2{1300,2367}, PropType::TREE, Textures}, // 14
            Prop{Textures.TreeGreen, Vector2{1400,2367}, PropType::TREE, Textures}, // 15
            Prop{Textures.TreeGreen, Vector2{1500,2367}, PropType::TREE, Textures}, // 16
            Prop{Textures.TreeGreen, Vector2{1600,2367}, PropType::TREE, Textures}, // 17
            Prop{Textures.TreeGreen, Vector2{1700,2367}, PropType::TREE, Textures}, // 18
            Prop{Textures.TreeGreen, Vector2{1800,2367}, PropType::TREE, Textures}, // 19
            // Prop{Textures.TreeGreen, Vector2{1900,2367}, PropType::TREE, Textures}, // 20
            // Prop{Textures.TreeGreen, Vector2{2000,2367}, PropType::TREE, Textures}, // 21
            // Prop{Textures.TreeGreen, Vector2{2100,2367}, PropType::TREE, Textures}, // 22
            // Prop{Textures.TreeGreen, Vector2{2200,2367}, PropType::TREE, Textures}, // 23
            // Prop{Textures.TreeGreen, Vector2{2300,2367}, PropType::TREE, Textures}, // 24
            // Prop{Textures.TreeGreen, Vector2{2400,2367}, PropType::TREE, Textures}, // 25
            // Prop{Textures.TreeGreen, Vector2{2500,2367}, PropType::TREE, Textures}, // 26
            Prop{Textures.TreeGreen, Vector2{2600,2340}, PropType::TREE, Textures}, // 27
            Prop{Textures.TreeGreen, Vector2{2700,2340}, PropType::TREE, Textures}, // 28
            Prop{Textures.TreeGreen, Vector2{2800,2340}, PropType::TREE, Textures}, // 29
            Prop{Textures.TreeGreen, Vector2{2900,2340}, PropType::TREE, Textures}, // 30
            Prop{Textures.TreeGreen, Vector2{3000,2340}, PropType::TREE, Textures}, // 31
            Prop{Textures.TreeGreen, Vector2{3100,2340}, PropType::TREE, Textures}, // 32
            Prop{Textures.TreeGreen, Vector2{3200,2340}, PropType::TREE, Textures}, // 33
            Prop{Textures.TreeGreen, Vector2{3300,2340}, PropType::TREE, Textures}, // 34
            Prop{Textures.TreeGreen, Vector2{3400,2340}, PropType::TREE, Textures}, // 35

            Prop{Textures.TreeGreen, Vector2{3500,2340}, PropType::TREE, Textures}, // 36
            Prop{Textures.TreeGreen, Vector2{3600,2340}, PropType::TREE, Textures}, // 37

            Prop{Textures.TreeGreen, Vector2{3720,2340}, PropType::TREE, Textures}, // 38
            Prop{Textures.TreeGreen, Vector2{3820,2340}, PropType::TREE, Textures}, // 39
            Prop{Textures.TreeGreen, Vector2{3920,2340}, PropType::TREE, Textures}, // 40

            // row23
            Prop{Textures.TreeGreen, Vector2{20,2467}, PropType::TREE, Textures},      // 1 
            // Prop{Textures.TreeGreen, Vector2{120,2467}, PropType::TREE, Textures},  // 2 
            // Prop{Textures.TreeGreen, Vector2{220,2467}, PropType::TREE, Textures},  // 3 
            // Prop{Textures.TreeGreen, Vector2{320,2467}, PropType::TREE, Textures},  // 4 
            // Prop{Textures.TreeGreen, Vector2{420,2467}, PropType::TREE, Textures},  // 5 
            // Prop{Textures.TreeGreen, Vector2{520,2467}, PropType::TREE, Textures},  // 6 
            Prop{Textures.TreeClear, Vector2{620,2403}, PropType::TREE, Textures},     // 7 Invisible Wall
            Prop{Textures.TreeGreen, Vector2{620,2467}, PropType::TREE, Textures},     // 7 
            Prop{Textures.TreeGreen, Vector2{720,2467}, PropType::TREE, Textures},     // 8 
            Prop{Textures.TreeGreen, Vector2{820,2467}, PropType::TREE, Textures},     // 9 
            Prop{Textures.TreeGreen, Vector2{920,2467}, PropType::TREE, Textures},     // 10
            Prop{Textures.TreeGreen, Vector2{1020,2467}, PropType::TREE, Textures},    // 11    
            Prop{Textures.TreeGreen, Vector2{1120,2467}, PropType::TREE, Textures},    // 12    
            Prop{Textures.TreeGreen, Vector2{1220,2467}, PropType::TREE, Textures},    // 13
            Prop{Textures.TreeGreen, Vector2{1320,2467}, PropType::TREE, Textures},    // 14
            Prop{Textures.TreeGreen, Vector2{1420,2467}, PropType::TREE, Textures},    // 15
            Prop{Textures.TreeGreen, Vector2{1520,2467}, PropType::TREE, Textures},    // 16
            Prop{Textures.TreeGreen, Vector2{1620,2467}, PropType::TREE, Textures},    // 17
            Prop{Textures.TreeGreen, Vector2{1720,2467}, PropType::TREE, Textures},    // 18
            Prop{Textures.TreeGreen, Vector2{1820,2467}, PropType::TREE, Textures},    // 19
            Prop{Textures.TreeGreen, Vector2{1920,2467}, PropType::TREE, Textures},    // 20
            // Prop{Textures.TreeGreen, Vector2{2020,2467}, PropType::TREE, Textures}, // 21
            // Prop{Textures.TreeGreen, Vector2{2120,2467}, PropType::TREE, Textures}, // 22
            // Prop{Textures.TreeGreen, Vector2{2220,2467}, PropType::TREE, Textures}, // 23
            // Prop{Textures.TreeGreen, Vector2{2320,2467}, PropType::TREE, Textures}, // 24
            // Prop{Textures.TreeGreen, Vector2{2420,2467}, PropType::TREE, Textures}, // 25
            // Prop{Textures.TreeGreen, Vector2{2520,2467}, PropType::TREE, Textures}, // 26
            // Prop{Textures.TreeGreen, Vector2{2620,2467}, PropType::TREE, Textures}, // 27
            // Prop{Textures.TreeGreen, Vector2{2720,2467}, PropType::TREE, Textures}, // 28
            // Prop{Textures.TreeGreen, Vector2{2820,2467}, PropType::TREE, Textures}, // 29
            // Prop{Textures.TreeGreen, Vector2{2920,2467}, PropType::TREE, Textures}, // 30
            // Prop{Textures.TreeGreen, Vector2{3020,2467}, PropType::TREE, Textures}, // 31
            // Prop{Textures.TreeGreen, Vector2{3120,2467}, PropType::TREE, Textures}, // 32
            // Prop{Textures.TreeGreen, Vector2{3220,2467}, PropType::TREE, Textures}, // 33
            // Prop{Textures.TreeGreen, Vector2{3320,2467}, PropType::TREE, Textures}, // 34
            // Prop{Textures.TreeGreen, Vector2{3420,2467}, PropType::TREE, Textures}, // 35

            // Prop{Textures.TreeGreen, Vector2{3700,2467}, PropType::TREE, Textures}, // 36
            // Prop{Textures.TreeGreen, Vector2{3800,2467}, PropType::TREE, Textures}, // 37
            // Prop{Textures.TreeGreen, Vector2{3900,2467}, PropType::TREE, Textures}, // 38
            // Prop{Textures.TreeGreen, Vector2{4000,2467}, PropType::TREE, Textures}, // 39

            // row24
            Prop{Textures.TreeGreen, Vector2{50,2567}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{150,2567}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{250,2567}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{350,2567}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{450,2567}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{550,2567}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{650,2567}, PropType::TREE, Textures},

            Prop{Textures.TreeGreen, Vector2{1980,2567}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{2100,2567}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{2200,2567}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{2490,2567}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{2590,2567}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{2700,2567}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{2800,2567}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{2900,2567}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{3000,2567}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{3100,2567}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{3200,2567}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{3300,2567}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{3400,2567}, PropType::TREE, Textures},

            // Prop{Textures.TreeGreen, Vector2{3720,2567}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{3820,2567}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{3920,2567}, PropType::TREE, Textures},

            // row25
            Prop{Textures.TreeGreen, Vector2{20,2667}, PropType::TREE, Textures}, 
            // Prop{Textures.TreeGreen, Vector2{120,2667}, PropType::TREE, Textures}, 
            // Prop{Textures.TreeGreen, Vector2{220,2667}, PropType::TREE, Textures}, 
            // Prop{Textures.TreeGreen, Vector2{320,2667}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{420,2667}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{520,2667}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{620,2667}, PropType::TREE, Textures},

            // Prop{Textures.TreeGreen, Vector2{1920,2667}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{2020,2667}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{2120,2667}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{2220,2667}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{2320,2667}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{2420,2667}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{2520,2667}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{2620,2667}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{2720,2667}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{2820,2667}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{2920,2667}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{3020,2667}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{3120,2667}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{3220,2667}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{3320,2667}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{3420,2667}, PropType::TREE, Textures},

            // Prop{Textures.TreeGreen, Vector2{3700,2667}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{3800,2667}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{3900,2667}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{4000,2667}, PropType::TREE, Textures},

            // row26
            Prop{Textures.TreeGreen, Vector2{50,2767}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{100,2767}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{250,2767}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{350,2767}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{450,2767}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{550,2767}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{650,2767}, PropType::TREE, Textures},

            Prop{Textures.TreeGreen, Vector2{1980,2767}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{2100,2767}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{2200,2767}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{2490,2767}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{2590,2767}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{2700,2767}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{2800,2767}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{2900,2767}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{3000,2767}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{3100,2767}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{3200,2767}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{3300,2767}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{3400,2767}, PropType::TREE, Textures},

            // Prop{Textures.TreeGreen, Vector2{3720,2767}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{3820,2767}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{3920,2767}, PropType::TREE, Textures},

            // row27
            Prop{Textures.TreeGreen, Vector2{20,2867}, PropType::TREE, Textures}, 
            Prop{Textures.TreeGreen, Vector2{120,2867}, PropType::TREE, Textures}, 
            // Prop{Textures.TreeGreen, Vector2{220,2867}, PropType::TREE, Textures}, 
            // Prop{Textures.TreeGreen, Vector2{320,2867}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{420,2867}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{520,2867}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{620,2867}, PropType::TREE, Textures},

            // Prop{Textures.TreeGreen, Vector2{1920,2867}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{2020,2867}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{2120,2867}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{2220,2867}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{2320,2867}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{2420,2867}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{2520,2867}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{2620,2867}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{2720,2867}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{2820,2867}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{2920,2867}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{3020,2867}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{3120,2867}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{3220,2867}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{3320,2867}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{3420,2867}, PropType::TREE, Textures},

            // Prop{Textures.TreeGreen, Vector2{3700,2867}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{3800,2867}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{3900,2867}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{4000,2867}, PropType::TREE, Textures},

            // row28
            Prop{Textures.TreeGreen, Vector2{50,2967}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{150,2967}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{250,2967}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{350,2967}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{450,2967}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{550,2967}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{650,2967}, PropType::TREE, Textures},

            Prop{Textures.TreeGreen, Vector2{1980,2967}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{2100,2967}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{2200,2967}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{2490,2967}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{2590,2967}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{2700,2967}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{2800,2967}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{2900,2967}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{3000,2967}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{3100,2967}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{3200,2967}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{3300,2967}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{3400,2967}, PropType::TREE, Textures},

            // Prop{Textures.TreeGreen, Vector2{3720,2967}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{3820,2967}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{3920,2967}, PropType::TREE, Textures},

            // row29
            Prop{Textures.TreeGreen, Vector2{20,3067}, PropType::TREE, Textures}, 
            Prop{Textures.TreeGreen, Vector2{120,3067}, PropType::TREE, Textures}, 
            Prop{Textures.TreeGreen, Vector2{220,3067}, PropType::TREE, Textures}, 
            // Prop{Textures.TreeGreen, Vector2{320,3067}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{420,3067}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{520,3067}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{620,3067}, PropType::TREE, Textures},

            // Prop{Textures.TreeGreen, Vector2{1920,3067}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{2020,3067}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{2120,3067}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{2220,3067}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{2320,3067}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{2420,3067}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{2520,3067}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{2620,3067}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{2720,3067}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{2820,3067}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{2920,3067}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{3020,3067}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{3120,3067}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{3220,3067}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{3320,3067}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{3420,3067}, PropType::TREE, Textures},

            // Prop{Textures.TreeGreen, Vector2{3700,3067}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{3800,3067}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{3900,3067}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{4000,3067}, PropType::TREE, Textures},

            // row30
            Prop{Textures.TreeGreen, Vector2{50,3167}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{150,3167}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{250,3167}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{350,3167}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{450,3167}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{550,3167}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{650,3167}, PropType::TREE, Textures},

            Prop{Textures.TreeGreen, Vector2{1980,3167}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{2100,3167}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{2200,3167}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{2490,3167}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{2590,3167}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{2700,3167}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{2800,3167}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{2900,3167}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{3000,3167}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{3100,3167}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{3200,3167}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{3300,3167}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{3400,3167}, PropType::TREE, Textures},

            // Prop{Textures.TreeGreen, Vector2{3720,3167}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{3820,3167}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{3920,3167}, PropType::TREE, Textures},

            // row31
            Prop{Textures.TreeGreen, Vector2{20,3267}, PropType::TREE, Textures}, 
            Prop{Textures.TreeGreen, Vector2{120,3267}, PropType::TREE, Textures}, 
            Prop{Textures.TreeGreen, Vector2{220,3267}, PropType::TREE, Textures}, 
            // Prop{Textures.TreeGreen, Vector2{320,3267}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{420,3267}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{530,3267}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{620,3267}, PropType::TREE, Textures},

            // Prop{Textures.TreeGreen, Vector2{1920,3267}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{2020,3267}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{2120,3267}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{2220,3267}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{2320,3267}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{2420,3267}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{2520,3267}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{2620,3267}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{2720,3267}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{2820,3267}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{2920,3267}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{3020,3267}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{3120,3267}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{3220,3267}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{3320,3267}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{3420,3267}, PropType::TREE, Textures},

            // Prop{Textures.TreeGreen, Vector2{3700,3267}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{3800,3267}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{3900,3267}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{4000,3267}, PropType::TREE, Textures},

            // row32
            Prop{Textures.TreeGreen, Vector2{50,3367}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{150,3367}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{250,3367}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{350,3367}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{450,3367}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{550,3367}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{650,3367}, PropType::TREE, Textures},

            // Prop{Textures.TreeGreen, Vector2{1980,3367}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{2100,3367}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{2200,3367}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{2490,3367}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{2590,3367}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{2700,3367}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{2800,3367}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{2900,3367}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{3000,3367}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{3100,3367}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{3200,3367}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{3300,3367}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{3400,3367}, PropType::TREE, Textures},

            // Prop{Textures.TreeGreen, Vector2{3720,3367}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{3820,3367}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{3920,3367}, PropType::TREE, Textures},

            // row33
            Prop{Textures.TreeGreen, Vector2{20,3467}, PropType::TREE, Textures},      
            Prop{Textures.TreeGreen, Vector2{120,3467}, PropType::TREE, Textures},         
            Prop{Textures.TreeGreen, Vector2{220,3467}, PropType::TREE, Textures},         
            Prop{Textures.TreeGreen, Vector2{320,3467}, PropType::TREE, Textures},     
            Prop{Textures.TreeGreen, Vector2{420,3467}, PropType::TREE, Textures},     
            // Prop{Textures.TreeGreen, Vector2{520,3467}, PropType::TREE, Textures},      
            Prop{Textures.TreeGreen, Vector2{620,3467}, PropType::TREE, Textures},     

            // Prop{Textures.TreeGreen, Vector2{2020,3467}, PropType::TREE, Textures},     
            // Prop{Textures.TreeGreen, Vector2{2120,3467}, PropType::TREE, Textures},     
            // Prop{Textures.TreeGreen, Vector2{2220,3467}, PropType::TREE, Textures},     
            // Prop{Textures.TreeGreen, Vector2{2320,3467}, PropType::TREE, Textures},     
            // Prop{Textures.TreeGreen, Vector2{2420,3467}, PropType::TREE, Textures},     
            // Prop{Textures.TreeGreen, Vector2{2520,3467}, PropType::TREE, Textures},     
            // Prop{Textures.TreeGreen, Vector2{2620,3467}, PropType::TREE, Textures},     
            // Prop{Textures.TreeGreen, Vector2{2720,3467}, PropType::TREE, Textures},     
            // Prop{Textures.TreeGreen, Vector2{2820,3467}, PropType::TREE, Textures},     
            // Prop{Textures.TreeGreen, Vector2{2920,3467}, PropType::TREE, Textures},     
            // Prop{Textures.TreeGreen, Vector2{3020,3467}, PropType::TREE, Textures},     
            // Prop{Textures.TreeGreen, Vector2{3120,3467}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{3220,3467}, PropType::TREE, Textures},     
            // Prop{Textures.TreeGreen, Vector2{3320,3467}, PropType::TREE, Textures},     
            // Prop{Textures.TreeGreen, Vector2{3420,3467}, PropType::TREE, Textures},     

            // Prop{Textures.TreeGreen, Vector2{3700,3467}, PropType::TREE, Textures}, 
            // Prop{Textures.TreeGreen, Vector2{3800,3467}, PropType::TREE, Textures}, 
            // Prop{Textures.TreeGreen, Vector2{3900,3467}, PropType::TREE, Textures}, 
            Prop{Textures.TreeGreen, Vector2{4000,3467}, PropType::TREE, Textures},    

            // row34
            Prop{Textures.TreeGreen, Vector2{0,3567}, PropType::TREE, Textures},       // 1 
            Prop{Textures.TreeGreen, Vector2{150,3567}, PropType::TREE, Textures},     // 2 
            Prop{Textures.TreeGreen, Vector2{250,3567}, PropType::TREE, Textures},     // 3 
            Prop{Textures.TreeGreen, Vector2{350,3567}, PropType::TREE, Textures},     // 4 
            // Prop{Textures.TreeGreen, Vector2{450,3567}, PropType::TREE, Textures},  // 5 
            Prop{Textures.TreeGreen, Vector2{665,3530}, PropType::TREE, Textures},     // 6 

            // Prop{Textures.TreeGreen, Vector2{2100,3567}, PropType::TREE, Textures}, // 7 
            // Prop{Textures.TreeGreen, Vector2{2200,3567}, PropType::TREE, Textures}, // 8 
            // Prop{Textures.TreeGreen, Vector2{2490,3567}, PropType::TREE, Textures}, // 9 
            // Prop{Textures.TreeGreen, Vector2{2590,3567}, PropType::TREE, Textures}, // 10
            // Prop{Textures.TreeGreen, Vector2{2700,3567}, PropType::TREE, Textures}, // 11
            // Prop{Textures.TreeGreen, Vector2{2800,3567}, PropType::TREE, Textures}, // 12
            // Prop{Textures.TreeGreen, Vector2{2900,3567}, PropType::TREE, Textures}, // 13
            // Prop{Textures.TreeGreen, Vector2{3000,3567}, PropType::TREE, Textures}, // 14
            // Prop{Textures.TreeGreen, Vector2{3100,3567}, PropType::TREE, Textures}, // 15 
            // Prop{Textures.TreeGreen, Vector2{3200,3567}, PropType::TREE, Textures}, // 16
            // Prop{Textures.TreeGreen, Vector2{3300,3567}, PropType::TREE, Textures}, // 17
            // Prop{Textures.TreeGreen, Vector2{3400,3567}, PropType::TREE, Textures}, // 18

            // Prop{Textures.TreeGreen, Vector2{3720,3567}, PropType::TREE, Textures}, // 19 
            // Prop{Textures.TreeGreen, Vector2{3820,3567}, PropType::TREE, Textures}, // 20
            Prop{Textures.TreeGreen, Vector2{3920,3567}, PropType::TREE, Textures},    // 21

            // row35
            Prop{Textures.TreeGreen, Vector2{100,3740}, PropType::TREE, Textures}, 
            Prop{Textures.TreeGreen, Vector2{200,3740}, PropType::TREE, Textures}, 
            Prop{Textures.TreeGreen, Vector2{300,3740}, PropType::TREE, Textures},

            Prop{Textures.TreeGreen, Vector2{500,3740}, PropType::TREE, Textures},

            Prop{Textures.TreeGreen, Vector2{800,3740}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{900,3740}, PropType::TREE, Textures},

            Prop{Textures.TreeGreen, Vector2{1600,3740}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{1700,3740}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{1800,3740}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{1900,3740}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{2000,3740}, PropType::TREE, Textures},

            Prop{Textures.TreeGreen, Vector2{2200,3740}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{2300,3740}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{2400,3740}, PropType::TREE, Textures},

            Prop{Textures.TreeGreen, Vector2{2600,3740}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{2700,3740}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{2800,3740}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{2900,3740}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{3000,3740}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{3100,3740}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{3200,3740}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{3300,3740}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{3400,3740}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{3500,3740}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{3600,3740}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{3700,3740}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{3800,3740}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{3900,3740}, PropType::TREE, Textures},
            // Prop{Textures.TreeGreen, Vector2{4000,3740}, PropType::TREE, Textures},

            // row36
            Prop{Textures.TreeGreen, Vector2{20,3840}, PropType::TREE, Textures},      // 1 
            Prop{Textures.TreeGreen, Vector2{120,3840}, PropType::TREE, Textures},     // 2 
            Prop{Textures.TreeGreen, Vector2{220,3840}, PropType::TREE, Textures},     // 3 
            // Prop{Textures.TreeGreen, Vector2{320,3840}, PropType::TREE, Textures},  // 4 
            Prop{Textures.TreeGreen, Vector2{420,3840}, PropType::TREE, Textures},     // 5 
            Prop{Textures.TreeGreen, Vector2{520,3840}, PropType::TREE, Textures},     // 6 
            // Prop{Textures.TreeGreen, Vector2{620,3840}, PropType::TREE, Textures},  // 7 
            Prop{Textures.TreeGreen, Vector2{720,3840}, PropType::TREE, Textures},     // 8 
            Prop{Textures.TreeGreen, Vector2{820,3840}, PropType::TREE, Textures},     // 9 
            // Prop{Textures.TreeGreen, Vector2{920,3840}, PropType::TREE, Textures},  // 10
            Prop{Textures.TreeGreen, Vector2{1020,3840}, PropType::TREE, Textures},    // 11
            Prop{Textures.TreeGreen, Vector2{1120,3840}, PropType::TREE, Textures},    // 12
            Prop{Textures.TreeGreen, Vector2{1220,3840}, PropType::TREE, Textures},    // 13
            // Prop{Textures.TreeGreen, Vector2{1320,3840}, PropType::TREE, Textures}, // 14    
            Prop{Textures.TreeGreen, Vector2{1420,3840}, PropType::TREE, Textures},    // 15
            Prop{Textures.TreeGreen, Vector2{1520,3840}, PropType::TREE, Textures},    // 16
            Prop{Textures.TreeGreen, Vector2{1620,3840}, PropType::TREE, Textures},    // 17
            Prop{Textures.TreeGreen, Vector2{1720,3840}, PropType::TREE, Textures},    // 18
            // Prop{Textures.TreeGreen, Vector2{1820,3840}, PropType::TREE, Textures}, // 19    
            Prop{Textures.TreeGreen, Vector2{1920,3840}, PropType::TREE, Textures},    // 20
            Prop{Textures.TreeGreen, Vector2{2020,3840}, PropType::TREE, Textures},    // 21
            Prop{Textures.TreeGreen, Vector2{2120,3840}, PropType::TREE, Textures},    // 22
            // Prop{Textures.TreeGreen, Vector2{2220,3840}, PropType::TREE, Textures}, // 23    
            Prop{Textures.TreeGreen, Vector2{2320,3840}, PropType::TREE, Textures},    // 24
            Prop{Textures.TreeGreen, Vector2{2420,3840}, PropType::TREE, Textures},    // 25
            Prop{Textures.TreeGreen, Vector2{2520,3840}, PropType::TREE, Textures},    // 26
            // Prop{Textures.TreeGreen, Vector2{2620,3840}, PropType::TREE, Textures}, // 27    
            Prop{Textures.TreeGreen, Vector2{2720,3840}, PropType::TREE, Textures},    // 28
            Prop{Textures.TreeGreen, Vector2{2820,3840}, PropType::TREE, Textures},    // 29
            Prop{Textures.TreeGreen, Vector2{2920,3840}, PropType::TREE, Textures},    // 30
            Prop{Textures.TreeGreen, Vector2{3020,3840}, PropType::TREE, Textures},    // 31
            Prop{Textures.TreeGreen, Vector2{3120,3840}, PropType::TREE, Textures},    // 32
            // Prop{Textures.TreeGreen, Vector2{3220,3840}, PropType::TREE, Textures}, // 33    
            // Prop{Textures.TreeGreen, Vector2{3320,3840}, PropType::TREE, Textures}, // 34    
            Prop{Textures.TreeGreen, Vector2{3420,3840}, PropType::TREE, Textures},    // 35
            Prop{Textures.TreeGreen, Vector2{3520,3840}, PropType::TREE, Textures},    // 36
            // Prop{Textures.TreeGreen, Vector2{3620,3840}, PropType::TREE, Textures}, // 37    
            Prop{Textures.TreeGreen, Vector2{3720,3840}, PropType::TREE, Textures},    // 38
            Prop{Textures.TreeGreen, Vector2{3820,3840}, PropType::TREE, Textures},    // 39
            Prop{Textures.TreeGreen, Vector2{3920,3840}, PropType::TREE, Textures},    // 40

            // row37
            Prop{Textures.TreeGreen, Vector2{0,3940}, PropType::TREE, Textures},       // 1 
            Prop{Textures.TreeGreen, Vector2{100,3940}, PropType::TREE, Textures},     // 2 
            Prop{Textures.TreeGreen, Vector2{200,3940}, PropType::TREE, Textures},     // 3 
            Prop{Textures.TreeGreen, Vector2{300,3940}, PropType::TREE, Textures},     // 4  
            Prop{Textures.TreeGreen, Vector2{400,3940}, PropType::TREE, Textures},     // 5 
            Prop{Textures.TreeGreen, Vector2{500,3940}, PropType::TREE, Textures},     // 6 
            Prop{Textures.TreeGreen, Vector2{600,3940}, PropType::TREE, Textures},     // 7 
            Prop{Textures.TreeGreen, Vector2{700,3940}, PropType::TREE, Textures},     // 8 
            Prop{Textures.TreeGreen, Vector2{800,3940}, PropType::TREE, Textures},     // 9 
            Prop{Textures.TreeGreen, Vector2{900,3940}, PropType::TREE, Textures},     // 10
            Prop{Textures.TreeGreen, Vector2{1000,3940}, PropType::TREE, Textures},    // 11  
            Prop{Textures.TreeGreen, Vector2{1100,3940}, PropType::TREE, Textures},    // 12
            Prop{Textures.TreeGreen, Vector2{1200,3940}, PropType::TREE, Textures},    // 13
            Prop{Textures.TreeGreen, Vector2{1300,3940}, PropType::TREE, Textures},    // 14
            Prop{Textures.TreeGreen, Vector2{1400,3940}, PropType::TREE, Textures},    // 15
            Prop{Textures.TreeGreen, Vector2{1500,3940}, PropType::TREE, Textures},    // 16
            Prop{Textures.TreeGreen, Vector2{1600,3940}, PropType::TREE, Textures},    // 17
            Prop{Textures.TreeGreen, Vector2{1700,3940}, PropType::TREE, Textures},    // 18
            Prop{Textures.TreeGreen, Vector2{1800,3940}, PropType::TREE, Textures},    // 19
            Prop{Textures.TreeGreen, Vector2{1900,3940}, PropType::TREE, Textures},    // 20
            Prop{Textures.TreeGreen, Vector2{2000,3940}, PropType::TREE, Textures},    // 21
            Prop{Textures.TreeGreen, Vector2{2100,3940}, PropType::TREE, Textures},    // 22
            Prop{Textures.TreeGreen, Vector2{2200,3940}, PropType::TREE, Textures},    // 23
            Prop{Textures.TreeGreen, Vector2{2300,3940}, PropType::TREE, Textures},    // 24
            Prop{Textures.TreeGreen, Vector2{2400,3940}, PropType::TREE, Textures},    // 25
            Prop{Textures.TreeGreen, Vector2{2500,3940}, PropType::TREE, Textures},    // 26
            Prop{Textures.TreeGreen, Vector2{2600,3940}, PropType::TREE, Textures},    // 27 
            Prop{Textures.TreeGreen, Vector2{2700,3940}, PropType::TREE, Textures},    // 28
            Prop{Textures.TreeGreen, Vector2{2800,3940}, PropType::TREE, Textures},    // 29
            Prop{Textures.TreeGreen, Vector2{2900,3940}, PropType::TREE, Textures},    // 30
            Prop{Textures.TreeGreen, Vector2{3000,3940}, PropType::TREE, Textures},    // 31
            Prop{Textures.TreeGreen, Vector2{3100,3940}, PropType::TREE, Textures},    // 32
            Prop{Textures.TreeGreen, Vector2{3200,3940}, PropType::TREE, Textures},    // 33
            Prop{Textures.TreeGreen, Vector2{3300,3940}, PropType::TREE, Textures},    // 34
            Prop{Textures.TreeGreen, Vector2{3400,3940}, PropType::TREE, Textures},    // 35 
            Prop{Textures.TreeGreen, Vector2{3500,3940}, PropType::TREE, Textures},    // 36 
            Prop{Textures.TreeGreen, Vector2{3600,3940}, PropType::TREE, Textures},    // 37
            Prop{Textures.TreeGreen, Vector2{3700,3940}, PropType::TREE, Textures},    // 38
            Prop{Textures.TreeGreen, Vector2{3800,3940}, PropType::TREE, Textures},    // 39
            Prop{Textures.TreeGreen, Vector2{3900,3940}, PropType::TREE, Textures},    // 40 
            Prop{Textures.TreeGreen, Vector2{4000,3940}, PropType::TREE, Textures},    // 41

            // large tree
            Prop{Textures.TreeGreen, Vector2{2185,2050}, PropType::TREE, Textures, 8.f}
        };
    }

    std::vector<Enemy> InitializeEnemies(Background& MapBG, Window& Window, GameTexture& Textures)
    {
        std::vector<Enemy> Enemies{};

        // ----------------------------------- Bears ------------------------------------

        Enemy BrownBearTwo
        {
            Sprite{Textures.BearBrownIdle, 4, 4},
            Sprite{Textures.BearBrownWalk, 4, 4},
            Sprite{Textures.BearBrownAttack, 4, 4},
            Sprite{Textures.BearBrownHurt, 1, 4},
            Sprite{Textures.BearBrownDeath, 11, 4},
            Sprite{Textures.Placeholder, 0, 0},
            EnemyType::BEAR, EnemyType::NORMAL, Vector2{2940, 740}, &Window, &MapBG, Textures
        };
        Enemies.emplace_back(BrownBearTwo);

        Enemy BrownBearThree
        {
            Sprite{Textures.BearBrownIdle, 4, 4},
            Sprite{Textures.BearBrownWalk, 4, 4},
            Sprite{Textures.BearBrownAttack, 4, 4},
            Sprite{Textures.BearBrownHurt, 1, 4},
            Sprite{Textures.BearBrownDeath, 11, 4},
            Sprite{Textures.Placeholder, 0, 0},
            EnemyType::BEAR, EnemyType::NORMAL, Vector2{2927, 1090}, &Window, &MapBG, Textures
        };
        Enemies.emplace_back(BrownBearThree);

        Enemy LightBrownBearTwo
        {
            Sprite{Textures.BearLightBrownIdle, 4, 4},
            Sprite{Textures.BearLightBrownWalk, 4, 4},
            Sprite{Textures.BearLightBrownAttack, 4, 4},
            Sprite{Textures.BearLightBrownHurt, 1, 4},
            Sprite{Textures.BearLightBrownDeath, 11, 4},
            Sprite{Textures.Placeholder, 0, 0},
            EnemyType::BEAR, EnemyType::NORMAL, Vector2{3102, 899}, &Window, &MapBG, Textures
        };
        Enemies.emplace_back(LightBrownBearTwo);

        Enemy LightBrownBearThree
        {
            Sprite{Textures.BearLightBrownIdle, 4, 4},
            Sprite{Textures.BearLightBrownWalk, 4, 4},
            Sprite{Textures.BearLightBrownAttack, 4, 4},
            Sprite{Textures.BearLightBrownHurt, 1, 4},
            Sprite{Textures.BearLightBrownDeath, 11, 4},
            Sprite{Textures.Placeholder, 0, 0},
            EnemyType::BEAR, EnemyType::NORMAL, Vector2{2835, 905}, &Window, &MapBG, Textures
        };
        Enemies.emplace_back(LightBrownBearThree);

        // ----------------------------------- Toads ------------------------------------

        Enemy GreenToadOne
        {
            Sprite{Textures.ToadGreenIdle, 4, 4},
            Sprite{Textures.ToadGreenWalk, 4, 4},
            Sprite{Textures.ToadGreenAttack, 4, 4},
            Sprite{Textures.ToadGreenHurt, 1, 4},
            Sprite{Textures.ToadGreenDeath, 11, 4},
            Sprite{Textures.Placeholder, 0, 0},
            EnemyType::TOAD, EnemyType::NORMAL, Vector2{1215, 230}, &Window, &MapBG, Textures
        };
        Enemies.emplace_back(GreenToadOne);

        Enemy GreenToadTwo
        {
            Sprite{Textures.ToadGreenIdle, 4, 4},
            Sprite{Textures.ToadGreenWalk, 4, 4},
            Sprite{Textures.ToadGreenAttack, 4, 4},
            Sprite{Textures.ToadGreenHurt, 1, 4},
            Sprite{Textures.ToadGreenDeath, 11, 4},
            Sprite{Textures.Placeholder, 0, 0},
            EnemyType::TOAD, EnemyType::NORMAL, Vector2{1107, 346}, &Window, &MapBG, Textures
        };
        Enemies.emplace_back(GreenToadTwo);

        Enemy PinkToadOne
        {
            Sprite{Textures.ToadPinkIdle, 4, 4},
            Sprite{Textures.ToadPinkWalk, 4, 4},
            Sprite{Textures.ToadPinkAttack, 4, 4},
            Sprite{Textures.ToadPinkHurt, 1, 4},
            Sprite{Textures.ToadPinkDeath, 11, 4},
            Sprite{Textures.Placeholder, 0, 0},
            EnemyType::TOAD, EnemyType::NORMAL, Vector2{1387, 356}, &Window, &MapBG, Textures
        };
        Enemies.emplace_back(PinkToadOne);

        // ----------------------------------- Mushrooms ------------------------------------

        Enemy RedMushroomOne
        {
            Sprite{Textures.MushroomRedIdle, 4, 4},
            Sprite{Textures.MushroomRedWalk, 4, 4},
            Sprite{Textures.MushroomRedAttack, 4, 4},
            Sprite{Textures.MushroomRedHurt, 4, 4},
            Sprite{Textures.MushroomRedDeath, 11, 4},
            Sprite{Textures.Placeholder, 0, 0},
            EnemyType::MUSHROOM, EnemyType::NORMAL, Vector2{555, 335}, &Window, &MapBG, Textures, 2
        };
        Enemies.emplace_back(RedMushroomOne);

        Enemy BlueMushroomOne
        {
            Sprite{Textures.MushroomBlueIdle, 4, 4},
            Sprite{Textures.MushroomBlueWalk, 4, 4},
            Sprite{Textures.MushroomBlueAttack, 4, 4},
            Sprite{Textures.MushroomBlueHurt, 4, 4},
            Sprite{Textures.MushroomBlueDeath, 11, 4},
            Sprite{Textures.Placeholder, 0, 0},
            EnemyType::MUSHROOM, EnemyType::NORMAL, Vector2{628, 236}, &Window, &MapBG, Textures, 2
        };
        Enemies.emplace_back(BlueMushroomOne);

        Enemy BlueMushroomTwo
        {
            Sprite{Textures.MushroomBlueIdle, 4, 4},
            Sprite{Textures.MushroomBlueWalk, 4, 4},
            Sprite{Textures.MushroomBlueAttack, 4, 4},
            Sprite{Textures.MushroomBlueHurt, 4, 4},
            Sprite{Textures.MushroomBlueDeath, 11, 4},
            Sprite{Textures.Placeholder, 0, 0},
            EnemyType::MUSHROOM, EnemyType::NORMAL, Vector2{1782, 226}, &Window, &MapBG, Textures, 2
        };
        Enemies.emplace_back(BlueMushroomTwo);


        // ----------------------------------- Test enemies below ------------------------------------

        // Enemy BrownBear
        // {
        //     Sprite{Textures.BearBrownIdle, 4, 4},
        //     Sprite{Textures.BearBrownWalk, 4, 4},
        //     Sprite{Textures.BearBrownAttack, 4, 4},
        //     Sprite{Textures.BearBrownHurt, 1, 4},
        //     Sprite{Textures.BearBrownDeath, 11, 4},
        //     Sprite{Textures.Placeholder, 0, 0},
        //     EnemyType::BEAR, EnemyType::NORMAL, Vector2{1800, 3566}, &Window, &MapBG, Textures, 1
        // };
        // Enemies.emplace_back(BrownBear);

        Enemy GreyBossBear
        {
            Sprite{Textures.BearGreyIdle, 4, 4},
            Sprite{Textures.BearGreyWalk, 4, 4},
            Sprite{Textures.BearGreyAttack, 4, 4},
            Sprite{Textures.BearGreyHurt, 1, 4},
            Sprite{Textures.BearGreyDeath, 11, 4},
            Sprite{Textures.Placeholder, 0, 0},
            EnemyType::BEAR, EnemyType::BOSS, Vector2{2163, 1518}, &Window, &MapBG, Textures, 5, 5.f
        };
        Enemies.emplace_back(GreyBossBear);

        // Enemy RedBeholder
        // {
        //     Sprite{Textures.BeholderRedIdle, 4, 4},
        //     Sprite{Textures.BeholderRedWalk, 4, 4},
        //     Sprite{Textures.BeholderRedAttack, 4, 4},
        //     Sprite{Textures.BeholderRedHurt, 4, 4},
        //     Sprite{Textures.BeholderRedDeath, 11, 4},
        //     Sprite{Textures.BeholderRedProjectile, 4, 4},
        //     EnemyType::BEHOLDER, EnemyType::NORMAL, Vector2{1550, 3466}, &Window, &MapBG, Textures, 3
        // };
        // Enemies.emplace_back(RedBeholder);

        // Enemy WhiteGhost
        // {
        //     Sprite{Textures.GhostWhiteIdle, 4, 4},
        //     Sprite{Textures.GhostWhiteWalk, 4, 4},
        //     Sprite{Textures.GhostWhiteAttack, 4, 4},
        //     Sprite{Textures.GhostWhiteHurt, 1, 4},
        //     Sprite{Textures.GhostWhiteDeath, 11, 4},
        //     Sprite{Textures.Placeholder, 0, 0},
        //     EnemyType::GHOST, EnemyType::NORMAL, Vector2{1850, 3566}, &Window, &MapBG, Textures, 1
        // };
        // Enemies.emplace_back(WhiteGhost);
    
        Enemy RedNecro
        {
            Sprite{Textures.NecromancerRedIdle, 4, 4},
            Sprite{Textures.NecromancerRedWalk, 4, 4},
            Sprite{Textures.NecromancerRedAttack, 4, 4},
            Sprite{Textures.NecromancerRedHurt, 1, 4},
            Sprite{Textures.NecromancerRedDeath, 11, 4},
            Sprite{Textures.NecromancerRedProjectile, 4, 4},
            EnemyType::NECROMANCER, EnemyType::NORMAL, Vector2{2038, 2345}, &Window, &MapBG, Textures
        };
        Enemies.emplace_back(RedNecro);

        Enemy PurpleCreature
        {
            Sprite{Textures.CreaturePurpleIdle, 4, 4},
            Sprite{Textures.CreaturePurpleWalk, 4, 4},
            Sprite{Textures.CreaturePurpleAttack, 4, 4},
            Sprite{Textures.CreaturePurpleHurt, 1, 4},
            Sprite{Textures.CreaturePurpleDeath, 11, 4},
            Sprite{Textures.Placeholder, 0, 0},
            EnemyType::CREATURE, EnemyType::NORMAL, Vector2{1660, 3366}, &Window, &MapBG, Textures
        };
        Enemies.emplace_back(PurpleCreature);

        Enemy GreenImp
        {
            Sprite{Textures.ImpGreenIdle, 4, 4},
            Sprite{Textures.ImpGreenWalk, 4, 4},
            Sprite{Textures.ImpGreenAttack, 4, 4},
            Sprite{Textures.ImpGreenHurt, 1, 4},
            Sprite{Textures.ImpGreenDeath, 11, 4},
            Sprite{Textures.ImpGreenProjectile, 4, 4},
            EnemyType::IMP, EnemyType::NORMAL, Vector2{1800, 3666}, &Window, &MapBG, Textures
        };
        Enemies.emplace_back(GreenImp);

        // Enemy BlueShadow
        // {
        //     Sprite{Textures.ShadowBlueIdle, 4, 4},
        //     Sprite{Textures.ShadowBlueWalk, 4, 4},
        //     Sprite{Textures.ShadowBlueAttack, 4, 4},
        //     Sprite{Textures.ShadowBlueHurt, 1, 4},
        //     Sprite{Textures.ShadowBlueDeath, 11, 4},
        //     Sprite{Textures.Placeholder, 0, 0},
        //     EnemyType::SHADOW, EnemyType::NORMAL, Vector2{1800, 3666}, &Window, &MapBG, Textures
        // };
        // Enemies.emplace_back(BlueShadow);

        // Enemy BrownSpider
        // {
        //     Sprite{Textures.SpiderBrownIdle, 4, 4},
        //     Sprite{Textures.SpiderBrownWalk, 4, 4},
        //     Sprite{Textures.SpiderBrownAttack, 4, 4},
        //     Sprite{Textures.SpiderBrownHurt, 1, 4},
        //     Sprite{Textures.SpiderBrownDeath, 11, 4},
        //     Sprite{Textures.Placeholder, 0, 0},
        //     EnemyType::SPIDER, EnemyType::NORMAL, Vector2{1600, 3666}, &Window, &MapBG, Textures
        // };
        // Enemies.emplace_back(BrownSpider);

        // ------------------- Wildlife NPCs ---------------------//
        //                                                        //
        // -------------------- Fox Family -----------------------//

        Enemy FoxFamOne
        {
            Sprite{Textures.FoxFamilyIdle, 5, 2},
            Sprite{Textures.FoxFamilyIdleTwo, 14, 2},
            Sprite{Textures.FoxFamilyWalk, 8, 2},
            Sprite{Textures.FoxFamilyLazy, 13, 2},
            Sprite{Textures.FoxFamilySleep, 6, 2},
            EnemyType::FOX, Vector2{473, 2524}, &Window, &MapBG, Textures
        };
        Enemies.emplace_back(FoxFamOne);
        
        Enemy FoxFamTwo
        {
            Sprite{Textures.FoxFamilyIdle, 5, 2},
            Sprite{Textures.FoxFamilyIdleTwo, 14, 2},
            Sprite{Textures.FoxFamilyWalk, 8, 2},
            Sprite{Textures.FoxFamilyLazy, 13, 2},
            Sprite{Textures.FoxFamilySleep, 6, 2},
            EnemyType::FOX, Vector2{247, 2524}, &Window, &MapBG, Textures
        };
        Enemies.emplace_back(FoxFamTwo);

        Enemy FoxFamThree
        {
            Sprite{Textures.FoxFamilyIdle, 5, 2},
            Sprite{Textures.FoxFamilyIdleTwo, 14, 2},
            Sprite{Textures.FoxFamilyWalk, 8, 2},
            Sprite{Textures.FoxFamilyLazy, 13, 2},
            Sprite{Textures.FoxFamilySleep, 6, 2},
            EnemyType::FOX, Vector2{428, 2840}, &Window, &MapBG, Textures, 2.5f
        };
        Enemies.emplace_back(FoxFamThree);

        Enemy FoxFamFour
        {
            Sprite{Textures.FoxFamilyIdle, 5, 2},
            Sprite{Textures.FoxFamilyIdleTwo, 14, 2},
            Sprite{Textures.FoxFamilyWalk, 8, 2},
            Sprite{Textures.FoxFamilyLazy, 13, 2},
            Sprite{Textures.FoxFamilySleep, 6, 2},
            EnemyType::FOX, Vector2{354, 2661}, &Window, &MapBG, Textures, 2.5f
        };
        Enemies.emplace_back(FoxFamFour);

        // ------------------- Squirrels ---------------------
        
        // Enemy SquirrelOne
        // {
        //     Sprite{Textures.SquirrelIdle, 6, 2},
        //     Sprite{Textures.SquirrelIdleTwo, 6, 2},
        //     Sprite{Textures.SquirrelWalk, 4, 2},
        //     Sprite{Textures.SquirrelEat, 2, 2},
        //     Sprite{Textures.SquirrelEat, 2, 2},
        //     EnemyType::SQUIRREL, Vector2{2839, 2041}, &Window, &MapBG, Textures, 2.0f
        // };
        // Enemies.emplace_back(SquirrelOne);

        // Enemy SquirrelTwo
        // {
        //     Sprite{Textures.SquirrelIdle, 6, 2},
        //     Sprite{Textures.SquirrelIdleTwo, 6, 2},
        //     Sprite{Textures.SquirrelWalk, 4, 2},
        //     Sprite{Textures.SquirrelEat, 2, 2},
        //     Sprite{Textures.SquirrelEat, 2, 2},
        //     EnemyType::SQUIRREL, Vector2{3050, 2147}, &Window, &MapBG, Textures, 2.0f
        // };
        // Enemies.emplace_back(SquirrelTwo);

        // Enemy SquirrelThree
        // {
        //     Sprite{Textures.SquirrelIdle, 6, 2},
        //     Sprite{Textures.SquirrelIdleTwo, 6, 2},
        //     Sprite{Textures.SquirrelWalk, 4, 2},
        //     Sprite{Textures.SquirrelEat, 2, 2},
        //     Sprite{Textures.SquirrelEat, 2, 2},
        //     EnemyType::SQUIRREL, Vector2{3155, 2133}, &Window, &MapBG, Textures, 2.0f
        // };
        // Enemies.emplace_back(SquirrelThree);

        // Enemy SquirrelFour
        // {
        //     Sprite{Textures.SquirrelIdle, 6, 2},
        //     Sprite{Textures.SquirrelIdleTwo, 6, 2},
        //     Sprite{Textures.SquirrelWalk, 4, 2},
        //     Sprite{Textures.SquirrelEat, 2, 2},
        //     Sprite{Textures.SquirrelEat, 2, 2},
        //     EnemyType::SQUIRREL, Vector2{2392, 244}, &Window, &MapBG, Textures, 2.0f
        // };
        // Enemies.emplace_back(SquirrelFour);

        // Enemy SquirrelFive
        // {
        //     Sprite{Textures.SquirrelIdle, 6, 2},
        //     Sprite{Textures.SquirrelIdleTwo, 6, 2},
        //     Sprite{Textures.SquirrelWalk, 4, 2},
        //     Sprite{Textures.SquirrelEat, 2, 2},
        //     Sprite{Textures.SquirrelEat, 2, 2},
        //     EnemyType::SQUIRREL, Vector2{3251, 339}, &Window, &MapBG, Textures, 2.0f
        // };
        // Enemies.emplace_back(SquirrelFive);

        // Enemy SquirrelSix
        // {
        //     Sprite{Textures.SquirrelIdle, 6, 2},
        //     Sprite{Textures.SquirrelIdleTwo, 6, 2},
        //     Sprite{Textures.SquirrelWalk, 4, 2},
        //     Sprite{Textures.SquirrelEat, 2, 2},
        //     Sprite{Textures.SquirrelEat, 2, 2},
        //     EnemyType::SQUIRREL, Vector2{2767, 338}, &Window, &MapBG, Textures, 2.0f
        // };
        // Enemies.emplace_back(SquirrelSix);

        // Enemy SquirrelSeven
        // {
        //     Sprite{Textures.SquirrelIdle, 6, 2},
        //     Sprite{Textures.SquirrelIdleTwo, 6, 2},
        //     Sprite{Textures.SquirrelWalk, 4, 2},
        //     Sprite{Textures.SquirrelEat, 2, 2},
        //     Sprite{Textures.SquirrelEat, 2, 2},
        //     EnemyType::SQUIRREL, Vector2{916, 940}, &Window, &MapBG, Textures, 2.0f
        // };
        // Enemies.emplace_back(SquirrelSeven);

        // Enemy SquirrelEight
        // {
        //     Sprite{Textures.SquirrelIdle, 6, 2},
        //     Sprite{Textures.SquirrelIdleTwo, 6, 2},
        //     Sprite{Textures.SquirrelWalk, 4, 2},
        //     Sprite{Textures.SquirrelEat, 2, 2},
        //     Sprite{Textures.SquirrelEat, 2, 2},
        //     EnemyType::SQUIRREL, Vector2{539, 1221}, &Window, &MapBG, Textures, 2.0f
        // };
        // Enemies.emplace_back(SquirrelEight);

        // Enemy SquirrelNine
        // {
        //     Sprite{Textures.SquirrelIdle, 6, 2},
        //     Sprite{Textures.SquirrelIdleTwo, 6, 2},
        //     Sprite{Textures.SquirrelWalk, 4, 2},
        //     Sprite{Textures.SquirrelEat, 2, 2},
        //     Sprite{Textures.SquirrelEat, 2, 2},
        //     EnemyType::SQUIRREL, Vector2{932, 1265}, &Window, &MapBG, Textures, 2.0f
        // };
        // Enemies.emplace_back(SquirrelNine);

        // Enemy SquirrelTen
        // {
        //     Sprite{Textures.SquirrelIdle, 6, 2},
        //     Sprite{Textures.SquirrelIdleTwo, 6, 2},
        //     Sprite{Textures.SquirrelWalk, 4, 2},
        //     Sprite{Textures.SquirrelEat, 2, 2},
        //     Sprite{Textures.SquirrelEat, 2, 2},
        //     EnemyType::SQUIRREL, Vector2{345, 1663}, &Window, &MapBG, Textures, 2.0f
        // };
        // Enemies.emplace_back(SquirrelTen);

        // Enemy SquirrelEleven
        // {
        //     Sprite{Textures.SquirrelIdle, 6, 2},
        //     Sprite{Textures.SquirrelIdleTwo, 6, 2},
        //     Sprite{Textures.SquirrelWalk, 4, 2},
        //     Sprite{Textures.SquirrelEat, 2, 2},
        //     Sprite{Textures.SquirrelEat, 2, 2},
        //     EnemyType::SQUIRREL, Vector2{254, 532}, &Window, &MapBG, Textures, 2.0f
        // };
        // Enemies.emplace_back(SquirrelEleven);

        // Enemy SquirrelTwelve
        // {
        //     Sprite{Textures.SquirrelIdle, 6, 2},
        //     Sprite{Textures.SquirrelIdleTwo, 6, 2},
        //     Sprite{Textures.SquirrelWalk, 4, 2},
        //     Sprite{Textures.SquirrelEat, 2, 2},
        //     Sprite{Textures.SquirrelEat, 2, 2},
        //     EnemyType::SQUIRREL, Vector2{334, 432}, &Window, &MapBG, Textures, 2.0f
        // };
        // Enemies.emplace_back(SquirrelTwelve);

        // Enemy SquirrelThirteen
        // {
        //     Sprite{Textures.SquirrelIdle, 6, 2},
        //     Sprite{Textures.SquirrelIdleTwo, 6, 2},
        //     Sprite{Textures.SquirrelWalk, 4, 2},
        //     Sprite{Textures.SquirrelEat, 2, 2},
        //     Sprite{Textures.SquirrelEat, 2, 2},
        //     EnemyType::SQUIRREL, Vector2{452, 533}, &Window, &MapBG, Textures, 2.0f
        // };
        // Enemies.emplace_back(SquirrelThirteen);

        // Enemy SquirrelFourteen
        // {
        //     Sprite{Textures.SquirrelIdle, 6, 2},
        //     Sprite{Textures.SquirrelIdleTwo, 6, 2},
        //     Sprite{Textures.SquirrelWalk, 4, 2},
        //     Sprite{Textures.SquirrelEat, 2, 2},
        //     Sprite{Textures.SquirrelEat, 2, 2},
        //     EnemyType::SQUIRREL, Vector2{1588, 1780}, &Window, &MapBG, Textures, 2.0f
        // };
        // Enemies.emplace_back(SquirrelFourteen);

        // Enemy SquirrelFifteen
        // {
        //     Sprite{Textures.SquirrelIdle, 6, 2},
        //     Sprite{Textures.SquirrelIdleTwo, 6, 2},
        //     Sprite{Textures.SquirrelWalk, 4, 2},
        //     Sprite{Textures.SquirrelEat, 2, 2},
        //     Sprite{Textures.SquirrelEat, 2, 2},
        //     EnemyType::SQUIRREL, Vector2{1739, 2002}, &Window, &MapBG, Textures, 2.0f
        // };
        // Enemies.emplace_back(SquirrelFifteen);

        // Enemy SquirrelSixteen
        // {
        //     Sprite{Textures.SquirrelIdle, 6, 2},
        //     Sprite{Textures.SquirrelIdleTwo, 6, 2},
        //     Sprite{Textures.SquirrelWalk, 4, 2},
        //     Sprite{Textures.SquirrelEat, 2, 2},
        //     Sprite{Textures.SquirrelEat, 2, 2},
        //     EnemyType::SQUIRREL, Vector2{2534, 2520}, &Window, &MapBG, Textures, 2.0f
        // };
        // Enemies.emplace_back(SquirrelSixteen);

        // Enemy SquirrelSeventeen
        // {
        //     Sprite{Textures.SquirrelIdle, 6, 2},
        //     Sprite{Textures.SquirrelIdleTwo, 6, 2},
        //     Sprite{Textures.SquirrelWalk, 4, 2},
        //     Sprite{Textures.SquirrelEat, 2, 2},
        //     Sprite{Textures.SquirrelEat, 2, 2},
        //     EnemyType::SQUIRREL, Vector2{2086, 2928}, &Window, &MapBG, Textures, 2.0f
        // };
        // Enemies.emplace_back(SquirrelSeventeen);

        // Enemy SquirrelEighteen
        // {
        //     Sprite{Textures.SquirrelIdle, 6, 2},
        //     Sprite{Textures.SquirrelIdleTwo, 6, 2},
        //     Sprite{Textures.SquirrelWalk, 4, 2},
        //     Sprite{Textures.SquirrelEat, 2, 2},
        //     Sprite{Textures.SquirrelEat, 2, 2},
        //     EnemyType::SQUIRREL, Vector2{3964, 2537}, &Window, &MapBG, Textures, 2.0f
        // };
        // Enemies.emplace_back(SquirrelEighteen);

        // Enemy SquirrelNineTeen
        // {
        //     Sprite{Textures.SquirrelIdle, 6, 2},
        //     Sprite{Textures.SquirrelIdleTwo, 6, 2},
        //     Sprite{Textures.SquirrelWalk, 4, 2},
        //     Sprite{Textures.SquirrelEat, 2, 2},
        //     Sprite{Textures.SquirrelEat, 2, 2},
        //     EnemyType::SQUIRREL, Vector2{3658, 3859}, &Window, &MapBG, Textures, 2.0f
        // };
        // Enemies.emplace_back(SquirrelNineTeen);

        // Enemy SquirrelTwenty
        // {
        //     Sprite{Textures.SquirrelIdle, 6, 2},
        //     Sprite{Textures.SquirrelIdleTwo, 6, 2},
        //     Sprite{Textures.SquirrelWalk, 4, 2},
        //     Sprite{Textures.SquirrelEat, 2, 2},
        //     Sprite{Textures.SquirrelEat, 2, 2},
        //     EnemyType::SQUIRREL, Vector2{480, 3813}, &Window, &MapBG, Textures, 2.0f
        // };
        // Enemies.emplace_back(SquirrelTwenty);

        // Enemy SquirrelTwentyOne
        // {
        //     Sprite{Textures.SquirrelIdle, 6, 2},
        //     Sprite{Textures.SquirrelIdleTwo, 6, 2},
        //     Sprite{Textures.SquirrelWalk, 4, 2},
        //     Sprite{Textures.SquirrelEat, 2, 2},
        //     Sprite{Textures.SquirrelEat, 2, 2},
        //     EnemyType::SQUIRREL, Vector2{661, 3847}, &Window, &MapBG, Textures, 2.0f
        // };
        // Enemies.emplace_back(SquirrelTwentyOne);

        // Enemy SquirrelTwentyTwo
        // {
        //     Sprite{Textures.SquirrelIdle, 6, 2},
        //     Sprite{Textures.SquirrelIdleTwo, 6, 2},
        //     Sprite{Textures.SquirrelWalk, 4, 2},
        //     Sprite{Textures.SquirrelEat, 2, 2},
        //     Sprite{Textures.SquirrelEat, 2, 2},
        //     EnemyType::SQUIRREL, Vector2{1354, 3846}, &Window, &MapBG, Textures, 2.0f
        // };
        // Enemies.emplace_back(SquirrelTwentyTwo);

        // Enemy SquirrelTwentyThree
        // {
        //     Sprite{Textures.SquirrelIdle, 6, 2},
        //     Sprite{Textures.SquirrelIdleTwo, 6, 2},
        //     Sprite{Textures.SquirrelWalk, 4, 2},
        //     Sprite{Textures.SquirrelEat, 2, 2},
        //     Sprite{Textures.SquirrelEat, 2, 2},
        //     EnemyType::SQUIRREL, Vector2{155, 1929}, &Window, &MapBG, Textures, 2.0f
        // };
        // Enemies.emplace_back(SquirrelTwentyThree);

        // Enemy SquirrelTwentyFour
        // {
        //     Sprite{Textures.SquirrelIdle, 6, 2},
        //     Sprite{Textures.SquirrelIdleTwo, 6, 2},
        //     Sprite{Textures.SquirrelWalk, 4, 2},
        //     Sprite{Textures.SquirrelEat, 2, 2},
        //     Sprite{Textures.SquirrelEat, 2, 2},
        //     EnemyType::SQUIRREL, Vector2{337, 2032}, &Window, &MapBG, Textures, 2.0f
        // };
        // Enemies.emplace_back(SquirrelTwentyFour);

        // Enemy SquirrelTwentyFive
        // {
        //     Sprite{Textures.SquirrelIdle, 6, 2},
        //     Sprite{Textures.SquirrelIdleTwo, 6, 2},
        //     Sprite{Textures.SquirrelWalk, 4, 2},
        //     Sprite{Textures.SquirrelEat, 2, 2},
        //     Sprite{Textures.SquirrelEat, 2, 2},
        //     EnemyType::SQUIRREL, Vector2{162, 2127}, &Window, &MapBG, Textures, 2.0f
        // };
        // Enemies.emplace_back(SquirrelTwentyFive);

        // Enemy SquirrelTwentySix
        // {
        //     Sprite{Textures.SquirrelIdle, 6, 2},
        //     Sprite{Textures.SquirrelIdleTwo, 6, 2},
        //     Sprite{Textures.SquirrelWalk, 4, 2},
        //     Sprite{Textures.SquirrelEat, 2, 2},
        //     Sprite{Textures.SquirrelEat, 2, 2},
        //     EnemyType::SQUIRREL, Vector2{752, 1933}, &Window, &MapBG, Textures, 2.0f
        // };
        // Enemies.emplace_back(SquirrelTwentySix);

        // Enemy SquirrelTwentySeven
        // {
        //     Sprite{Textures.SquirrelIdle, 6, 2},
        //     Sprite{Textures.SquirrelIdleTwo, 6, 2},
        //     Sprite{Textures.SquirrelWalk, 4, 2},
        //     Sprite{Textures.SquirrelEat, 2, 2},
        //     Sprite{Textures.SquirrelEat, 2, 2},
        //     EnemyType::SQUIRREL, Vector2{934, 2033}, &Window, &MapBG, Textures, 2.0f
        // };
        // Enemies.emplace_back(SquirrelTwentySeven);

        // Enemy SquirrelTwentyEight
        // {
        //     Sprite{Textures.SquirrelIdle, 6, 2},
        //     Sprite{Textures.SquirrelIdleTwo, 6, 2},
        //     Sprite{Textures.SquirrelWalk, 4, 2},
        //     Sprite{Textures.SquirrelEat, 2, 2},
        //     Sprite{Textures.SquirrelEat, 2, 2},
        //     EnemyType::SQUIRREL, Vector2{333, 1631}, &Window, &MapBG, Textures, 2.0f
        // };
        // Enemies.emplace_back(SquirrelTwentyEight);

        // Enemy SquirrelTwentyNine
        // {
        //     Sprite{Textures.SquirrelIdle, 6, 2},
        //     Sprite{Textures.SquirrelIdleTwo, 6, 2},
        //     Sprite{Textures.SquirrelWalk, 4, 2},
        //     Sprite{Textures.SquirrelEat, 2, 2},
        //     Sprite{Textures.SquirrelEat, 2, 2},
        //     EnemyType::SQUIRREL, Vector2{838, 1630}, &Window, &MapBG, Textures, 2.0f
        // };
        // Enemies.emplace_back(SquirrelTwentyNine);

        // Enemy SquirrelThirty
        // {
        //     Sprite{Textures.SquirrelIdle, 6, 2},
        //     Sprite{Textures.SquirrelIdleTwo, 6, 2},
        //     Sprite{Textures.SquirrelWalk, 4, 2},
        //     Sprite{Textures.SquirrelEat, 2, 2},
        //     Sprite{Textures.SquirrelEat, 2, 2},
        //     EnemyType::SQUIRREL, Vector2{1107, 1525}, &Window, &MapBG, Textures, 2.0f
        // };
        // Enemies.emplace_back(SquirrelThirty);

        // Enemy SquirrelThirtyOne
        // {
        //     Sprite{Textures.SquirrelIdle, 6, 2},
        //     Sprite{Textures.SquirrelIdleTwo, 6, 2},
        //     Sprite{Textures.SquirrelWalk, 4, 2},
        //     Sprite{Textures.SquirrelEat, 2, 2},
        //     Sprite{Textures.SquirrelEat, 2, 2},
        //     EnemyType::SQUIRREL, Vector2{1061, 1728}, &Window, &MapBG, Textures, 2.0f
        // };
        // Enemies.emplace_back(SquirrelThirtyOne);

        // Enemy SquirrelThirtyTwo
        // {
        //     Sprite{Textures.SquirrelIdle, 6, 2},
        //     Sprite{Textures.SquirrelIdleTwo, 6, 2},
        //     Sprite{Textures.SquirrelWalk, 4, 2},
        //     Sprite{Textures.SquirrelEat, 2, 2},
        //     Sprite{Textures.SquirrelEat, 2, 2},
        //     EnemyType::SQUIRREL, Vector2{728, 912}, &Window, &MapBG, Textures, 2.0f
        // };
        // Enemies.emplace_back(SquirrelThirtyTwo);

        // Enemy SquirrelThirtyThree
        // {
        //     Sprite{Textures.SquirrelIdle, 6, 2},
        //     Sprite{Textures.SquirrelIdleTwo, 6, 2},
        //     Sprite{Textures.SquirrelWalk, 4, 2},
        //     Sprite{Textures.SquirrelEat, 2, 2},
        //     Sprite{Textures.SquirrelEat, 2, 2},
        //     EnemyType::SQUIRREL, Vector2{1145, 772}, &Window, &MapBG, Textures, 2.0f
        // };
        // Enemies.emplace_back(SquirrelThirtyThree);

        return Enemies;
    }

    std::vector<Enemy> InitializeCrows(Background& MapBG, Window& Window, GameTexture& Textures)
    {
        std::vector<Enemy> Crows{};

        Enemy CrowOne
        {
            Sprite{Textures.CrowIdle, 8, 2},
            Sprite{Textures.CrowIdleTwo, 5, 2},
            Sprite{Textures.CrowFlying, 5, 2},
            Sprite{Textures.CrowWalk, 4, 2},
            Sprite{Textures.CrowIdleTwo, 5, 2},
            EnemyType::CROW, Vector2{3740, 2149}, &Window, &MapBG, Textures, 2.5f
        };
        Crows.emplace_back(CrowOne);

        // Enemy CrowTwo
        // {
        //     Sprite{Textures.CrowIdle, 8, 2},
        //     Sprite{Textures.CrowIdleTwo, 5, 2},
        //     Sprite{Textures.CrowFlying, 5, 2},
        //     Sprite{Textures.CrowWalk, 4, 2},
        //     Sprite{Textures.CrowIdleTwo, 5, 2},
        //     EnemyType::CROW, Vector2{3546, 2032}, &Window, &MapBG, Textures, 2.5f
        // };
        // Crows.emplace_back(CrowTwo);

        // Enemy CrowThree
        // {
        //     Sprite{Textures.CrowIdle, 8, 2},
        //     Sprite{Textures.CrowIdleTwo, 5, 2},
        //     Sprite{Textures.CrowFlying, 5, 2},
        //     Sprite{Textures.CrowWalk, 4, 2},
        //     Sprite{Textures.CrowIdleTwo, 5, 2},
        //     EnemyType::CROW, Vector2{3339, 2132}, &Window, &MapBG, Textures, 2.5f
        // };
        // Crows.emplace_back(CrowThree);

        // Enemy CrowFour
        // {
        //     Sprite{Textures.CrowIdle, 8, 2},
        //     Sprite{Textures.CrowIdleTwo, 5, 2},
        //     Sprite{Textures.CrowFlying, 5, 2},
        //     Sprite{Textures.CrowWalk, 4, 2},
        //     Sprite{Textures.CrowIdleTwo, 5, 2},
        //     EnemyType::CROW, Vector2{3848, 1936}, &Window, &MapBG, Textures, 2.5f
        // };
        // Crows.emplace_back(CrowFour);
        
        // Enemy CrowFive
        // {
        //     Sprite{Textures.CrowIdle, 8, 2},
        //     Sprite{Textures.CrowIdleTwo, 5, 2},
        //     Sprite{Textures.CrowFlying, 5, 2},
        //     Sprite{Textures.CrowWalk, 4, 2},
        //     Sprite{Textures.CrowIdleTwo, 5, 2},
        //     EnemyType::CROW, Vector2{3947, 1842}, &Window, &MapBG, Textures, 2.5f
        // };
        // Crows.emplace_back(CrowFive);
        
        // Enemy CrowSix
        // {
        //     Sprite{Textures.CrowIdle, 8, 2},
        //     Sprite{Textures.CrowIdleTwo, 5, 2},
        //     Sprite{Textures.CrowFlying, 5, 2},
        //     Sprite{Textures.CrowWalk, 4, 2},
        //     Sprite{Textures.CrowIdleTwo, 5, 2},
        //     EnemyType::CROW, Vector2{3338, 1931}, &Window, &MapBG, Textures, 2.5f
        // };
        // Crows.emplace_back(CrowSix);

        // Enemy CrowSeven
        // {
        //     Sprite{Textures.CrowIdle, 8, 2},
        //     Sprite{Textures.CrowIdleTwo, 5, 2},
        //     Sprite{Textures.CrowFlying, 5, 2},
        //     Sprite{Textures.CrowWalk, 4, 2},
        //     Sprite{Textures.CrowIdleTwo, 5, 2},
        //     EnemyType::CROW, Vector2{3458, 1729}, &Window, &MapBG, Textures, 2.5f
        // };
        // Crows.emplace_back(CrowSeven);

        // Enemy CrowEight
        // {
        //     Sprite{Textures.CrowIdle, 8, 2},
        //     Sprite{Textures.CrowIdleTwo, 5, 2},
        //     Sprite{Textures.CrowFlying, 5, 2},
        //     Sprite{Textures.CrowWalk, 4, 2},
        //     Sprite{Textures.CrowIdleTwo, 5, 2},
        //     EnemyType::CROW, Vector2{3750, 1607}, &Window, &MapBG, Textures, 2.5f
        // };
        // Crows.emplace_back(CrowEight);

        // Enemy CrowNine
        // {
        //     Sprite{Textures.CrowIdle, 8, 2},
        //     Sprite{Textures.CrowIdleTwo, 5, 2},
        //     Sprite{Textures.CrowFlying, 5, 2},
        //     Sprite{Textures.CrowWalk, 4, 2},
        //     Sprite{Textures.CrowIdleTwo, 5, 2},
        //     EnemyType::CROW, Vector2{3322, 1539}, &Window, &MapBG, Textures, 2.5f
        // };
        // Crows.emplace_back(CrowNine);

        // Enemy CrowTen
        // {
        //     Sprite{Textures.CrowIdle, 8, 2},
        //     Sprite{Textures.CrowIdleTwo, 5, 2},
        //     Sprite{Textures.CrowFlying, 5, 2},
        //     Sprite{Textures.CrowWalk, 4, 2},
        //     Sprite{Textures.CrowIdleTwo, 5, 2},
        //     EnemyType::CROW, Vector2{3763, 1434}, &Window, &MapBG, Textures, 2.5f
        // };
        // Crows.emplace_back(CrowTen);
        
        // Enemy CrowEleven
        // {
        //     Sprite{Textures.CrowIdle, 8, 2},
        //     Sprite{Textures.CrowIdleTwo, 5, 2},
        //     Sprite{Textures.CrowFlying, 5, 2},
        //     Sprite{Textures.CrowWalk, 4, 2},
        //     Sprite{Textures.CrowIdleTwo, 5, 2},
        //     EnemyType::CROW, Vector2{3339, 1355}, &Window, &MapBG, Textures, 2.5f
        // };
        // Crows.emplace_back(CrowEleven);
        
        // Enemy CrowTwelve
        // {
        //     Sprite{Textures.CrowIdle, 8, 2},
        //     Sprite{Textures.CrowIdleTwo, 5, 2},
        //     Sprite{Textures.CrowFlying, 5, 2},
        //     Sprite{Textures.CrowWalk, 4, 2},
        //     Sprite{Textures.CrowIdleTwo, 5, 2},
        //     EnemyType::CROW, Vector2{3860, 1249}, &Window, &MapBG, Textures, 2.5f
        // };
        // Crows.emplace_back(CrowTwelve);
        
        // Enemy CrowThirteen
        // {
        //     Sprite{Textures.CrowIdle, 8, 2},
        //     Sprite{Textures.CrowIdleTwo, 5, 2},
        //     Sprite{Textures.CrowFlying, 5, 2},
        //     Sprite{Textures.CrowWalk, 4, 2},
        //     Sprite{Textures.CrowIdleTwo, 5, 2},
        //     EnemyType::CROW, Vector2{3832, 939}, &Window, &MapBG, Textures, 2.5f
        // };
        // Crows.emplace_back(CrowThirteen);
        
        // Enemy CrowFourteen
        // {
        //     Sprite{Textures.CrowIdle, 8, 2},
        //     Sprite{Textures.CrowIdleTwo, 5, 2},
        //     Sprite{Textures.CrowFlying, 5, 2},
        //     Sprite{Textures.CrowWalk, 4, 2},
        //     Sprite{Textures.CrowIdleTwo, 5, 2},
        //     EnemyType::CROW, Vector2{3742, 747}, &Window, &MapBG, Textures, 2.5f
        // };
        // Crows.emplace_back(CrowFourteen);
        
        // Enemy CrowFifteen
        // {
        //     Sprite{Textures.CrowIdle, 8, 2},
        //     Sprite{Textures.CrowIdleTwo, 5, 2},
        //     Sprite{Textures.CrowFlying, 5, 2},
        //     Sprite{Textures.CrowWalk, 4, 2},
        //     Sprite{Textures.CrowIdleTwo, 5, 2},
        //     EnemyType::CROW, Vector2{3915, 747}, &Window, &MapBG, Textures, 2.5f
        // };
        // Crows.emplace_back(CrowFifteen);
        
        // Enemy CrowSixteen
        // {
        //     Sprite{Textures.CrowIdle, 8, 2},
        //     Sprite{Textures.CrowIdleTwo, 5, 2},
        //     Sprite{Textures.CrowFlying, 5, 2},
        //     Sprite{Textures.CrowWalk, 4, 2},
        //     Sprite{Textures.CrowIdleTwo, 5, 2},
        //     EnemyType::CROW, Vector2{3763, 635}, &Window, &MapBG, Textures, 2.5f
        // };
        // Crows.emplace_back(CrowSixteen);
        
        // Enemy CrowSeventeen
        // {
        //     Sprite{Textures.CrowIdle, 8, 2},
        //     Sprite{Textures.CrowIdleTwo, 5, 2},
        //     Sprite{Textures.CrowFlying, 5, 2},
        //     Sprite{Textures.CrowWalk, 4, 2},
        //     Sprite{Textures.CrowIdleTwo, 5, 2},
        //     EnemyType::CROW, Vector2{3850, 544}, &Window, &MapBG, Textures, 2.5f
        // };
        // Crows.emplace_back(CrowSeventeen);
        
        // Enemy CrowEighteen
        // {
        //     Sprite{Textures.CrowIdle, 8, 2},
        //     Sprite{Textures.CrowIdleTwo, 5, 2},
        //     Sprite{Textures.CrowFlying, 5, 2},
        //     Sprite{Textures.CrowWalk, 4, 2},
        //     Sprite{Textures.CrowIdleTwo, 5, 2},
        //     EnemyType::CROW, Vector2{3751, 441}, &Window, &MapBG, Textures, 2.5f
        // };
        // Crows.emplace_back(CrowEighteen);
        
        // Enemy CrowNineteen
        // {
        //     Sprite{Textures.CrowIdle, 8, 2},
        //     Sprite{Textures.CrowIdleTwo, 5, 2},
        //     Sprite{Textures.CrowFlying, 5, 2},
        //     Sprite{Textures.CrowWalk, 4, 2},
        //     Sprite{Textures.CrowIdleTwo, 5, 2},
        //     EnemyType::CROW, Vector2{3928, 361}, &Window, &MapBG, Textures, 2.5f
        // };
        // Crows.emplace_back(CrowNineteen);
        
        // Enemy CrowTwenty
        // {
        //     Sprite{Textures.CrowIdle, 8, 2},
        //     Sprite{Textures.CrowIdleTwo, 5, 2},
        //     Sprite{Textures.CrowFlying, 5, 2},
        //     Sprite{Textures.CrowWalk, 4, 2},
        //     Sprite{Textures.CrowIdleTwo, 5, 2},
        //     EnemyType::CROW, Vector2{4040, 551}, &Window, &MapBG, Textures, 2.5f
        // };
        // Crows.emplace_back(CrowTwenty);
        
        // Enemy CrowTwentyone
        // {
        //     Sprite{Textures.CrowIdle, 8, 2},
        //     Sprite{Textures.CrowIdleTwo, 5, 2},
        //     Sprite{Textures.CrowFlying, 5, 2},
        //     Sprite{Textures.CrowWalk, 4, 2},
        //     Sprite{Textures.CrowIdleTwo, 5, 2},
        //     EnemyType::CROW, Vector2{3459, 1170}, &Window, &MapBG, Textures, 2.5f
        // };
        // Crows.emplace_back(CrowTwentyone);
        
        // Enemy CrowTwentytwo
        // {
        //     Sprite{Textures.CrowIdle, 8, 2},
        //     Sprite{Textures.CrowIdleTwo, 5, 2},
        //     Sprite{Textures.CrowFlying, 5, 2},
        //     Sprite{Textures.CrowWalk, 4, 2},
        //     Sprite{Textures.CrowIdleTwo, 5, 2},
        //     EnemyType::CROW, Vector2{3463, 952}, &Window, &MapBG, Textures, 2.5f
        // };
        // Crows.emplace_back(CrowTwentytwo);
        
        // Enemy CrowTwentythree
        // {
        //     Sprite{Textures.CrowIdle, 8, 2},
        //     Sprite{Textures.CrowIdleTwo, 5, 2},
        //     Sprite{Textures.CrowFlying, 5, 2},
        //     Sprite{Textures.CrowWalk, 4, 2},
        //     Sprite{Textures.CrowIdleTwo, 5, 2},
        //     EnemyType::CROW, Vector2{3340, 648}, &Window, &MapBG, Textures, 2.5f
        // };
        // Crows.emplace_back(CrowTwentythree);
        
        // Enemy CrowTwentyfour
        // {
        //     Sprite{Textures.CrowIdle, 8, 2},
        //     Sprite{Textures.CrowIdleTwo, 5, 2},
        //     Sprite{Textures.CrowFlying, 5, 2},
        //     Sprite{Textures.CrowWalk, 4, 2},
        //     Sprite{Textures.CrowIdleTwo, 5, 2},
        //     EnemyType::CROW, Vector2{3450, 470}, &Window, &MapBG, Textures, 2.5f
        // };
        // Crows.emplace_back(CrowTwentyfour);
        
        // Enemy CrowTwentyfive
        // {
        //     Sprite{Textures.CrowIdle, 8, 2},
        //     Sprite{Textures.CrowIdleTwo, 5, 2},
        //     Sprite{Textures.CrowFlying, 5, 2},
        //     Sprite{Textures.CrowWalk, 4, 2},
        //     Sprite{Textures.CrowIdleTwo, 5, 2},
        //     EnemyType::CROW, Vector2{3357, 371}, &Window, &MapBG, Textures, 2.5f
        // };
        // Crows.emplace_back(CrowTwentyfive);
        
        // Enemy CrowTwentysix
        // {
        //     Sprite{Textures.CrowIdle, 8, 2},
        //     Sprite{Textures.CrowIdleTwo, 5, 2},
        //     Sprite{Textures.CrowFlying, 5, 2},
        //     Sprite{Textures.CrowWalk, 4, 2},
        //     Sprite{Textures.CrowIdleTwo, 5, 2},
        //     EnemyType::CROW, Vector2{3260, 1739}, &Window, &MapBG, Textures, 2.5f
        // };
        // Crows.emplace_back(CrowTwentysix);
        
        // Enemy CrowTwentyseven
        // {
        //     Sprite{Textures.CrowIdle, 8, 2},
        //     Sprite{Textures.CrowIdleTwo, 5, 2},
        //     Sprite{Textures.CrowFlying, 5, 2},
        //     Sprite{Textures.CrowWalk, 4, 2},
        //     Sprite{Textures.CrowIdleTwo, 5, 2},
        //     EnemyType::CROW, Vector2{3039, 1835}, &Window, &MapBG, Textures, 2.5f
        // };
        // Crows.emplace_back(CrowTwentyseven);

        // Enemy CrowTwentyeight
        // {
        //     Sprite{Textures.CrowIdle, 8, 2},
        //     Sprite{Textures.CrowIdleTwo, 5, 2},
        //     Sprite{Textures.CrowFlying, 5, 2},
        //     Sprite{Textures.CrowWalk, 4, 2},
        //     Sprite{Textures.CrowIdleTwo, 5, 2},
        //     EnemyType::CROW, Vector2{2838, 1553}, &Window, &MapBG, Textures, 2.5f
        // };
        // Crows.emplace_back(CrowTwentyeight);

        // Enemy CrowTwentynine
        // {
        //     Sprite{Textures.CrowIdle, 8, 2},
        //     Sprite{Textures.CrowIdleTwo, 5, 2},
        //     Sprite{Textures.CrowFlying, 5, 2},
        //     Sprite{Textures.CrowWalk, 4, 2},
        //     Sprite{Textures.CrowIdleTwo, 5, 2},
        //     EnemyType::CROW, Vector2{2638, 1440}, &Window, &MapBG, Textures, 2.5f
        // };
        // Crows.emplace_back(CrowTwentynine);

        // Enemy CrowThirty
        // {
        //     Sprite{Textures.CrowIdle, 8, 2},
        //     Sprite{Textures.CrowIdleTwo, 5, 2},
        //     Sprite{Textures.CrowFlying, 5, 2},
        //     Sprite{Textures.CrowWalk, 4, 2},
        //     Sprite{Textures.CrowIdleTwo, 5, 2},
        //     EnemyType::CROW, Vector2{638, 1791}, &Window, &MapBG, Textures, 2.5f
        // };
        // Crows.emplace_back(CrowThirty);

        // Enemy CrowThirtyone
        // {
        //     Sprite{Textures.CrowIdle, 8, 2},
        //     Sprite{Textures.CrowIdleTwo, 5, 2},
        //     Sprite{Textures.CrowFlying, 5, 2},
        //     Sprite{Textures.CrowWalk, 4, 2},
        //     Sprite{Textures.CrowIdleTwo, 5, 2},
        //     EnemyType::CROW, Vector2{549, 1951}, &Window, &MapBG, Textures, 2.5f
        // };
        // Crows.emplace_back(CrowThirtyone);

        // Enemy CrowThirtytwo
        // {
        //     Sprite{Textures.CrowIdle, 8, 2},
        //     Sprite{Textures.CrowIdleTwo, 5, 2},
        //     Sprite{Textures.CrowFlying, 5, 2},
        //     Sprite{Textures.CrowWalk, 4, 2},
        //     Sprite{Textures.CrowIdleTwo, 5, 2},
        //     EnemyType::CROW, Vector2{1436, 1431}, &Window, &MapBG, Textures, 2.5f
        // };
        // Crows.emplace_back(CrowThirtytwo);

        // Enemy CrowThirtythree
        // {
        //     Sprite{Textures.CrowIdle, 8, 2},
        //     Sprite{Textures.CrowIdleTwo, 5, 2},
        //     Sprite{Textures.CrowFlying, 5, 2},
        //     Sprite{Textures.CrowWalk, 4, 2},
        //     Sprite{Textures.CrowIdleTwo, 5, 2},
        //     EnemyType::CROW, Vector2{1761, 1146}, &Window, &MapBG, Textures, 2.5f
        // };
        // Crows.emplace_back(CrowThirtythree);

        // Enemy CrowThirtyfour
        // {
        //     Sprite{Textures.CrowIdle, 8, 2},
        //     Sprite{Textures.CrowIdleTwo, 5, 2},
        //     Sprite{Textures.CrowFlying, 5, 2},
        //     Sprite{Textures.CrowWalk, 4, 2},
        //     Sprite{Textures.CrowIdleTwo, 5, 2},
        //     EnemyType::CROW, Vector2{1453, 960}, &Window, &MapBG, Textures, 2.5f
        // };
        // Crows.emplace_back(CrowThirtyfour);

        // Enemy CrowThirtyfive
        // {
        //     Sprite{Textures.CrowIdle, 8, 2},
        //     Sprite{Textures.CrowIdleTwo, 5, 2},
        //     Sprite{Textures.CrowFlying, 5, 2},
        //     Sprite{Textures.CrowWalk, 4, 2},
        //     Sprite{Textures.CrowIdleTwo, 5, 2},
        //     EnemyType::CROW, Vector2{246, 774}, &Window, &MapBG, Textures, 2.5f
        // };
        // Crows.emplace_back(CrowThirtyfive);

        // Enemy CrowThirtysix
        // {
        //     Sprite{Textures.CrowIdle, 8, 2},
        //     Sprite{Textures.CrowIdleTwo, 5, 2},
        //     Sprite{Textures.CrowFlying, 5, 2},
        //     Sprite{Textures.CrowWalk, 4, 2},
        //     Sprite{Textures.CrowIdleTwo, 5, 2},
        //     EnemyType::CROW, Vector2{255, 1186}, &Window, &MapBG, Textures, 2.5f
        // };
        // Crows.emplace_back(CrowThirtysix);

        // Enemy CrowThirtyseven
        // {
        //     Sprite{Textures.CrowIdle, 8, 2},
        //     Sprite{Textures.CrowIdleTwo, 5, 2},
        //     Sprite{Textures.CrowFlying, 5, 2},
        //     Sprite{Textures.CrowWalk, 4, 2},
        //     Sprite{Textures.CrowIdleTwo, 5, 2},
        //     EnemyType::CROW, Vector2{154, 1563}, &Window, &MapBG, Textures, 2.5f
        // };
        // Crows.emplace_back(CrowThirtyseven);

        // Enemy CrowThirtyeight
        // {
        //     Sprite{Textures.CrowIdle, 8, 2},
        //     Sprite{Textures.CrowIdleTwo, 5, 2},
        //     Sprite{Textures.CrowFlying, 5, 2},
        //     Sprite{Textures.CrowWalk, 4, 2},
        //     Sprite{Textures.CrowIdleTwo, 5, 2},
        //     EnemyType::CROW, Vector2{860, 363}, &Window, &MapBG, Textures, 2.5f
        // };
        // Crows.emplace_back(CrowThirtyeight);

        // Enemy CrowThirtynine
        // {
        //     Sprite{Textures.CrowIdle, 8, 2},
        //     Sprite{Textures.CrowIdleTwo, 5, 2},
        //     Sprite{Textures.CrowFlying, 5, 2},
        //     Sprite{Textures.CrowWalk, 4, 2},
        //     Sprite{Textures.CrowIdleTwo, 5, 2},
        //     EnemyType::CROW, Vector2{2183, 1059}, &Window, &MapBG, Textures, 2.5f
        // };
        // Crows.emplace_back(CrowThirtynine);

        // Enemy CrowForty
        // {
        //     Sprite{Textures.CrowIdle, 8, 2},
        //     Sprite{Textures.CrowIdleTwo, 5, 2},
        //     Sprite{Textures.CrowFlying, 5, 2},
        //     Sprite{Textures.CrowWalk, 4, 2},
        //     Sprite{Textures.CrowIdleTwo, 5, 2},
        //     EnemyType::CROW, Vector2{2761, 560}, &Window, &MapBG, Textures, 2.5f
        // };
        // Crows.emplace_back(CrowForty);

        // Enemy CrowFortyone
        // {
        //     Sprite{Textures.CrowIdle, 8, 2},
        //     Sprite{Textures.CrowIdleTwo, 5, 2},
        //     Sprite{Textures.CrowFlying, 5, 2},
        //     Sprite{Textures.CrowWalk, 4, 2},
        //     Sprite{Textures.CrowIdleTwo, 5, 2},
        //     EnemyType::CROW, Vector2{1647, 563}, &Window, &MapBG, Textures, 2.5f
        // };
        // Crows.emplace_back(CrowFortyone);

        return Crows;
    }

    // Debugging --------------------
    void DrawCollisionRecs(Prop& Prop, Vector2 CharacterWorldPos, Color RecColor)
    {
        if (Prop.IsInteractable()) {
            DrawRectangle(
                Prop.GetInteractRec(CharacterWorldPos).x,
                Prop.GetInteractRec(CharacterWorldPos).y,
                Prop.GetInteractRec(CharacterWorldPos).width,
                Prop.GetInteractRec(CharacterWorldPos).height, 
                CLITERAL(Color){ 200, 122, 255, 100 }        
            );

            DrawRectangle(
                Prop.GetCollisionRec(CharacterWorldPos).x,
                Prop.GetCollisionRec(CharacterWorldPos).y,
                Prop.GetCollisionRec(CharacterWorldPos).width,
                Prop.GetCollisionRec(CharacterWorldPos).height, 
                CLITERAL(Color){ 51, 255, 255, 100 }
            );
        }
        else {
            DrawRectangle(
                Prop.GetCollisionRec(CharacterWorldPos).x,
                Prop.GetCollisionRec(CharacterWorldPos).y,
                Prop.GetCollisionRec(CharacterWorldPos).width,
                Prop.GetCollisionRec(CharacterWorldPos).height, 
                RecColor
            );
        }
    }

    template <typename Object>
    void DrawCollisionRecs(Object& Type, Color Color)
    {   
        DrawRectangle(
            Type.GetCollisionRec().x,
            Type.GetCollisionRec().y,
            Type.GetCollisionRec().width,
            Type.GetCollisionRec().height, 
            Color
        );
    }

    template <typename Object>
    void DrawAttackRecs(Object& Type, Color Color)
    {
        if (Type.IsAttacking())
        {
            DrawRectangle(
                Type.GetAttackRec().x,
                Type.GetAttackRec().y,
                Type.GetAttackRec().width,
                Type.GetAttackRec().height,
                Color
            );
        }
    }

} // namespace Game