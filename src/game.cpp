#include "game.hpp"

namespace Game 
{
    void Run() 
    {
        Window Window{1280, 720}; 
        Game::Initialize(Window, "Cryptex Adventure");

        if (IsWindowReady()) {
            // Initialization ---------------------------
            GameTexture Textures;
            Randomizer RandomEngine{};
            Game::Info Info{Background{Textures}, 0, 0.f, 0.f, 0.f};
            Game::Objects Objects{
                Game::InitializeHud(Textures), 
                Game::InitializeFox(Window, Info, Textures),
                {Game::InitializePropsUnder(Textures), Game::InitializePropsOver(Textures)},
                {Game::InitializeEnemies(Info.Map, Window, Textures, RandomEngine)},
                {Game::InitializeCrows(Info.Map, Window, Textures, RandomEngine)},
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

    void Initialize(const Window& Window, const std::string& Title)
    {
        SetTraceLogLevel(LOG_WARNING);
        InitWindow(Window.x, Window.y, Title.c_str());
        SetTargetFPS(144);
        SetExitKey(0);
        HideCursor();
    }

    void CheckScreenSizing(Window& Window)
    {
        if (IsWindowResized()) {
            Window.x = GetScreenWidth();
            Window.y = GetScreenHeight();
        }

        if (IsKeyPressed(KEY_ENTER) && ((IsKeyDown(KEY_RIGHT_ALT) || IsKeyDown(KEY_LEFT_ALT)))) {
            ToggleFullscreen();
        }
    }

    void Tick(Window& Window, Game::Info& Info, Game::Objects& Objects, const GameTexture& Textures)
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
        Objects.Fox.Tick(DeltaTime, Objects.PropsContainer, Objects.Enemies, Objects.Trees);

        if (!Objects.Fox.IsAlive()) {
            Info.NextState = Game::State::GAMEOVER;
            Info.State = Game::State::TRANSITION;
        }

        for (auto& Enemy:Objects.Enemies) {
            Enemy.Tick(DeltaTime, Objects.PropsContainer, Objects.Fox.GetWorldPos(), Objects.Fox.GetCharPos(), Objects.Enemies, Objects.Trees);
        }

        for (auto& Crow:Objects.Crows) {
            Crow.Tick(DeltaTime, Objects.PropsContainer, Objects.Fox.GetWorldPos(), Objects.Fox.GetCharPos(), Objects.Enemies, Objects.Trees);
        }

        for (auto& Proptype:Objects.PropsContainer.Under) {
            for (auto& Prop:Proptype) {
                Prop.Tick(DeltaTime);
            }
        }

        for (auto& Tree:Objects.Trees) {
            Tree.Tick(DeltaTime);
        }

        for (auto& Proptype:Objects.PropsContainer.Over) {
            for (auto& Prop:Proptype) {
                Prop.Tick(DeltaTime);
            }
        }

        if (IsKeyPressed(KEY_L)) {
            Objects.Fox.SetSleep();
        }

        if (IsKeyPressed(KEY_P)) {
            Info.NextState = Game::State::PAUSED;
            Info.State = Game::State::TRANSITION;
        }
        else if (IsKeyPressed(KEY_F5) || IsKeyPressed(KEY_PERIOD) || IsKeyPressed(KEY_ESCAPE)) {
            Info.NextState = Game::State::EXIT;
            Info.State = Game::State::TRANSITION;
        }

        // Dev Tools--------------------------------------
        if (IsKeyPressed(KEY_GRAVE)) {
            Info.DevToolsOn = !Info.DevToolsOn;
        }

        if (Info.DevToolsOn) {
            if (IsKeyPressed(KEY_ONE)) {
                Info.NoClipOn = !Info.NoClipOn;
                Objects.Fox.SwitchCollidable();
            }
            else if (IsKeyPressed(KEY_TWO)) {
                Info.DrawRectanglesOn = !Info.DrawRectanglesOn;
            }
            else if (IsKeyPressed(KEY_THREE)) {
                Info.ShowFPS = !Info.ShowFPS;
            }
            else if (IsKeyPressed(KEY_FOUR)) {
                Info.ShowPos = !Info.ShowPos;
            }
            else if (IsKeyPressed(KEY_ZERO)) {
                Info.ShowDevTools = !Info.ShowDevTools;
            }
            else if (Objects.Fox.GetHealth() < 11) {
                if (IsKeyPressed(KEY_EQUAL)) {
                    Objects.Fox.AddHealth(0.5f);
                }
            }
            else if (Objects.Fox.GetHealth() > 0) {
                if (IsKeyPressed(KEY_MINUS)) {
                    Objects.Fox.AddHealth(-0.5f);
                }
            }
        }
        else {
            if (Info.NoClipOn) {
                Objects.Fox.SwitchCollidable();
            }
                
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

        for (auto& PropType:Objects.PropsContainer.Under) {
            for (auto& Prop:PropType) {
                Prop.Draw(Objects.Fox.GetWorldPos());
                
                // Debugging --------------------
                if (Info.DrawRectanglesOn) {
                    if (Prop.WithinScreen(Objects.Fox.GetWorldPos())) {
                        Game::DrawCollisionRecs(Prop, Objects.Fox.GetWorldPos());
                    }
                }
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
                    Game::DrawCollisionRecs(Enemy, Color{ 205, 0, 255, 150 });
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

        for (auto& PropType:Objects.PropsContainer.Over) {
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
                DrawRectangle(15, 145, 150, 50, Color{0,0,0,170});
                DrawText(TextFormat("Player.x: %i", (int)Objects.Fox.GetWorldPos().x + 615.f), 20, 150, 20, WHITE);
                DrawText(TextFormat("Player.y: %i", (int)Objects.Fox.GetWorldPos().y + 335.f), 20, 170, 20, WHITE);
            }

            if (Info.ShowFPS) {
                DrawRectangle(15, 215, 90, 30, Color{0,0,0,170});
                DrawFPS(20, 221);
            }
            
            if (Info.ShowDevTools) {
                DrawRectangle(15, 270, 220, 25, Color{0,0,0,170});
                DrawText("   Dev Tools Menu", 20, 273, 20, WHITE);
                DrawRectangle(15, 300, 220, 240, Color{0,0,0,170});
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

    void PauseDraw(const Game::Info& Info, Game::Objects& Objects, const GameTexture& Textures)
    {
        DrawTextureEx(Textures.PauseBackground, Vector2{0.f,0.f}, 0.f, 4.f, WHITE);

        // PauseFoxIndex controls which Fox sprite is drawn
        DrawTexturePro(Objects.PauseFox.at(Info.PauseFoxIndex).Texture, Objects.PauseFox.at(Info.PauseFoxIndex).GetSourceRec(), Objects.PauseFox.at(Info.PauseFoxIndex).GetPosRec(Vector2{674.f,396.f}, 4.f), Vector2{}, 0.f, WHITE);

        // Draw Buttons Depending on which are pushed
        if (IsKeyDown(KEY_W)) {
            DrawTextureEx(Objects.Buttons.at(0), Vector2{208.f,124.f}, 0.f, 4.f, WHITE);
        }
        if (IsKeyDown(KEY_A)) {
            DrawTextureEx(Objects.Buttons.at(1), Vector2{160.f,180.f}, 0.f, 4.f, WHITE);
        }
        if (IsKeyDown(KEY_S)) {
            DrawTextureEx(Objects.Buttons.at(2), Vector2{208.f,180.f}, 0.f, 4.f, WHITE);
        }
        if (IsKeyDown(KEY_D)) {
            DrawTextureEx(Objects.Buttons.at(3), Vector2{256.f,180.f}, 0.f, 4.f, WHITE);
        }
        if (IsKeyDown(KEY_L)) {
            DrawTextureEx(Objects.Buttons.at(4), Vector2{160.f,460.f}, 0.f, 4.f, WHITE);
        }
        if (IsKeyDown(KEY_M)) {
            DrawTextureEx(Objects.Buttons.at(5), Vector2{160.f,372.f}, 0.f, 4.f, WHITE);
        }
        if (IsKeyDown(KEY_LEFT_SHIFT)) {
            DrawTextureEx(Objects.Buttons.at(6), Vector2{160.f,276.f}, 0.f, 4.f, WHITE);
        }
        if (IsKeyDown(KEY_SPACE)) {
            DrawTextureEx(Objects.Buttons.at(7), Vector2{152.f,552.f}, 0.f, 4.f, WHITE);
        }
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            DrawTextureEx(Objects.Buttons.at(8), Vector2{264.f,548.f}, 0.f, 4.f, WHITE);
        }
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
        if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_S) || IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_DOWN)) {
            Info.GameOverStart = !Info.GameOverStart;
        }

        if (!Info.GameOverStart) {
            if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER)) {
                Info.NextState = Game::State::RUNNING;
                Info.State = Game::State::TRANSITION;    
            }
        }
        else {
            if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER)) {
                Info.NextState = Game::State::MAINMENU;
                Info.State = Game::State::TRANSITION;
            }
        }
    }

    void GameOverDraw(const Game::Info& Info)
    {
        DrawText("GAME OVER", 500, 240, 40, WHITE);
        DrawText("RESUME", 563, 380, 30, WHITE);
        DrawText("END", 597, 430, 30, WHITE);
        
        if (!Info.GameOverStart) {
            DrawRectangle(557, 370, 138, 47, Color{ 0, 238, 135, 100 });
        }
        else {
            DrawRectangle(557, 420, 138, 47, Color{ 0, 238, 135, 100 });
        }
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

    HUD InitializeHud(const GameTexture& Textures)
    {
        return HUD(Textures);
    }

    Character InitializeFox(const Window& Window, Game::Info& Info, const GameTexture& Textures)
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
            Window, Info.Map
        };
    }

    std::array<Sprite,5> InitializePauseFox(const GameTexture& Textures)
    {
        return std::array<Sprite, 5>{
            Sprite{Textures.FoxIdle, 4, 4},
            Sprite{Textures.FoxWalk, 4, 4},
            Sprite{Textures.FoxRun, 4, 4},
            Sprite{Textures.FoxSleeping, 4, 1},
            Sprite{Textures.FoxMelee, 4, 1}
        };
    }

    std::array<const Texture2D,9> InitializeButtons(const GameTexture& Textures)
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

    std::vector<std::vector<Prop>> InitializePropsUnder(const GameTexture& Textures)
    {
        std::vector<std::vector<Prop>> Props{};

        /*  Placeholder prop to allow "!" to turn off. Need to figure out a better solution to this later.
            Removing Placeholder will make "!" keep showing even if you walk out of an interactable range.
            It will also introduce many bugs such as getting permanantly stuck in place if space is pressed when not in an interactable range.
        */
        std::vector<Prop> Placeholder
        {
            Prop{Textures.Placeholder, Vector2{0.f,0.f}, PropType::PLACEHOLDER, Textures, 4.f, false, true}
        };
        Props.emplace_back(Placeholder);

        std::vector<Prop> DungeonEntrance
        {
            Prop{Textures.DungeonEntrance, Vector2{3520.f,60.f}, PropType::DUNGEONLEFT, Textures, 4.f},
            Prop{Textures.DungeonEntrance, Vector2{3520.f,60.f}, PropType::DUNGEONRIGHT, Textures, 4.f},
            Prop{Textures.DungeonEntrance, Vector2{3520.f,60.f}, PropType::DUNGEON, Textures, 4.f, false, true},
        };
        Props.emplace_back(DungeonEntrance);

        std::vector<Prop> Grass
        {   
            // --------------------------------------------- Far Left Column -----------------------------------------------
            // ---------------------------------------------------- -4 ------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1816.f,1806.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1816.f,1846.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1880.f,1806.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1880.f,1846.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1816.f,1870.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1816.f,1910.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1880.f,1870.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1880.f,1910.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // ---------------------------------------------------- -3 ------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1816.f,1678.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1816.f,1718.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1880.f,1678.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1880.f,1718.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1816.f,1742.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1816.f,1782.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1880.f,1742.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1880.f,1782.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // ---------------------------------------------------- -2 ------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1816.f,1614.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1816.f,1654.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1880.f,1614.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1880.f,1654.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // ---------------------------------------------------- -1 ------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2008.f,1358.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2008.f,1398.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // ----------------------------------------------------- 0 ------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1944.f,1442.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1944.f,1462.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2008.f,1442.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2008.f,1462.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1944.f,1486.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1944.f,1526.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2008.f,1486.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2008.f,1526.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // ----------------------------------------------------- 1 ------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1944.f,1550.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1944.f,1590.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2008.f,1550.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2008.f,1590.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1944.f,1614.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1944.f,1654.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2008.f,1614.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2008.f,1654.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // ----------------------------------------------------- 2 ------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1944.f,1678.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1944.f,1718.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2008.f,1678.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2008.f,1718.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1944.f,1742.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1944.f,1782.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2008.f,1742.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2008.f,1782.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // ------------------------------------------------------ 3 ------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1944.f,1806.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1944.f,1846.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2008.f,1806.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2008.f,1846.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1944.f,1870.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1944.f,1910.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2008.f,1870.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2008.f,1910.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // ------------------------------------------------------ 3 ------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1944.f,1806.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1944.f,1846.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2008.f,1806.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2008.f,1846.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1944.f,1870.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1944.f,1910.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2008.f,1870.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2008.f,1910.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // ----------------------------------------------------- 4 ------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1944.f,1934.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1944.f,1974.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2008.f,1934.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2008.f,1974.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1944.f,1998.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1944.f,2038.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2008.f,1998.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2008.f,2038.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // ------------------------------------------------ Left Column ------------------------------------------------
            // ---------------------------------------------------- -1 ------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2072.f,1294.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2072.f,1334.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2136.f,1294.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2136.f,1334.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2072.f,1358.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2072.f,1398.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2136.f,1358.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2136.f,1398.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // ---------------------------------------------------- 0 ------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2072.f,1422.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2072.f,1462.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2136.f,1422.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2136.f,1462.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2072.f,1486.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2072.f,1526.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2136.f,1486.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2136.f,1526.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // ---------------------------------------------------- 1 ------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2072.f,1550.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2072.f,1590.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2136.f,1550.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2136.f,1590.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2072.f,1614.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2072.f,1654.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2136.f,1614.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2136.f,1654.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------- 2 -------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2072.f,1678.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2072.f,1718.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2136.f,1678.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2136.f,1718.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2072.f,1742.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2072.f,1782.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2136.f,1742.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2136.f,1782.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------- 3 -------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2072.f,1806.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2072.f,1846.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2136.f,1806.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2136.f,1846.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2072.f,1870.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2072.f,1910.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2136.f,1870.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2136.f,1910.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------- 4 -------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2072.f,1934.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2072.f,1974.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2136.f,1934.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2136.f,1974.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2072.f,1998.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2072.f,2038.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2136.f,1998.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2136.f,2038.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------- Far Right Column ----------------------------------------------
            // --------------------------------------------------- -6 -------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2456.f,1934.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2456.f,1974.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2520.f,1934.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2520.f,1974.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2456.f,1998.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2456.f,2038.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2520.f,1998.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------- -5 -------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2456.f,1806.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2456.f,1846.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2520.f,1806.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2520.f,1846.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2456.f,1870.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2456.f,1910.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2520.f,1870.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2520.f,1910.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------- -4 -------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2456.f,1678.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2456.f,1718.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2520.f,1678.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2520.f,1718.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2456.f,1742.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2456.f,1782.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2520.f,1742.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2520.f,1782.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------- -3 -------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2456.f,1614.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2456.f,1654.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2520.f,1614.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2520.f,1654.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------- -2 -------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2456.f,1526.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2520.f,1486.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2520.f,1526.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------- -1 -------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2328.f,1334.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2392.f,1294.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2392.f,1334.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2328.f,1358.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2328.f,1398.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2392.f,1358.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2392.f,1398.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------- 0 -------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2328.f,1422.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2328.f,1462.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2392.f,1422.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2392.f,1462.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2328.f,1486.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2328.f,1526.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2392.f,1486.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2392.f,1526.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------- 1 -------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2328.f,1550.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2328.f,1590.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2392.f,1550.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2392.f,1590.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2328.f,1614.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2328.f,1654.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2392.f,1614.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2392.f,1654.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------- 2 -------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2328.f,1678.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2328.f,1718.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2392.f,1678.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2392.f,1718.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2328.f,1742.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2328.f,1782.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2392.f,1742.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2392.f,1782.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------- 3 -------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2328.f,1806.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2328.f,1846.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2392.f,1806.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2392.f,1846.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2328.f,1870.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2328.f,1910.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2392.f,1870.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2392.f,1910.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------- 4 -------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2328.f,1934.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2328.f,1974.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2392.f,1934.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2392.f,1974.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2328.f,1998.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2328.f,2038.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2392.f,1998.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2392.f,2038.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // ------------------------------------------------ Right Column -----------------------------------------------
            // --------------------------------------------------- -1 -------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2200.f,1334.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2264.f,1334.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2200.f,1358.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2200.f,1398.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2264.f,1358.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2264.f,1398.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------- 0 -------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2200.f,1422.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2200.f,1462.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2264.f,1422.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2264.f,1462.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2200.f,1486.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2200.f,1526.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2264.f,1486.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2264.f,1526.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------- 1 -------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2200.f,1550.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2200.f,1590.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2264.f,1550.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2264.f,1590.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2200.f,1614.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2200.f,1654.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2264.f,1614.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2264.f,1654.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------- 2 -------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2200.f,1678.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2200.f,1718.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2264.f,1678.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2264.f,1718.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2200.f,1742.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2200.f,1782.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2264.f,1742.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2264.f,1782.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------- 3 -------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2200.f,1806.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2200.f,1846.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2264.f,1806.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2264.f,1846.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2200.f,1870.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2200.f,1910.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2264.f,1870.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2264.f,1910.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------- 4 -------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2200.f,1934.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2200.f,1974.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2264.f,1934.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2264.f,1974.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2200.f,1998.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2200.f,2038.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2264.f,1998.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2264.f,2038.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------------------------------------------------------------------
            // -----------------------------------------Dungeon Entrance Grass-----------------------------------------------
            // --------------------------------------------------------------------------------------------------------------
            // ------------------------------------------------ Left Column -------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500.f,170.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500.f,210.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564.f,170.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564.f,210.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500.f,234.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500.f,274.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564.f,234.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564.f,274.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------------------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500.f,298.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500.f,338.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564.f,298.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564.f,338.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500.f,362.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500.f,402.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564.f,362.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564.f,402.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------------------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500.f,426.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500.f,446.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564.f,426.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564.f,446.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500.f,490.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500.f,530.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564.f,490.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564.f,530.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------------------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500.f,554.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500.f,594.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564.f,554.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564.f,594.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500.f,618.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500.f,658.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564.f,618.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564.f,658.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------------------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500.f,682.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500.f,722.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564.f,682.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564.f,722.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500.f,746.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500.f,786.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564.f,746.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564.f,786.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------------------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500.f,810.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500.f,850.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564.f,810.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564.f,850.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500.f,874.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500.f,914.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564.f,874.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564.f,914.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------------------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500.f,938.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500.f,978.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564.f,938.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564.f,978.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500.f,1002.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500.f,1042.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564.f,1002.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564.f,1042.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------------------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500.f,1066.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500.f,1106.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564.f,1066.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564.f,1106.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500.f,1130.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500.f,1170.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564.f,1130.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564.f,1170.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------------------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500.f,1194.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500.f,1234.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564.f,1194.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564.f,1234.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500.f,1258.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500.f,1298.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564.f,1258.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564.f,1298.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------------------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500.f,1322.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500.f,1362.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564.f,1322.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564.f,1362.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500.f,1386.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500.f,1426.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564.f,1386.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564.f,1426.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------------------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500.f,1450.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500.f,1490.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564.f,1450.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564.f,1490.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500.f,1514.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500.f,1554.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564.f,1514.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564.f,1554.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------------------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500.f,1578.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500.f,1618.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564.f,1578.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564.f,1618.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500.f,1642.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500.f,1682.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564.f,1642.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564.f,1682.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------------------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500.f,1706.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500.f,1746.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564.f,1706.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564.f,1746.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500.f,1770.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500.f,1810.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564.f,1770.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564.f,1810.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------------------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500.f,1834.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500.f,1874.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564.f,1834.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564.f,1874.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500.f,1898.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500.f,1938.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564.f,1898.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564.f,1938.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------------------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500.f,1962.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500.f,2002.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564.f,1962.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564.f,2002.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500.f,2026.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3500.f,2046.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564.f,2026.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3564.f,2046.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // ------------------------------------------------ Right Column -------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628.f,170.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628.f,210.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692.f,170.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692.f,210.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628.f,234.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628.f,274.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692.f,234.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692.f,274.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------------------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628.f,298.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628.f,338.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692.f,298.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692.f,338.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628.f,362.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628.f,402.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692.f,362.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692.f,402.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------------------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628.f,426.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628.f,446.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692.f,426.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692.f,446.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628.f,490.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628.f,530.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692.f,490.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692.f,530.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------------------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628.f,554.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628.f,594.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692.f,554.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692.f,594.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628.f,618.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628.f,658.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692.f,618.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692.f,658.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------------------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628.f,682.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628.f,722.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692.f,682.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692.f,722.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628.f,746.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628.f,786.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692.f,746.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692.f,786.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------------------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628.f,810.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628.f,850.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692.f,810.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692.f,850.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628.f,874.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628.f,914.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692.f,874.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692.f,914.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------------------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628.f,938.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628.f,978.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692.f,938.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692.f,978.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628.f,1002.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628.f,1042.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692.f,1002.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692.f,1042.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------------------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628.f,1066.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628.f,1106.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692.f,1066.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692.f,1106.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628.f,1130.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628.f,1170.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692.f,1130.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692.f,1170.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------------------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628.f,1194.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628.f,1234.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692.f,1194.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692.f,1234.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628.f,1258.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628.f,1298.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692.f,1258.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692.f,1298.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------------------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628.f,1322.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628.f,1362.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692.f,1322.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692.f,1362.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628.f,1386.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628.f,1426.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692.f,1386.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692.f,1426.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------------------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628.f,1450.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628.f,1490.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692.f,1450.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692.f,1490.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628.f,1514.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628.f,1554.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692.f,1514.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692.f,1554.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------------------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628.f,1578.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628.f,1618.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692.f,1578.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692.f,1618.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628.f,1642.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628.f,1682.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692.f,1642.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692.f,1682.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------------------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628.f,1706.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628.f,1746.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692.f,1706.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692.f,1746.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628.f,1770.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628.f,1810.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692.f,1770.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692.f,1810.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------------------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628.f,1834.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628.f,1874.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692.f,1834.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692.f,1874.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628.f,1898.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628.f,1938.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692.f,1898.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692.f,1938.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------------------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628.f,1962.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628.f,2002.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692.f,1962.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692.f,2002.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628.f,2026.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3628.f,2046.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692.f,2026.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3692.f,2046.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
        };
        Props.emplace_back(Grass);

        std::vector<Prop> Walls
        {
            // ---------------------------------------- Left Side Hill -----------------------------------
            // Left Side Walls
            Prop{Textures.GrassWallLeft, Vector2{768.f, 2688.f}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.GrassWallLeft, Vector2{768.f, 2752.f}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.GrassWallLeft, Vector2{768.f, 2816.f}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.GrassWallLeft, Vector2{768.f, 2880.f}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.GrassWallLeft, Vector2{768.f, 2944.f}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.GrassWallLeft, Vector2{768.f, 3008.f}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.GrassWallLeft, Vector2{768.f, 3072.f}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.GrassWallLeft, Vector2{768.f, 3136.f}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.GrassWallLeft, Vector2{768.f, 3200.f}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.GrassWallLeft, Vector2{768.f, 3264.f}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.GrassWallLeft, Vector2{768.f, 3328.f}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.GrassWallLeft, Vector2{768.f, 3392.f}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.GrassWallLeft, Vector2{768.f, 3456.f}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.GrassWallLeft, Vector2{768.f, 3520.f}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.GrassWallBotLeft, Vector2{768.f, 3584.f}, PropType::BOTTOMWALL, Textures},
            // Bottom Walls
            Prop{Textures.GrassWallBottom, Vector2{896.f, 3584.f}, PropType::BOTTOMWALL, Textures},
            Prop{Textures.GrassWallBottom, Vector2{960.f, 3584.f}, PropType::BOTTOMWALL, Textures},
            Prop{Textures.GrassWallBottom, Vector2{832.f, 3584.f}, PropType::BOTTOMWALL, Textures},
            Prop{Textures.GrassWallBottom, Vector2{1024.f, 3584.f}, PropType::BOTTOMWALL, Textures},
            Prop{Textures.GrassWallBottom, Vector2{1216.f, 3584.f}, PropType::BOTTOMWALL, Textures},
            Prop{Textures.GrassWallBotRight, Vector2{1280.f, 3584.f}, PropType::BOTTOMWALL, Textures},
            // Bottom Right Walls
            Prop{Textures.GrassWallRight, Vector2{1280.f, 3520.f}, PropType::RIGHTSIDEWALL, Textures},
            Prop{Textures.GrassWallRight, Vector2{1280.f, 3456.f}, PropType::RIGHTSIDEWALL, Textures},
            Prop{Textures.GrassWallRight, Vector2{1280.f, 3392.f}, PropType::RIGHTSIDEWALL, Textures},
            Prop{Textures.GrassWallRight, Vector2{1280.f, 3328.f}, PropType::RIGHTSIDEWALL, Textures},
            Prop{Textures.GrassWallInnerRight, Vector2{1280.f, 3264.f}, PropType::RIGHTSIDEWALL, Textures},
            // Bottom Walls
            Prop{Textures.GrassWallBottom, Vector2{1344.f, 3264.f}, PropType::BOTTOMWALL, Textures},
            Prop{Textures.GrassWallBottom, Vector2{1408.f, 3264.f}, PropType::BOTTOMWALL, Textures},
            Prop{Textures.GrassWallBottom, Vector2{1472.f, 3264.f}, PropType::BOTTOMWALL, Textures},
            Prop{Textures.GrassWallBottom, Vector2{1536.f, 3264.f}, PropType::BOTTOMWALL, Textures},
            Prop{Textures.GrassWallBottom, Vector2{1600.f, 3264.f}, PropType::BOTTOMWALL, Textures},
            Prop{Textures.GrassWallBottom, Vector2{1664.f, 3264.f}, PropType::BOTTOMWALL, Textures},
            Prop{Textures.GrassWallBottom, Vector2{1728.f, 3264.f}, PropType::BOTTOMWALL, Textures},
            Prop{Textures.GrassWallBottom, Vector2{1792.f, 3264.f}, PropType::BOTTOMWALL, Textures},
            Prop{Textures.GrassWallBottom, Vector2{1856.f, 3264.f}, PropType::BOTTOMWALL, Textures},
            Prop{Textures.GrassWallBotRight, Vector2{1920.f, 3264.f}, PropType::BOTTOMWALL, Textures},     
            // Right Side Walls
            Prop{Textures.GrassWallRight, Vector2{1920.f, 3200.f}, PropType::RIGHTSIDEWALL, Textures},
            Prop{Textures.GrassWallRight, Vector2{1920.f, 3136.f}, PropType::RIGHTSIDEWALL, Textures},
            Prop{Textures.GrassWallRight, Vector2{1920.f, 3072.f}, PropType::RIGHTSIDEWALL, Textures},
            Prop{Textures.GrassWallRight, Vector2{1920.f, 3008.f}, PropType::RIGHTSIDEWALL, Textures},
            Prop{Textures.GrassWallRight, Vector2{1920.f, 2944.f}, PropType::RIGHTSIDEWALL, Textures},
            Prop{Textures.GrassWallRight, Vector2{1920.f, 2880.f}, PropType::RIGHTSIDEWALL, Textures},
            Prop{Textures.GrassWallRight, Vector2{1920.f, 2816.f}, PropType::RIGHTSIDEWALL, Textures},
            Prop{Textures.GrassWallRight, Vector2{1920.f, 2752.f}, PropType::RIGHTSIDEWALL, Textures},
            Prop{Textures.GrassWallRight, Vector2{1920.f, 2688.f}, PropType::RIGHTSIDEWALL, Textures},
            Prop{Textures.GrassWallTopRight, Vector2{1920.f, 2624.f}, PropType::TOPWALL, Textures},
            // Top Walls
            Prop{Textures.GrassWallTop, Vector2{1856.f, 2624.f}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{1792.f, 2624.f}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{1728.f, 2624.f}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{1664.f, 2624.f}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{1600.f, 2624.f}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{1536.f, 2624.f}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{1472.f, 2624.f}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{1408.f, 2624.f}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{1344.f, 2624.f}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{1280.f, 2624.f}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{1216.f, 2624.f}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{1152.f, 2624.f}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{1088.f, 2624.f}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{1024.f, 2624.f}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{960.f, 2624.f}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{896.f, 2624.f}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{832.f, 2624.f}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTopLeft, Vector2{768.f, 2624.f}, PropType::TOPWALL, Textures},
            // Fences
            Prop{Textures.FenceDown, Vector2{832.f, 3662.f}, PropType::FENCE, Textures},
            Prop{Textures.FenceDown, Vector2{896.f, 3662.f}, PropType::FENCE, Textures},
            Prop{Textures.FenceDown, Vector2{960.f, 3662.f}, PropType::FENCE, Textures},
            Prop{Textures.FenceDown, Vector2{1024.f, 3662.f}, PropType::FENCE, Textures},
            Prop{Textures.FenceDown, Vector2{1216.f, 3662.f}, PropType::FENCE, Textures},
            // ---------------------------------------- Garden Fences -----------------------------------
            // Left Side
            Prop{Textures.FenceLeft, Vector2{1688.f, 3070.f}, PropType::FENCE, Textures},
            Prop{Textures.FenceLeft, Vector2{1688.f, 3198.f}, PropType::FENCE, Textures},
            // Right Side
            Prop{Textures.FenceRight, Vector2{1925.f, 3070.f}, PropType::FENCE, Textures},
            Prop{Textures.FenceRight, Vector2{1925.f, 3128.f}, PropType::FENCE, Textures},
            Prop{Textures.FenceRight, Vector2{1925.f, 3198.f}, PropType::FENCE, Textures},
            // Top Side
            Prop{Textures.FenceUp, Vector2{1728.f, 3070.f}, PropType::FENCE, Textures},
            Prop{Textures.FenceUp, Vector2{1856.f, 3070.f}, PropType::FENCE, Textures},
            // Bottom Side
            Prop{Textures.FenceDown, Vector2{1728.f, 3215.f}, PropType::FENCE, Textures},
            Prop{Textures.FenceDown, Vector2{1792.f, 3215.f}, PropType::FENCE, Textures},
            Prop{Textures.FenceDown, Vector2{1856.f, 3215.f}, PropType::FENCE, Textures},
            // ---------------------------------------- Left Side Inner Hill -----------------------------------
            // Holes
            Prop{Textures.Hole, Vector2{960.f, 3008.f}, PropType::HOLE, Textures},
            Prop{Textures.Hole, Vector2{1088.f, 3008.f}, PropType::HOLE, Textures},
            // Left Side Walls
            Prop{Textures.WallLeft, Vector2{896.f, 2816.f}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.WallLeft, Vector2{896.f, 2880.f}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.WallLeft, Vector2{896.f, 2944.f}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.WallLeft, Vector2{896.f, 3008.f}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.WallLeft, Vector2{896.f, 3072.f}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.WallLeft, Vector2{896.f, 3136.f}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.WallLeft, Vector2{896.f, 3200.f}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.WallBotLeft, Vector2{896.f, 3264.f}, PropType::BOTTOMWALL, Textures},
            // Bottom Walls
            Prop{Textures.WallBottom, Vector2{960.f, 3264.f}, PropType::BOTTOMWALL, Textures},
            Prop{Textures.WallBottom, Vector2{1024.f, 3264.f}, PropType::BOTTOMWALL, Textures},
            Prop{Textures.WallBotRight, Vector2{1152.f, 3264.f}, PropType::BOTTOMWALL, Textures},
            // Right Side Walls
            Prop{Textures.WallRight, Vector2{1152.f, 3200.f}, PropType::RIGHTSIDEWALL, Textures},
            Prop{Textures.WallRight, Vector2{1152.f, 3136.f}, PropType::RIGHTSIDEWALL, Textures},
            Prop{Textures.WallRight, Vector2{1152.f, 3072.f}, PropType::RIGHTSIDEWALL, Textures},
            Prop{Textures.WallRight, Vector2{1152.f, 3008.f}, PropType::RIGHTSIDEWALL, Textures},
            Prop{Textures.WallRight, Vector2{1152.f, 2944.f}, PropType::RIGHTSIDEWALL, Textures},
            Prop{Textures.WallRight, Vector2{1152.f, 2880.f}, PropType::RIGHTSIDEWALL, Textures},
            Prop{Textures.WallRight, Vector2{1152.f, 2816.f}, PropType::RIGHTSIDEWALL, Textures},
            Prop{Textures.WallTopRight, Vector2{1152.f, 2752.f}, PropType::TOPWALL, Textures},
            // Top Walls
            Prop{Textures.GrassWallTop, Vector2{1088.f, 2752.f}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{1024.f, 2752.f}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{960.f, 2752.f}, PropType::TOPWALL, Textures},
            Prop{Textures.WallTopLeft, Vector2{896.f, 2752.f}, PropType::TOPWALL, Textures},
            // ---------------------------------------- Right Side Hill -----------------------------------
            // Left Side Walls
            Prop{Textures.GrassWallLeft, Vector2{2688.f, 2560.f}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.GrassWallLeft, Vector2{2688.f, 2624.f}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.GrassWallLeft, Vector2{2688.f, 2688.f}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.GrassWallLeft, Vector2{2688.f, 2752.f}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.GrassWallLeft, Vector2{2688.f, 2816.f}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.GrassWallLeft, Vector2{2688.f, 2880.f}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.GrassWallLeft, Vector2{2688.f, 2944.f}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.GrassWallLeft, Vector2{2688.f, 3008.f}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.GrassWallLeft, Vector2{2688.f, 3072.f}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.GrassWallLeft, Vector2{2688.f, 3136.f}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.GrassWallLeft, Vector2{2688.f, 3200.f}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.GrassWallLeft, Vector2{2688.f, 3264.f}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.GrassWallLeft, Vector2{2688.f, 3328.f}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.GrassWallLeft, Vector2{2688.f, 3392.f}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.GrassWallBotLeft, Vector2{2688.f, 3456.f}, PropType::BOTTOMWALL, Textures},
            // Bottom Walls
            Prop{Textures.GrassWallBottom, Vector2{2752.f, 3456.f}, PropType::BOTTOMWALL, Textures},
            Prop{Textures.GrassWallBottom, Vector2{2816.f, 3456.f}, PropType::BOTTOMWALL, Textures},
            Prop{Textures.GrassWallBottom, Vector2{2880.f, 3456.f}, PropType::BOTTOMWALL, Textures},
            Prop{Textures.GrassWallBottom, Vector2{3264.f, 3456.f}, PropType::BOTTOMWALL, Textures},
            Prop{Textures.GrassWallBottom, Vector2{3328.f, 3456.f}, PropType::BOTTOMWALL, Textures},
            Prop{Textures.GrassWallBottom, Vector2{3392.f, 3456.f}, PropType::BOTTOMWALL, Textures},
            Prop{Textures.GrassWallBottom, Vector2{3456.f, 3456.f}, PropType::BOTTOMWALL, Textures},
            Prop{Textures.GrassWallBottom, Vector2{3520.f, 3456.f}, PropType::BOTTOMWALL, Textures},
            Prop{Textures.GrassWallBottom, Vector2{3584.f, 3456.f}, PropType::BOTTOMWALL, Textures},
            Prop{Textures.GrassWallBottom, Vector2{3648.f, 3456.f}, PropType::BOTTOMWALL, Textures},
            Prop{Textures.GrassWallBottom, Vector2{3712.f, 3456.f}, PropType::BOTTOMWALL, Textures},
            Prop{Textures.GrassWallBottom, Vector2{3776.f, 3456.f}, PropType::BOTTOMWALL, Textures},
            Prop{Textures.GrassWallBotRight, Vector2{3840.f, 3456.f}, PropType::BOTTOMWALL, Textures},
            // Right Side Walls
            Prop{Textures.GrassWallRight, Vector2{3840.f, 3392.f}, PropType::RIGHTSIDEWALL, Textures},
            Prop{Textures.GrassWallRight, Vector2{3840.f, 3328.f}, PropType::RIGHTSIDEWALL, Textures},
            Prop{Textures.GrassWallRight, Vector2{3840.f, 3264.f}, PropType::RIGHTSIDEWALL, Textures},
            Prop{Textures.GrassWallRight, Vector2{3840.f, 3200.f}, PropType::RIGHTSIDEWALL, Textures},
            Prop{Textures.GrassWallRight, Vector2{3840.f, 3136.f}, PropType::RIGHTSIDEWALL, Textures},
            Prop{Textures.GrassWallRight, Vector2{3840.f, 3072.f}, PropType::RIGHTSIDEWALL, Textures},
            Prop{Textures.GrassWallRight, Vector2{3840.f, 3008.f}, PropType::RIGHTSIDEWALL, Textures},
            Prop{Textures.GrassWallRight, Vector2{3840.f, 2944.f}, PropType::RIGHTSIDEWALL, Textures},
            Prop{Textures.GrassWallRight, Vector2{3840.f, 2880.f}, PropType::RIGHTSIDEWALL, Textures},
            Prop{Textures.GrassWallRight, Vector2{3840.f, 2816.f}, PropType::RIGHTSIDEWALL, Textures},
            Prop{Textures.GrassWallRight, Vector2{3840.f, 2752.f}, PropType::RIGHTSIDEWALL, Textures},
            Prop{Textures.GrassWallRight, Vector2{3840.f, 2688.f}, PropType::RIGHTSIDEWALL, Textures},
            Prop{Textures.GrassWallRight, Vector2{3840.f, 2624.f}, PropType::RIGHTSIDEWALL, Textures},
            Prop{Textures.GrassWallRight, Vector2{3840.f, 2560.f}, PropType::RIGHTSIDEWALL, Textures},
            Prop{Textures.GrassWallTopRight, Vector2{3840.f, 2496.f}, PropType::TOPWALL, Textures},
            // Top Side Walls
            Prop{Textures.GrassWallTop, Vector2{3776.f, 2496.f}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{3712.f, 2496.f}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{3648.f, 2496.f}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{3584.f, 2496.f}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{3520.f, 2496.f}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{3456.f, 2496.f}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{3392.f, 2496.f}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{3328.f, 2496.f}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{3264.f, 2496.f}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{3200.f, 2496.f}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{3136.f, 2496.f}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{3072.f, 2496.f}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{3008.f, 2496.f}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{2944.f, 2496.f}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{2880.f, 2496.f}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{2816.f, 2496.f}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{2752.f, 2496.f}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTopLeft, Vector2{2688.f, 2496.f}, PropType::TOPWALL, Textures},
            // ---------------------------------------- Right Side Second Hill -----------------------------------
            // Left Side Walls
            Prop{Textures.GrassWallLeft, Vector2{2752.f, 2624.f}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.GrassWallLeft, Vector2{2752.f, 2688.f}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.GrassWallLeft, Vector2{2752.f, 2752.f}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.GrassWallLeft, Vector2{2752.f, 2816.f}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.GrassWallLeft, Vector2{2752.f, 2880.f}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.GrassWallLeft, Vector2{2752.f, 2944.f}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.GrassWallLeft, Vector2{2752.f, 3008.f}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.GrassWallLeft, Vector2{2752.f, 3072.f}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.GrassWallLeft, Vector2{2752.f, 3136.f}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.GrassWallLeft, Vector2{2752.f, 3200.f}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.GrassWallBotLeft, Vector2{2752.f, 3264.f}, PropType::BOTTOMWALL, Textures},
            // Bottom Walls
            Prop{Textures.GrassWallBottom, Vector2{2816.f, 3264.f}, PropType::BOTTOMWALL, Textures},
            Prop{Textures.GrassWallBottom, Vector2{2880.f, 3264.f}, PropType::BOTTOMWALL, Textures},
            Prop{Textures.GrassWallBottom, Vector2{2944.f, 3264.f}, PropType::BOTTOMWALL, Textures},
            Prop{Textures.GrassWallBottom, Vector2{3136.f, 3264.f}, PropType::BOTTOMWALL, Textures},
            Prop{Textures.GrassWallBottom, Vector2{3200.f, 3264.f}, PropType::BOTTOMWALL, Textures},
            Prop{Textures.GrassWallBottom, Vector2{3264.f, 3264.f}, PropType::BOTTOMWALL, Textures},
            Prop{Textures.GrassWallBottom, Vector2{3328.f, 3264.f}, PropType::BOTTOMWALL, Textures},
            Prop{Textures.GrassWallBottom, Vector2{3392.f, 3264.f}, PropType::BOTTOMWALL, Textures},
            Prop{Textures.GrassWallBottom, Vector2{3456.f, 3264.f}, PropType::BOTTOMWALL, Textures},
            Prop{Textures.GrassWallBottom, Vector2{3520.f, 3264.f}, PropType::BOTTOMWALL, Textures},
            Prop{Textures.GrassWallBottom, Vector2{3584.f, 3264.f}, PropType::BOTTOMWALL, Textures},
            Prop{Textures.GrassWallBottom, Vector2{3648.f, 3264.f}, PropType::BOTTOMWALL, Textures},
            Prop{Textures.GrassWallBotRight, Vector2{3712.f, 3264.f}, PropType::BOTTOMWALL, Textures},
            // Right Side Walls
            Prop{Textures.GrassWallRight, Vector2{3712.f, 3200.f}, PropType::RIGHTSIDEWALL, Textures},
            Prop{Textures.GrassWallRight, Vector2{3712.f, 3136.f}, PropType::RIGHTSIDEWALL, Textures},
            Prop{Textures.GrassWallRight, Vector2{3712.f, 3072.f}, PropType::RIGHTSIDEWALL, Textures},
            Prop{Textures.GrassWallRight, Vector2{3712.f, 3008.f}, PropType::RIGHTSIDEWALL, Textures},
            Prop{Textures.GrassWallRight, Vector2{3712.f, 2944.f}, PropType::RIGHTSIDEWALL, Textures},
            Prop{Textures.GrassWallRight, Vector2{3712.f, 2880.f}, PropType::RIGHTSIDEWALL, Textures},
            Prop{Textures.GrassWallRight, Vector2{3712.f, 2816.f}, PropType::RIGHTSIDEWALL, Textures},
            Prop{Textures.GrassWallRight, Vector2{3712.f, 2752.f}, PropType::RIGHTSIDEWALL, Textures},
            Prop{Textures.GrassWallRight, Vector2{3712.f, 2688.f}, PropType::RIGHTSIDEWALL, Textures},
            Prop{Textures.GrassWallRight, Vector2{3712.f, 2624.f}, PropType::RIGHTSIDEWALL, Textures},
            Prop{Textures.GrassWallTopRight, Vector2{3712.f, 2560.f}, PropType::TOPWALL, Textures},
            // Top Walls
            Prop{Textures.GrassWallTop, Vector2{3648.f, 2560.f}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{3584.f, 2560.f}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{3520.f, 2560.f}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{3456.f, 2560.f}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{3392.f, 2560.f}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{3328.f, 2560.f}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{3264.f, 2560.f}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{3200.f, 2560.f}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{3136.f, 2560.f}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{3072.f, 2560.f}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{3008.f, 2560.f}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{2944.f, 2560.f}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{2880.f, 2560.f}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{2816.f, 2560.f}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTopLeft, Vector2{2752.f, 2560.f}, PropType::TOPWALL, Textures},
            // ---------------------------------------- Right Side Third Hill -----------------------------------
            // Left Side Walls
            Prop{Textures.WallLeft, Vector2{2944.f, 2688.f}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.WallLeft, Vector2{2944.f, 2752.f}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.WallLeft, Vector2{2944.f, 2816.f}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.WallLeft, Vector2{2944.f, 2880.f}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.WallLeft, Vector2{2944.f, 2944.f}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.WallLeft, Vector2{2944.f, 3008.f}, PropType::LEFTSIDEWALL, Textures},
            Prop{Textures.WallBotLeft, Vector2{2944.f, 3072.f}, PropType::BOTTOMWALL, Textures},
            // Bottom Walls
            Prop{Textures.WallBottom, Vector2{3008.f, 3072.f}, PropType::BOTTOMWALL, Textures},
            Prop{Textures.WallBottom, Vector2{3136.f, 3072.f}, PropType::BOTTOMWALL, Textures},
            Prop{Textures.WallBottom, Vector2{3200.f, 3072.f}, PropType::BOTTOMWALL, Textures},
            Prop{Textures.WallBottom, Vector2{3264.f, 3072.f}, PropType::BOTTOMWALL, Textures},
            Prop{Textures.WallBottom, Vector2{3328.f, 3072.f}, PropType::BOTTOMWALL, Textures},
            Prop{Textures.WallBottom, Vector2{3456.f, 3072.f}, PropType::BOTTOMWALL, Textures},
            Prop{Textures.WallBotRight, Vector2{3520.f, 3072.f}, PropType::BOTTOMWALL, Textures},
            // Right Side Walls
            Prop{Textures.WallRight, Vector2{3520.f, 3008.f}, PropType::RIGHTSIDEWALL, Textures},
            Prop{Textures.WallRight, Vector2{3520.f, 2944.f}, PropType::RIGHTSIDEWALL, Textures},
            Prop{Textures.WallRight, Vector2{3520.f, 2880.f}, PropType::RIGHTSIDEWALL, Textures},
            Prop{Textures.WallRight, Vector2{3520.f, 2816.f}, PropType::RIGHTSIDEWALL, Textures},
            Prop{Textures.WallRight, Vector2{3520.f, 2752.f}, PropType::RIGHTSIDEWALL, Textures},
            Prop{Textures.WallRight, Vector2{3520.f, 2688.f}, PropType::RIGHTSIDEWALL, Textures},
            Prop{Textures.WallTopRight, Vector2{3520.f, 2624.f}, PropType::TOPWALL, Textures},
            // Bottom Walls
            Prop{Textures.GrassWallTop, Vector2{3456.f, 2624.f}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{3392.f, 2624.f}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{3328.f, 2624.f}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{3264.f, 2624.f}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{3200.f, 2624.f}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{3136.f, 2624.f}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{3072.f, 2624.f}, PropType::TOPWALL, Textures},
            Prop{Textures.GrassWallTop, Vector2{3008.f, 2624.f}, PropType::TOPWALL, Textures},
            Prop{Textures.WallTopLeft, Vector2{2944.f, 2624.f}, PropType::TOPWALL, Textures},
            // Door
            Prop{Textures.DoorRed, Vector2{1612.f,2815.f}, PropType::DOOR, Textures, 4.f, false, true}, 
            Prop{Textures.DoorBlue, Vector2{3120.f,2685.f}, PropType::DOOR, Textures, 4.f, false, true}
        };
        Props.emplace_back(Walls);

        std::vector<Prop> Moveable
        {
            Prop{Textures.TreeStump, Vector2{3525.f,3640.f}, PropType::STUMP, Textures, 4.f, true},
            Prop{Textures.Boulder, Vector2{1025.f,3050.f}, PropType::BOULDER, Textures, 4.f, true},
        };
        Props.emplace_back(Moveable);
        
        return Props;
    }

    std::vector<std::vector<Prop>> InitializePropsOver(const GameTexture& Textures)
    {
        std::vector<std::vector<Prop>> Props{};

        std::vector<Prop> Houses
        {
            Prop{Textures.HouseRed, Vector2{1472.f,2688.f}, PropType::rHOUSELEFT, Textures},   
            Prop{Textures.HouseRed, Vector2{1472.f,2688.f}, PropType::rHOUSERIGHT, Textures},
            Prop{Textures.HouseBlue, Vector2{3073.f,2495.f}, PropType::bHOUSELEFT, Textures},   
            Prop{Textures.HouseBlue, Vector2{3073.f,2495.f}, PropType::bHOUSERIGHT, Textures},

        };
        Props.emplace_back(Houses);

        std::vector<Prop> Boulder
        {
            Prop{Textures.Boulder, Vector2{960.f,2950.f}, PropType::BOULDER, Textures},
            Prop{Textures.Boulder, Vector2{1090.f,2950.f}, PropType::BOULDER, Textures},
            Prop{Textures.Boulder, Vector2{940.f,3050.f}, PropType::BOULDER, Textures},
            Prop{Textures.Boulder, Vector2{1110.f,3050.f}, PropType::BOULDER, Textures},
        };
        Props.emplace_back(Boulder);

        std::vector<Prop> Moveable
        {
            Prop{Textures.TreeStump, Vector2{3525.f,3640.f}, PropType::STUMP, Textures, 4.f, true},
            Prop{Textures.Boulder, Vector2{1025.f,3050.f}, PropType::BOULDER, Textures, 4.f, true},
        };
        Props.emplace_back(Moveable);

        std::vector<Prop> Grass
        {
            // --------------------------------------------- Far Left Column -----------------------------------------------
            // --------------------------------------------------- -4 -------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1781.f,1826.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1781.f,1866.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1845.f,1826.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1845.f,1866.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1781.f,1890.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1845.f,1890.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------- -3 -------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1781.f,1698.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1781.f,1738.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1845.f,1698.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1845.f,1738.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1781.f,1762.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1781.f,1802.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1845.f,1762.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1845.f,1802.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------- -2 -------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1781.f,1634.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1781.f,1674.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1845.f,1634.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1845.f,1674.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // ---------------------------------------------------- -1 ------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1909.f,1418.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1973.f,1378.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1973.f,1418.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------- 0 -------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1909.f,1442.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1909.f,1482.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1973.f,1442.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1973.f,1482.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1909.f,1546.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1973.f,1506.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1973.f,1546.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------- 1 -------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1909.f,1570.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1909.f,1610.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1973.f,1570.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1973.f,1610.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1909.f,1634.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1909.f,1674.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1973.f,1634.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1973.f,1674.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------- 2 -------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1909.f,1698.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1909.f,1738.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1973.f,1698.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1973.f,1738.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1909.f,1762.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1909.f,1802.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1973.f,1762.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1973.f,1802.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------- 3 -------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1909.f,1826.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1909.f,1866.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1973.f,1826.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1973.f,1866.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1909.f,1890.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1909.f,1930.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1973.f,1890.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{1973.f,1930.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // ------------------------------------------------ Left Column ------------------------------------------------
            // ---------------------------------------------------- -1 ------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2037.f,1314.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2037.f,1354.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2101.f,1314.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2101.f,1354.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2037.f,1378.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2037.f,1418.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2101.f,1378.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2101.f,1418.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // ---------------------------------------------------- 0 ------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2037.f,1442.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2037.f,1482.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2101.f,1442.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2101.f,1482.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2037.f,1506.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2037.f,1546.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2101.f,1506.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2101.f,1546.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------- 1 -------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2037.f,1570.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2037.f,1610.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2101.f,1570.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2101.f,1610.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2037.f,1634.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2037.f,1674.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2101.f,1634.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2101.f,1674.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------- 2 -------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2037.f,1698.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2037.f,1738.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2101.f,1698.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2101.f,1738.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2037.f,1762.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2037.f,1802.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2101.f,1762.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2101.f,1802.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------- 3 -------------------------------------------------------        
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2037.f,1826.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2037.f,1866.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2101.f,1826.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2101.f,1866.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2037.f,1890.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2037.f,1930.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2101.f,1890.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2101.f,1930.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------- 4 ------------------------------------------------------- 
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2037.f,1954.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2037.f,1994.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2101.f,1954.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2101.f,1994.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2037.f,2018.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2037.f,2058.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2101.f,2018.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------- Far Right Column ----------------------------------------------
            // --------------------------------------------------- -6 -------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2421.f,1954.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2421.f,1994.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2485.f,1954.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2485.f,1994.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2421.f,2018.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2421.f,2058.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2485.f,2018.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------- -5 -------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2421.f,1826.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2421.f,1866.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2421.f,1890.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2421.f,1930.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2485.f,1890.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2485.f,1930.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------- -4 -------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2421.f,1698.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2421.f,1738.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2485.f,1698.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2485.f,1738.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2421.f,1762.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2421.f,1802.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------- -3 -------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2421.f,1634.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2421.f,1674.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2485.f,1674.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // ---------------------------------------------------- -2 ------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2421.f,1418.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // ---------------------------------------------------- -1 ------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2293.f,1314.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2293.f,1354.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2357.f,1314.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2357.f,1354.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2293.f,1378.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2293.f,1418.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2357.f,1378.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2357.f,1418.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // ---------------------------------------------------- 0 ------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2293.f,1442.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2293.f,1482.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2357.f,1442.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2357.f,1482.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2293.f,1506.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2293.f,1546.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2357.f,1506.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2357.f,1546.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------- 1 -------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2293.f,1570.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2293.f,1610.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2357.f,1570.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2357.f,1610.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2293.f,1634.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2293.f,1674.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2357.f,1634.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2357.f,1674.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------- 2 -------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2293.f,1698.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2293.f,1738.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2357.f,1698.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2357.f,1738.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2293.f,1762.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2293.f,1802.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2357.f,1762.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2357.f,1802.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------- 3 -------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2293.f,1826.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2293.f,1866.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2357.f,1826.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2357.f,1866.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2293.f,1890.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2293.f,1930.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2357.f,1890.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2357.f,1930.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------- 4 -------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2293.f,1954.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2293.f,1994.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2357.f,1954.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2357.f,1994.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2357.f,2018.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // ------------------------------------------------ Right Column -----------------------------------------------
            // --------------------------------------------------- -1 -------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2165.f,1314.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2165.f,1354.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2229.f,1314.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2229.f,1354.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2165.f,1378.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2165.f,1418.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2229.f,1378.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2229.f,1418.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------- 0 -------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2165.f,1442.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2165.f,1482.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2229.f,1442.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2229.f,1482.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2165.f,1506.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2165.f,1546.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2229.f,1506.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2229.f,1546.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------- 1 -------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2165.f,1570.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2165.f,1610.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2229.f,1570.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2229.f,1610.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2165.f,1634.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2165.f,1674.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2229.f,1634.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2229.f,1674.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------- 2 -------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2165.f,1698.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2165.f,1738.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2229.f,1698.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2229.f,1738.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2165.f,1762.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2165.f,1802.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2229.f,1762.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2229.f,1802.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------- 3 -------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2165.f,1826.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2165.f,1866.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2229.f,1826.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2229.f,1866.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2165.f,1890.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2165.f,1930.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2229.f,1890.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2229.f,1930.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------- 4 -------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2165.f,1954.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2165.f,1994.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2229.f,1954.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2229.f,1994.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2165.f,2018.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2165.f,2058.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{2229.f,2018.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------------------------------------------------------------------
            // -----------------------------------------Dungeon Entrance Grass-----------------------------------------------
            // --------------------------------------------------------------------------------------------------------------
            // ------------------------------------------------ Left Column -------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529.f,150.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529.f,190.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529.f,214.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529.f,254.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------------------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529.f,278.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529.f,318.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529.f,342.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529.f,382.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------------------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529.f,406.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529.f,446.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529.f,470.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529.f,510.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------------------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529.f,534.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529.f,574.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529.f,598.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529.f,638.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------------------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529.f,662.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529.f,702.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529.f,726.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529.f,766.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------------------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529.f,790.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529.f,830.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529.f,854.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529.f,894.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------------------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529.f,918.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529.f,958.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529.f,982.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529.f,1022.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------------------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529.f,1046.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529.f,1086.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529.f,1110.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529.f,1150.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------------------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529.f,1174.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529.f,1214.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529.f,1238.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529.f,1278.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------------------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529.f,1302.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529.f,1342.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529.f,1366.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529.f,1406.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------------------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529.f,1430.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529.f,1470.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529.f,1494.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529.f,1534.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------------------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529.f,1558.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529.f,1598.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529.f,1622.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529.f,1662.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------------------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529.f,1686.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529.f,1726.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529.f,1750.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529.f,1790.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------------------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529.f,1814.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529.f,1854.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529.f,1878.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3529.f,1918.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // ------------------------------------------------ Right Column -------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593.f,150.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593.f,190.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657.f,150.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657.f,190.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593.f,214.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593.f,254.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657.f,214.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657.f,254.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------------------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593.f,278.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593.f,318.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657.f,278.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657.f,318.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593.f,342.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593.f,382.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657.f,342.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657.f,382.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------------------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593.f,406.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593.f,446.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657.f,406.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657.f,446.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593.f,470.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593.f,510.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657.f,470.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657.f,510.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------------------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593.f,534.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593.f,574.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657.f,534.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657.f,574.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593.f,598.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593.f,638.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657.f,598.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657.f,638.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------------------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593.f,662.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593.f,702.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657.f,662.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657.f,702.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593.f,726.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593.f,766.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657.f,726.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657.f,766.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------------------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593.f,790.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593.f,830.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657.f,790.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657.f,830.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593.f,854.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593.f,894.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657.f,854.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657.f,894.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------------------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593.f,918.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593.f,958.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657.f,918.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657.f,958.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593.f,982.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593.f,1022.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657.f,982.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657.f,1022.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------------------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593.f,1046.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593.f,1086.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657.f,1046.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657.f,1086.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593.f,1110.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593.f,1150.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657.f,1110.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657.f,1150.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------------------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593.f,1174.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593.f,1214.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657.f,1174.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657.f,1214.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593.f,1238.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593.f,1278.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657.f,1238.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657.f,1278.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------------------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593.f,1302.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593.f,1342.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657.f,1302.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657.f,1342.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593.f,1366.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593.f,1406.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657.f,1366.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657.f,1406.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------------------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593.f,1430.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593.f,1470.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657.f,1430.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657.f,1470.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593.f,1494.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593.f,1534.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657.f,1494.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657.f,1534.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------------------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593.f,1558.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593.f,1598.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657.f,1558.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657.f,1598.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593.f,1622.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593.f,1662.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657.f,1622.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657.f,1662.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------------------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593.f,1686.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593.f,1726.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657.f,1686.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657.f,1726.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593.f,1750.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593.f,1790.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657.f,1750.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657.f,1790.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            // --------------------------------------------------------------------------------------------------------------
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593.f,1814.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593.f,1854.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657.f,1814.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657.f,1854.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593.f,1878.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3593.f,1918.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657.f,1878.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
            Prop{Sprite{Textures.GrassAnimation, 4, 1, 1.f/3.f}, Vector2{3657.f,1918.f}, PropType::GRASS, Textures, Textures.Placeholder,  4.f, true},
        };
        Props.emplace_back(Grass);

        std::vector<Prop> Treasure
        {
            Prop{Sprite{Textures.TreasureChestBig, 4, 1, 1.f/4.f}, Vector2{2198.f,1677.f}, PropType::BIGTREASURE, Textures, Textures.TreasureHeart,  4.f, false, true, Progress::ACT_O, PropType::NPC_O, "Life Stone", 4.f},
            Prop{Sprite{Textures.TreasureChest, 4, 1, 1.f/4.f}, Vector2{2250.f,3350.f}, PropType::TREASURE, Textures, Textures.AltarTopRight,  4.f, false, true, Progress::ACT_O, PropType::NPC_O, "Top Right Altar Piece"},
            Prop{Sprite{Textures.TreasureChest, 4, 1, 1.f/4.f}, Vector2{2250.f,3150.f}, PropType::TREASURE, Textures, Textures.AltarTop,  4.f, false, true, Progress::ACT_O, PropType::NPC_O, "Top Altar Piece"},
            Prop{Sprite{Textures.TreasureChest, 4, 1, 1.f/4.f}, Vector2{130.f,210.f}, PropType::TREASURE, Textures, Textures.AltarTopLeft,  4.f, false, true, Progress::ACT_O, PropType::NPC_O, "Top Left Altar Piece"},
            Prop{Sprite{Textures.TreasureChest, 4, 1, 1.f/4.f}, Vector2{1025.f,2765.f}, PropType::TREASURE, Textures, Textures.AltarBotLeft,  4.f, false, true, Progress::ACT_O, PropType::NPC_O, "Bottom Left Altar Piece"},
            Prop{Sprite{Textures.TreasureChest, 4, 1, 1.f/4.f}, Vector2{2250.f,2750.f}, PropType::TREASURE, Textures, Textures.AltarBot,  4.f, false, true, Progress::ACT_O, PropType::NPC_O, "Bottom Altar Piece"},
            Prop{Sprite{Textures.TreasureChest, 4, 1, 1.f/4.f}, Vector2{2250.f,2550.f}, PropType::TREASURE, Textures, Textures.AltarTopRight,  4.f, false, true, Progress::ACT_O, PropType::NPC_O, "Bottom Right Altar Piece"},
        };
        Props.emplace_back(Treasure);

        std::vector<Prop> NPCS
        {
            Prop{Sprite{Textures.Didi, 4, 1, 1.f/8.f}, Vector2{3163.f, 2853.f}, PropType::NPC_A, Textures, Textures.Placeholder, 3.f, false, true},
            Prop{Sprite{Textures.Jade, 4, 1, 1.f/8.f}, Vector2{1546.f, 2959.f}, PropType::NPC_B, Textures, Textures.Placeholder, 3.f, false, true},
            Prop{Sprite{Textures.Son, 4, 1, 1.f/8.f}, Vector2{1240.f, 2036.f}, PropType::NPC_C, Textures, Textures.Placeholder, 3.f, false, true, Progress::ACT_II, PropType::NPC_B},
        };
        Props.emplace_back(NPCS);

        std::vector<Prop> AnimatedAltarPieces
        {
            Prop{Sprite{Textures.AltarTopLeftAnimated, 14, 1, 1/14.f}, Vector2{700.f,1025.f}, PropType::ANIMATEDALTAR, Textures, "Top Left Altar Piece", true, true},
            Prop{Sprite{Textures.AltarTopAnimated, 14, 1, 1/14.f}, Vector2{764.f,1025.f}, PropType::ANIMATEDALTAR, Textures, "Top Altar Piece", true, true},
            Prop{Sprite{Textures.AltarTopRightAnimated, 14, 1, 1/14.f}, Vector2{828.f,1025.f}, PropType::ANIMATEDALTAR, Textures, "Top Right Altar Piece", true, true},
            Prop{Sprite{Textures.AltarBotLeftAnimated, 14, 1, 1/14.f}, Vector2{700.f,1089.f}, PropType::ANIMATEDALTAR, Textures, "Bottom Left Altar Piece", true, true},
            Prop{Sprite{Textures.AltarBotAnimated, 14, 1, 1/14.f}, Vector2{764.f,1089.f}, PropType::ANIMATEDALTAR, Textures, "Bottom Altar Piece", true, true},
            Prop{Sprite{Textures.AltarBotRightAnimated, 14, 1, 1/14.f}, Vector2{828.f,1089.f}, PropType::ANIMATEDALTAR, Textures, "Bottom Right Altar Piece", true, true},
        };
        Props.emplace_back(AnimatedAltarPieces);
        
        return Props;
    }

    std::vector<Prop> InitializeTrees(const GameTexture& Textures)
    {
        return std::vector<Prop> {
            // row1
            Prop{Textures.TreeGreen, Vector2{20.f,67.f}, PropType::TREE, Textures},        // 1
            Prop{Textures.TreeGreen, Vector2{120.f,67.f}, PropType::TREE, Textures},       // 2
            Prop{Textures.TreeGreen, Vector2{220.f,67.f}, PropType::TREE, Textures},       // 3
            Prop{Textures.TreeGreen, Vector2{320.f,67.f}, PropType::TREE, Textures},       // 4
            Prop{Textures.TreeGreen, Vector2{420.f,67.f}, PropType::TREE, Textures},       // 5
            Prop{Textures.TreeGreen, Vector2{520.f,67.f}, PropType::TREE, Textures},       // 6
            Prop{Textures.TreeGreen, Vector2{620.f,67.f}, PropType::TREE, Textures},       // 7
            Prop{Textures.TreeGreen, Vector2{720.f,67.f}, PropType::TREE, Textures},       // 8
            Prop{Textures.TreeGreen, Vector2{820.f,67.f}, PropType::TREE, Textures},       // 9
            Prop{Textures.TreeGreen, Vector2{920.f,67.f}, PropType::TREE, Textures},       // 10
            Prop{Textures.TreeGreen, Vector2{1020.f,67.f}, PropType::TREE, Textures},      // 11
            Prop{Textures.TreeGreen, Vector2{1120.f,67.f}, PropType::TREE, Textures},      // 12
            Prop{Textures.TreeGreen, Vector2{1220.f,67.f}, PropType::TREE, Textures},      // 13
            Prop{Textures.TreeGreen, Vector2{1320.f,67.f}, PropType::TREE, Textures},      // 14
            Prop{Textures.TreeGreen, Vector2{1420.f,67.f}, PropType::TREE, Textures},      // 15
            Prop{Textures.TreeGreen, Vector2{1520.f,67.f}, PropType::TREE, Textures},      // 16
            Prop{Textures.TreeGreen, Vector2{1620.f,67.f}, PropType::TREE, Textures},      // 17
            Prop{Textures.TreeGreen, Vector2{1720.f,67.f}, PropType::TREE, Textures},      // 18
            Prop{Textures.TreeGreen, Vector2{1820.f,67.f}, PropType::TREE, Textures},      // 19
            Prop{Textures.TreeGreen, Vector2{1920.f,67.f}, PropType::TREE, Textures},      // 20
            Prop{Textures.TreeGreen, Vector2{2020.f,67.f}, PropType::TREE, Textures},      // 21
            Prop{Textures.TreeGreen, Vector2{2120.f,67.f}, PropType::TREE, Textures},      // 22
            Prop{Textures.TreeGreen, Vector2{2220.f,67.f}, PropType::TREE, Textures},      // 23
            Prop{Textures.TreeGreen, Vector2{2320.f,67.f}, PropType::TREE, Textures},      // 24
            Prop{Textures.TreeGreen, Vector2{2420.f,67.f}, PropType::TREE, Textures},      // 25
            Prop{Textures.TreeGreen, Vector2{2520.f,67.f}, PropType::TREE, Textures},      // 26
            Prop{Textures.TreeGreen, Vector2{2620.f,67.f}, PropType::TREE, Textures},      // 27
            Prop{Textures.TreeGreen, Vector2{2720.f,67.f}, PropType::TREE, Textures},      // 28
            Prop{Textures.TreeGreen, Vector2{2820.f,67.f}, PropType::TREE, Textures},      // 29
            Prop{Textures.TreeGreen, Vector2{2920.f,67.f}, PropType::TREE, Textures},      // 30
            Prop{Textures.TreeGreen, Vector2{3020.f,67.f}, PropType::TREE, Textures},      // 31
            Prop{Textures.TreeGreen, Vector2{3120.f,67.f}, PropType::TREE, Textures},      // 32
            Prop{Textures.TreeGreen, Vector2{3220.f,67.f}, PropType::TREE, Textures},      // 33
            Prop{Textures.TreeGreen, Vector2{3320.f,67.f}, PropType::TREE, Textures},      // 34
            Prop{Textures.TreeGreen, Vector2{3420.f,67.f}, PropType::TREE, Textures},      // 35
            Prop{Textures.TreeGreen, Vector2{3700.f,67.f}, PropType::TREE, Textures},      // 36
            Prop{Textures.TreeGreen, Vector2{3800.f,67.f}, PropType::TREE, Textures},      // 37
            Prop{Textures.TreeGreen, Vector2{3900.f,67.f}, PropType::TREE, Textures},      // 38
            Prop{Textures.TreeGreen, Vector2{4000.f,67.f}, PropType::TREE, Textures},      // 39
            // row2
            Prop{Textures.TreeGreen, Vector2{0.f,167.f}, PropType::TREE, Textures},        // 1
            Prop{Textures.TreeGreen, Vector2{200.f,167.f}, PropType::TREE, Textures},      // 3
            Prop{Textures.TreeGreen, Vector2{300.f,167.f}, PropType::TREE, Textures},      // 4
            Prop{Textures.TreeGreen, Vector2{400.f,167.f}, PropType::TREE, Textures},      // 5
            Prop{Textures.TreeGreen, Vector2{500.f,167.f}, PropType::TREE, Textures},      // 6
            Prop{Textures.TreeGreen, Vector2{700.f,167.f}, PropType::TREE, Textures},      // 8
            Prop{Textures.TreeGreen, Vector2{800.f,167.f}, PropType::TREE, Textures},      // 9
            Prop{Textures.TreeGreen, Vector2{900.f,167.f}, PropType::TREE, Textures},      // 10
            Prop{Textures.TreeGreen, Vector2{1000.f,167.f}, PropType::TREE, Textures},     // 11
            Prop{Textures.TreeGreen, Vector2{1400.f,167.f}, PropType::TREE, Textures},     // 15
            Prop{Textures.TreeGreen, Vector2{1500.f,167.f}, PropType::TREE, Textures},     // 16
            Prop{Textures.TreeGreen, Vector2{1600.f,167.f}, PropType::TREE, Textures},     // 17
            Prop{Textures.TreeGreen, Vector2{1900.f,167.f}, PropType::TREE, Textures},     // 20
            Prop{Textures.TreeGreen, Vector2{2000.f,167.f}, PropType::TREE, Textures},     // 21
            Prop{Textures.TreeGreen, Vector2{2100.f,167.f}, PropType::TREE, Textures},     // 22
            Prop{Textures.TreeGreen, Vector2{2200.f,167.f}, PropType::TREE, Textures},     // 23
            Prop{Textures.TreeGreen, Vector2{2500.f,167.f}, PropType::TREE, Textures},     // 26
            Prop{Textures.TreeGreen, Vector2{2600.f,167.f}, PropType::TREE, Textures},     // 27
            Prop{Textures.TreeGreen, Vector2{2700.f,167.f}, PropType::TREE, Textures},     // 28
            Prop{Textures.TreeGreen, Vector2{2800.f,167.f}, PropType::TREE, Textures},     // 29
            Prop{Textures.TreeGreen, Vector2{3000.f,167.f}, PropType::TREE, Textures},     // 31
            Prop{Textures.TreeGreen, Vector2{3100.f,167.f}, PropType::TREE, Textures},     // 32
            Prop{Textures.TreeGreen, Vector2{3200.f,167.f}, PropType::TREE, Textures},     // 33
            Prop{Textures.TreeGreen, Vector2{3300.f,167.f}, PropType::TREE, Textures},     // 34
            Prop{Textures.TreeGreen, Vector2{3400.f,167.f}, PropType::TREE, Textures},     // 35
            Prop{Textures.TreeGreen, Vector2{3720.f,167.f}, PropType::TREE, Textures},     // 36
            Prop{Textures.TreeGreen, Vector2{3920.f,167.f}, PropType::TREE, Textures},     // 38
            // row3
            Prop{Textures.TreeGreen, Vector2{20.f,267.f}, PropType::TREE, Textures},       // 1 
            Prop{Textures.TreeGreen, Vector2{120.f,267.f}, PropType::TREE, Textures},      // 2 
            Prop{Textures.TreeGreen, Vector2{220.f,267.f}, PropType::TREE, Textures},      // 3 
            Prop{Textures.TreeGreen, Vector2{320.f,267.f}, PropType::TREE, Textures},      // 4 
            Prop{Textures.TreeGreen, Vector2{420.f,267.f}, PropType::TREE, Textures},      // 5 
            Prop{Textures.TreeGreen, Vector2{620.f,267.f}, PropType::TREE, Textures},      // 7 
            Prop{Textures.TreeGreen, Vector2{720.f,267.f}, PropType::TREE, Textures},      // 8 
            Prop{Textures.TreeGreen, Vector2{820.f,267.f}, PropType::TREE, Textures},      // 9 
            Prop{Textures.TreeGreen, Vector2{920.f,267.f}, PropType::TREE, Textures},      // 10
            Prop{Textures.TreeGreen, Vector2{1520.f,267.f}, PropType::TREE, Textures},     // 16
            Prop{Textures.TreeGreen, Vector2{1620.f,267.f}, PropType::TREE, Textures},     // 17
            Prop{Textures.TreeGreen, Vector2{1720.f,267.f}, PropType::TREE, Textures},     // 18
            Prop{Textures.TreeGreen, Vector2{1820.f,267.f}, PropType::TREE, Textures},     // 19
            Prop{Textures.TreeGreen, Vector2{1920.f,267.f}, PropType::TREE, Textures},     // 20
            Prop{Textures.TreeGreen, Vector2{2020.f,267.f}, PropType::TREE, Textures},     // 21
            Prop{Textures.TreeGreen, Vector2{2220.f,267.f}, PropType::TREE, Textures},     // 23
            Prop{Textures.TreeGreen, Vector2{2320.f,267.f}, PropType::TREE, Textures},     // 24
            Prop{Textures.TreeGreen, Vector2{2420.f,267.f}, PropType::TREE, Textures},     // 25
            Prop{Textures.TreeGreen, Vector2{2520.f,267.f}, PropType::TREE, Textures},     // 26
            Prop{Textures.TreeGreen, Vector2{2620.f,267.f}, PropType::TREE, Textures},     // 27
            Prop{Textures.TreeGreen, Vector2{2820.f,267.f}, PropType::TREE, Textures},     // 29
            Prop{Textures.TreeGreen, Vector2{2920.f,267.f}, PropType::TREE, Textures},     // 30
            Prop{Textures.TreeGreen, Vector2{3020.f,267.f}, PropType::TREE, Textures},     // 31
            Prop{Textures.TreeGreen, Vector2{3120.f,267.f}, PropType::TREE, Textures},     // 32
            Prop{Textures.TreeGreen, Vector2{3320.f,267.f}, PropType::TREE, Textures},     // 34
            Prop{Textures.TreeGreen, Vector2{3700.f,267.f}, PropType::TREE, Textures},     // 36
            Prop{Textures.TreeGreen, Vector2{3800.f,267.f}, PropType::TREE, Textures},     // 37
            Prop{Textures.TreeGreen, Vector2{3900.f,267.f}, PropType::TREE, Textures},     // 38
            Prop{Textures.TreeGreen, Vector2{4000.f,267.f}, PropType::TREE, Textures},     // 39
            // row4
            Prop{Textures.TreeGreen, Vector2{0.f,367.f}, PropType::TREE, Textures},        // 1 
            Prop{Textures.TreeGreen, Vector2{100.f,367.f}, PropType::TREE, Textures},      // 2 
            Prop{Textures.TreeGreen, Vector2{200.f,367.f}, PropType::TREE, Textures},      // 3 
            Prop{Textures.TreeGreen, Vector2{400.f,367.f}, PropType::TREE, Textures},      // 5 
            Prop{Textures.TreeGreen, Vector2{500.f,367.f}, PropType::TREE, Textures},      // 6 
            Prop{Textures.TreeGreen, Vector2{700.f,367.f}, PropType::TREE, Textures},      // 8 
            Prop{Textures.TreeGreen, Vector2{800.f,367.f}, PropType::TREE, Textures},      // 9 
            Prop{Textures.TreeGreen, Vector2{900.f,367.f}, PropType::TREE, Textures},      // 10
            Prop{Textures.TreeGreen, Vector2{1500.f,367.f}, PropType::TREE, Textures},     // 16
            Prop{Textures.TreeGreen, Vector2{1600.f,367.f}, PropType::TREE, Textures},     // 17
            Prop{Textures.TreeGreen, Vector2{1800.f,367.f}, PropType::TREE, Textures},     // 19
            Prop{Textures.TreeGreen, Vector2{1900.f,367.f}, PropType::TREE, Textures},     // 20
            Prop{Textures.TreeGreen, Vector2{2400.f,367.f}, PropType::TREE, Textures},     // 25
            Prop{Textures.TreeGreen, Vector2{2500.f,367.f}, PropType::TREE, Textures},     // 26
            Prop{Textures.TreeGreen, Vector2{2600.f,367.f}, PropType::TREE, Textures},     // 27
            Prop{Textures.TreeGreen, Vector2{2700.f,367.f}, PropType::TREE, Textures},     // 28
            Prop{Textures.TreeGreen, Vector2{2800.f,367.f}, PropType::TREE, Textures},     // 29
            Prop{Textures.TreeGreen, Vector2{2900.f,367.f}, PropType::TREE, Textures},     // 30
            Prop{Textures.TreeGreen, Vector2{3000.f,367.f}, PropType::TREE, Textures},     // 31
            Prop{Textures.TreeGreen, Vector2{3100.f,367.f}, PropType::TREE, Textures},     // 32
            Prop{Textures.TreeGreen, Vector2{3200.f,367.f}, PropType::TREE, Textures},     // 33
            Prop{Textures.TreeGreen, Vector2{3300.f,367.f}, PropType::TREE, Textures},     // 34
            Prop{Textures.TreeGreen, Vector2{3400.f,367.f}, PropType::TREE, Textures},     // 35
            Prop{Textures.TreeGreen, Vector2{3720.f,367.f}, PropType::TREE, Textures},     // 36
            Prop{Textures.TreeGreen, Vector2{3820.f,367.f}, PropType::TREE, Textures},     // 37
            Prop{Textures.TreeGreen, Vector2{3920.f,367.f}, PropType::TREE, Textures},     // 38
            //row5
            Prop{Textures.TreeGreen, Vector2{20.f,467.f}, PropType::TREE, Textures},       // 1 
            Prop{Textures.TreeGreen, Vector2{120.f,467.f}, PropType::TREE, Textures},      // 2 
            Prop{Textures.TreeGreen, Vector2{320.f,467.f}, PropType::TREE, Textures},      // 4 
            Prop{Textures.TreeGreen, Vector2{520.f,467.f}, PropType::TREE, Textures},      // 6 
            Prop{Textures.TreeGreen, Vector2{620.f,467.f}, PropType::TREE, Textures},      // 7 
            Prop{Textures.TreeGreen, Vector2{720.f,467.f}, PropType::TREE, Textures},      // 8 
            Prop{Textures.TreeGreen, Vector2{920.f,467.f}, PropType::TREE, Textures},      // 10
            Prop{Textures.TreeGreen, Vector2{1020.f,467.f}, PropType::TREE, Textures},     // 11
            Prop{Textures.TreeGreen, Vector2{1420.f,467.f}, PropType::TREE, Textures},     // 15
            Prop{Textures.TreeGreen, Vector2{1520.f,467.f}, PropType::TREE, Textures},     // 16
            Prop{Textures.TreeGreen, Vector2{1620.f,467.f}, PropType::TREE, Textures},     // 17
            Prop{Textures.TreeGreen, Vector2{1820.f,467.f}, PropType::TREE, Textures},     // 19
            Prop{Textures.TreeGreen, Vector2{2520.f,467.f}, PropType::TREE, Textures},     // 26
            Prop{Textures.TreeGreen, Vector2{2620.f,467.f}, PropType::TREE, Textures},     // 27
            Prop{Textures.TreeGreen, Vector2{2720.f,467.f}, PropType::TREE, Textures},     // 28
            Prop{Textures.TreeGreen, Vector2{2820.f,467.f}, PropType::TREE, Textures},     // 29
            Prop{Textures.TreeGreen, Vector2{2920.f,467.f}, PropType::TREE, Textures},     // 30
            Prop{Textures.TreeGreen, Vector2{3120.f,467.f}, PropType::TREE, Textures},     // 32
            Prop{Textures.TreeGreen, Vector2{3220.f,467.f}, PropType::TREE, Textures},     // 33
            Prop{Textures.TreeGreen, Vector2{3320.f,467.f}, PropType::TREE, Textures},     // 34
            Prop{Textures.TreeGreen, Vector2{3420.f,467.f}, PropType::TREE, Textures},     // 35
            Prop{Textures.TreeGreen, Vector2{3700.f,467.f}, PropType::TREE, Textures},     // 36
            Prop{Textures.TreeGreen, Vector2{3800.f,467.f}, PropType::TREE, Textures},     // 37
            Prop{Textures.TreeGreen, Vector2{3900.f,467.f}, PropType::TREE, Textures},     // 38
            Prop{Textures.TreeGreen, Vector2{4000.f,467.f}, PropType::TREE, Textures},     // 39
            // row6
            Prop{Textures.TreeGreen, Vector2{0.f,567.f}, PropType::TREE, Textures},        // 1 
            Prop{Textures.TreeGreen, Vector2{100.f,567.f}, PropType::TREE, Textures},      // 2 
            Prop{Textures.TreeGreen, Vector2{200.f,567.f}, PropType::TREE, Textures},      // 3 
            Prop{Textures.TreeGreen, Vector2{300.f,567.f}, PropType::TREE, Textures},      // 4 
            Prop{Textures.TreeGreen, Vector2{400.f,567.f}, PropType::TREE, Textures},      // 5 
            Prop{Textures.TreeGreen, Vector2{500.f,567.f}, PropType::TREE, Textures},      // 6 
            Prop{Textures.TreeGreen, Vector2{600.f,567.f}, PropType::TREE, Textures},      // 7 
            Prop{Textures.TreeGreen, Vector2{800.f,567.f}, PropType::TREE, Textures},      // 9 
            Prop{Textures.TreeGreen, Vector2{900.f,567.f}, PropType::TREE, Textures},      // 10
            Prop{Textures.TreeGreen, Vector2{1000.f,567.f}, PropType::TREE, Textures},     // 11
            Prop{Textures.TreeGreen, Vector2{1100.f,567.f}, PropType::TREE, Textures},     // 12
            Prop{Textures.TreeGreen, Vector2{1200.f,567.f}, PropType::TREE, Textures},     // 13
            Prop{Textures.TreeGreen, Vector2{1400.f,567.f}, PropType::TREE, Textures},     // 15
            Prop{Textures.TreeGreen, Vector2{1500.f,567.f}, PropType::TREE, Textures},     // 16
            Prop{Textures.TreeGreen, Vector2{1600.f,567.f}, PropType::TREE, Textures},     // 17
            Prop{Textures.TreeGreen, Vector2{1700.f,567.f}, PropType::TREE, Textures},     // 18
            Prop{Textures.TreeGreen, Vector2{2500.f,567.f}, PropType::TREE, Textures},     // 26
            Prop{Textures.TreeGreen, Vector2{2700.f,567.f}, PropType::TREE, Textures},     // 28
            Prop{Textures.TreeGreen, Vector2{2800.f,567.f}, PropType::TREE, Textures},     // 29
            Prop{Textures.TreeGreen, Vector2{2900.f,567.f}, PropType::TREE, Textures},     // 30
            Prop{Textures.TreeGreen, Vector2{3000.f,567.f}, PropType::TREE, Textures},     // 31
            Prop{Textures.TreeGreen, Vector2{3100.f,567.f}, PropType::TREE, Textures},     // 32
            Prop{Textures.TreeGreen, Vector2{3200.f,567.f}, PropType::TREE, Textures},     // 33
            Prop{Textures.TreeGreen, Vector2{3300.f,567.f}, PropType::TREE, Textures},     // 34
            Prop{Textures.TreeGreen, Vector2{3400.f,567.f}, PropType::TREE, Textures},     // 35
            Prop{Textures.TreeGreen, Vector2{3720.f,567.f}, PropType::TREE, Textures},     // 36
            Prop{Textures.TreeGreen, Vector2{3820.f,567.f}, PropType::TREE, Textures},     // 37
            Prop{Textures.TreeGreen, Vector2{3920.f,567.f}, PropType::TREE, Textures},     // 38
            // row7
            Prop{Textures.TreeGreen, Vector2{20.f,667.f}, PropType::TREE, Textures},       // 1 
            Prop{Textures.TreeGreen, Vector2{120.f,667.f}, PropType::TREE, Textures},      // 2 
            Prop{Textures.TreeGreen, Vector2{220.f,667.f}, PropType::TREE, Textures},      // 3 
            Prop{Textures.TreeGreen, Vector2{320.f,667.f}, PropType::TREE, Textures},      // 4 
            Prop{Textures.TreeGreen, Vector2{420.f,667.f}, PropType::TREE, Textures},      // 5 
            Prop{Textures.TreeGreen, Vector2{520.f,667.f}, PropType::TREE, Textures},      // 6 
            Prop{Textures.TreeGreen, Vector2{720.f,667.f}, PropType::TREE, Textures},      // 8 
            Prop{Textures.TreeGreen, Vector2{820.f,667.f}, PropType::TREE, Textures},      // 9 
            Prop{Textures.TreeGreen, Vector2{920.f,667.f}, PropType::TREE, Textures},      // 10
            Prop{Textures.TreeGreen, Vector2{1020.f,667.f}, PropType::TREE, Textures},     // 11
            Prop{Textures.TreeGreen, Vector2{1220.f,667.f}, PropType::TREE, Textures},     // 13
            Prop{Textures.TreeGreen, Vector2{1320.f,667.f}, PropType::TREE, Textures},     // 14
            Prop{Textures.TreeGreen, Vector2{1420.f,667.f}, PropType::TREE, Textures},     // 15
            Prop{Textures.TreeGreen, Vector2{1520.f,667.f}, PropType::TREE, Textures},     // 16
            Prop{Textures.TreeGreen, Vector2{1720.f,667.f}, PropType::TREE, Textures},     // 18
            Prop{Textures.TreeGreen, Vector2{1820.f,667.f}, PropType::TREE, Textures},     // 19
            Prop{Textures.TreeGreen, Vector2{2420.f,667.f}, PropType::TREE, Textures},     // 25
            Prop{Textures.TreeGreen, Vector2{2520.f,667.f}, PropType::TREE, Textures},     // 26
            Prop{Textures.TreeGreen, Vector2{2620.f,667.f}, PropType::TREE, Textures},     // 27
            Prop{Textures.TreeGreen, Vector2{2720.f,667.f}, PropType::TREE, Textures},     // 28    
            Prop{Textures.TreeGreen, Vector2{3120.f,667.f}, PropType::TREE, Textures},     // 32    
            Prop{Textures.TreeGreen, Vector2{3220.f,667.f}, PropType::TREE, Textures},     // 33
            Prop{Textures.TreeGreen, Vector2{3320.f,667.f}, PropType::TREE, Textures},     // 34
            Prop{Textures.TreeGreen, Vector2{3420.f,667.f}, PropType::TREE, Textures},     // 35
            Prop{Textures.TreeGreen, Vector2{3700.f,667.f}, PropType::TREE, Textures},     // 36    
            Prop{Textures.TreeGreen, Vector2{3800.f,667.f}, PropType::TREE, Textures},     // 37
            Prop{Textures.TreeGreen, Vector2{3900.f,667.f}, PropType::TREE, Textures},     // 38
            Prop{Textures.TreeGreen, Vector2{4000.f,667.f}, PropType::TREE, Textures},     // 39
            // row8
            Prop{Textures.TreeGreen, Vector2{0.f,767.f}, PropType::TREE, Textures},        // 1 
            Prop{Textures.TreeGreen, Vector2{100.f,767.f}, PropType::TREE, Textures},      // 2 
            Prop{Textures.TreeGreen, Vector2{200.f,767.f}, PropType::TREE, Textures},      // 3 
            Prop{Textures.TreeGreen, Vector2{300.f,767.f}, PropType::TREE, Textures},      // 4 
            Prop{Textures.TreeGreen, Vector2{400.f,767.f}, PropType::TREE, Textures},      // 5 
            Prop{Textures.TreeGreen, Vector2{500.f,767.f}, PropType::TREE, Textures},      // 6 
            Prop{Textures.TreeGreen, Vector2{600.f,767.f}, PropType::TREE, Textures},      // 7 
            Prop{Textures.TreeGreen, Vector2{800.f,767.f}, PropType::TREE, Textures},      // 9 
            Prop{Textures.TreeGreen, Vector2{900.f,767.f}, PropType::TREE, Textures},      // 10
            Prop{Textures.TreeGreen, Vector2{1000.f,767.f}, PropType::TREE, Textures},     // 11
            Prop{Textures.TreeGreen, Vector2{1200.f,767.f}, PropType::TREE, Textures},     // 13
            Prop{Textures.TreeGreen, Vector2{1300.f,767.f}, PropType::TREE, Textures},     // 14
            Prop{Textures.TreeGreen, Vector2{1400.f,767.f}, PropType::TREE, Textures},     // 15
            Prop{Textures.TreeGreen, Vector2{1500.f,767.f}, PropType::TREE, Textures},     // 16
            Prop{Textures.TreeGreen, Vector2{1600.f,767.f}, PropType::TREE, Textures},     // 17
            Prop{Textures.TreeGreen, Vector2{1700.f,767.f}, PropType::TREE, Textures},     // 18
            Prop{Textures.TreeGreen, Vector2{1900.f,767.f}, PropType::TREE, Textures},     // 20
            Prop{Textures.TreeGreen, Vector2{2400.f,767.f}, PropType::TREE, Textures},     // 25
            Prop{Textures.TreeGreen, Vector2{2500.f,767.f}, PropType::TREE, Textures},     // 26
            Prop{Textures.TreeGreen, Vector2{2600.f,767.f}, PropType::TREE, Textures},     // 27
            Prop{Textures.TreeGreen, Vector2{3200.f,767.f}, PropType::TREE, Textures},     // 33
            Prop{Textures.TreeGreen, Vector2{3300.f,767.f}, PropType::TREE, Textures},     // 34    
            Prop{Textures.TreeGreen, Vector2{3400.f,767.f}, PropType::TREE, Textures},     // 35
            Prop{Textures.TreeGreen, Vector2{3720.f,767.f}, PropType::TREE, Textures},     // 36    
            Prop{Textures.TreeGreen, Vector2{3820.f,767.f}, PropType::TREE, Textures},     // 37    
            Prop{Textures.TreeGreen, Vector2{3920.f,767.f}, PropType::TREE, Textures},     // 38
            // row9
            Prop{Textures.TreeGreen, Vector2{20.f,867.f}, PropType::TREE, Textures},       // 1 
            Prop{Textures.TreeGreen, Vector2{120.f,867.f}, PropType::TREE, Textures},      // 2 
            Prop{Textures.TreeGreen, Vector2{220.f,867.f}, PropType::TREE, Textures},      // 3 
            Prop{Textures.TreeGreen, Vector2{320.f,867.f}, PropType::TREE, Textures},      // 4 
            Prop{Textures.TreeGreen, Vector2{420.f,867.f}, PropType::TREE, Textures},      // 5 
            Prop{Textures.TreeGreen, Vector2{520.f,867.f}, PropType::TREE, Textures},      // 6 
            Prop{Textures.TreeGreen, Vector2{1020.f,867.f}, PropType::TREE, Textures},     // 11
            Prop{Textures.TreeGreen, Vector2{1120.f,867.f}, PropType::TREE, Textures},     // 12
            Prop{Textures.TreeGreen, Vector2{1220.f,867.f}, PropType::TREE, Textures},     // 13
            Prop{Textures.TreeGreen, Vector2{1320.f,867.f}, PropType::TREE, Textures},     // 14
            Prop{Textures.TreeGreen, Vector2{1420.f,867.f}, PropType::TREE, Textures},     // 15
            Prop{Textures.TreeGreen, Vector2{1520.f,867.f}, PropType::TREE, Textures},     // 16
            Prop{Textures.TreeGreen, Vector2{1620.f,867.f}, PropType::TREE, Textures},     // 17
            Prop{Textures.TreeGreen, Vector2{1720.f,867.f}, PropType::TREE, Textures},     // 18
            Prop{Textures.TreeGreen, Vector2{1820.f,867.f}, PropType::TREE, Textures},     // 19
            Prop{Textures.TreeGreen, Vector2{1920.f,867.f}, PropType::TREE, Textures},     // 20
            Prop{Textures.TreeGreen, Vector2{2020.f,867.f}, PropType::TREE, Textures},     // 21
            Prop{Textures.TreeGreen, Vector2{2120.f,867.f}, PropType::TREE, Textures},     // 22
            Prop{Textures.TreeGreen, Vector2{2220.f,867.f}, PropType::TREE, Textures},     // 23
            Prop{Textures.TreeGreen, Vector2{2320.f,867.f}, PropType::TREE, Textures},     // 24
            Prop{Textures.TreeGreen, Vector2{2420.f,867.f}, PropType::TREE, Textures},     // 25
            Prop{Textures.TreeGreen, Vector2{2520.f,867.f}, PropType::TREE, Textures},     // 26
            Prop{Textures.TreeGreen, Vector2{2620.f,867.f}, PropType::TREE, Textures},     // 27
            Prop{Textures.TreeGreen, Vector2{3220.f,867.f}, PropType::TREE, Textures},     // 33
            Prop{Textures.TreeGreen, Vector2{3320.f,867.f}, PropType::TREE, Textures},     // 34
            Prop{Textures.TreeGreen, Vector2{3420.f,867.f}, PropType::TREE, Textures},     // 35
            Prop{Textures.TreeGreen, Vector2{3700.f,867.f}, PropType::TREE, Textures},     // 36
            Prop{Textures.TreeGreen, Vector2{3800.f,867.f}, PropType::TREE, Textures},     // 37
            Prop{Textures.TreeGreen, Vector2{3900.f,867.f}, PropType::TREE, Textures},     // 38
            Prop{Textures.TreeGreen, Vector2{4000.f,867.f}, PropType::TREE, Textures},     // 39
            // row10
            Prop{Textures.TreeGreen, Vector2{0.f,967.f}, PropType::TREE, Textures},        // 1 
            Prop{Textures.TreeGreen, Vector2{100.f,967.f}, PropType::TREE, Textures},      // 2 
            Prop{Textures.TreeGreen, Vector2{200.f,967.f}, PropType::TREE, Textures},      // 3 
            Prop{Textures.TreeGreen, Vector2{300.f,967.f}, PropType::TREE, Textures},      // 4 
            Prop{Textures.TreeGreen, Vector2{400.f,967.f}, PropType::TREE, Textures},      // 5 
            Prop{Textures.TreeGreen, Vector2{1100.f,967.f}, PropType::TREE, Textures},     // 12
            Prop{Textures.TreeGreen, Vector2{1200.f,967.f}, PropType::TREE, Textures},     // 13
            Prop{Textures.TreeGreen, Vector2{1300.f,967.f}, PropType::TREE, Textures},     // 14
            Prop{Textures.TreeGreen, Vector2{1400.f,967.f}, PropType::TREE, Textures},     // 15
            Prop{Textures.TreeGreen, Vector2{1500.f,967.f}, PropType::TREE, Textures},     // 16
            Prop{Textures.TreeGreen, Vector2{1600.f,967.f}, PropType::TREE, Textures},     // 17
            Prop{Textures.TreeGreen, Vector2{1700.f,967.f}, PropType::TREE, Textures},     // 18
            Prop{Textures.TreeGreen, Vector2{1800.f,967.f}, PropType::TREE, Textures},     // 19
            Prop{Textures.TreeGreen, Vector2{1900.f,967.f}, PropType::TREE, Textures},     // 20
            Prop{Textures.TreeGreen, Vector2{2000.f,967.f}, PropType::TREE, Textures},     // 21
            Prop{Textures.TreeGreen, Vector2{2100.f,967.f}, PropType::TREE, Textures},     // 22
            Prop{Textures.TreeGreen, Vector2{2200.f,967.f}, PropType::TREE, Textures},     // 23
            Prop{Textures.TreeGreen, Vector2{2300.f,967.f}, PropType::TREE, Textures},     // 24
            Prop{Textures.TreeGreen, Vector2{2400.f,967.f}, PropType::TREE, Textures},     // 25
            Prop{Textures.TreeGreen, Vector2{2500.f,967.f}, PropType::TREE, Textures},     // 26
            Prop{Textures.TreeGreen, Vector2{2600.f,967.f}, PropType::TREE, Textures},     // 27
            Prop{Textures.TreeGreen, Vector2{3200.f,967.f}, PropType::TREE, Textures},     // 33
            Prop{Textures.TreeGreen, Vector2{3300.f,967.f}, PropType::TREE, Textures},     // 34
            Prop{Textures.TreeGreen, Vector2{3400.f,967.f}, PropType::TREE, Textures},     // 35
            Prop{Textures.TreeGreen, Vector2{3720.f,967.f}, PropType::TREE, Textures},     // 36
            Prop{Textures.TreeGreen, Vector2{3820.f,967.f}, PropType::TREE, Textures},     // 37
            Prop{Textures.TreeGreen, Vector2{3920.f,967.f}, PropType::TREE, Textures},     // 38
            // row11
            Prop{Textures.TreeGreen, Vector2{20.f,1067.f}, PropType::TREE, Textures},      // 1 
            Prop{Textures.TreeGreen, Vector2{120.f,1067.f}, PropType::TREE, Textures},     // 2     
            Prop{Textures.TreeGreen, Vector2{220.f,1067.f}, PropType::TREE, Textures},     // 3     
            Prop{Textures.TreeGreen, Vector2{320.f,1067.f}, PropType::TREE, Textures},     // 4 
            Prop{Textures.TreeGreen, Vector2{420.f,1067.f}, PropType::TREE, Textures},     // 5 
            Prop{Textures.TreeGreen, Vector2{1420.f,1067.f}, PropType::TREE, Textures},    // 15    
            Prop{Textures.TreeGreen, Vector2{1520.f,1067.f}, PropType::TREE, Textures},    // 16    
            Prop{Textures.TreeGreen, Vector2{1620.f,1067.f}, PropType::TREE, Textures},    // 17    
            Prop{Textures.TreeGreen, Vector2{1720.f,1067.f}, PropType::TREE, Textures},    // 18    
            Prop{Textures.TreeGreen, Vector2{1820.f,1067.f}, PropType::TREE, Textures},    // 19    
            Prop{Textures.TreeGreen, Vector2{1920.f,1067.f}, PropType::TREE, Textures},    // 20    
            Prop{Textures.TreeGreen, Vector2{2020.f,1067.f}, PropType::TREE, Textures},    // 21    
            Prop{Textures.TreeGreen, Vector2{2120.f,1067.f}, PropType::TREE, Textures},    // 22    
            Prop{Textures.TreeGreen, Vector2{2220.f,1067.f}, PropType::TREE, Textures},    // 23    
            Prop{Textures.TreeGreen, Vector2{2320.f,1067.f}, PropType::TREE, Textures},    // 24    
            Prop{Textures.TreeGreen, Vector2{2420.f,1067.f}, PropType::TREE, Textures},    // 25    
            Prop{Textures.TreeGreen, Vector2{2520.f,1067.f}, PropType::TREE, Textures},    // 26    
            Prop{Textures.TreeGreen, Vector2{2620.f,1067.f}, PropType::TREE, Textures},    // 27    
            Prop{Textures.TreeGreen, Vector2{2720.f,1067.f}, PropType::TREE, Textures},    // 28    
            Prop{Textures.TreeGreen, Vector2{3120.f,1067.f}, PropType::TREE, Textures},    // 32    
            Prop{Textures.TreeGreen, Vector2{3220.f,1067.f}, PropType::TREE, Textures},    // 33    
            Prop{Textures.TreeGreen, Vector2{3320.f,1067.f}, PropType::TREE, Textures},    // 34    
            Prop{Textures.TreeGreen, Vector2{3420.f,1067.f}, PropType::TREE, Textures},    // 35    
            Prop{Textures.TreeGreen, Vector2{3700.f,1067.f}, PropType::TREE, Textures},    // 36    
            Prop{Textures.TreeGreen, Vector2{3800.f,1067.f}, PropType::TREE, Textures},    // 37    
            Prop{Textures.TreeGreen, Vector2{3900.f,1067.f}, PropType::TREE, Textures},    // 38    
            Prop{Textures.TreeGreen, Vector2{4000.f,1067.f}, PropType::TREE, Textures},    // 39
            // row12
            Prop{Textures.TreeGreen, Vector2{0.f,1167.f}, PropType::TREE, Textures},       // 1 
            Prop{Textures.TreeGreen, Vector2{100.f,1167.f}, PropType::TREE, Textures},     // 2 
            Prop{Textures.TreeGreen, Vector2{200.f,1167.f}, PropType::TREE, Textures},     // 3 
            Prop{Textures.TreeGreen, Vector2{300.f,1167.f}, PropType::TREE, Textures},     // 4 
            Prop{Textures.TreeGreen, Vector2{400.f,1167.f}, PropType::TREE, Textures},     // 5 
            Prop{Textures.TreeGreen, Vector2{1100.f,1167.f}, PropType::TREE, Textures},    // 12    
            Prop{Textures.TreeGreen, Vector2{1200.f,1167.f}, PropType::TREE, Textures},    // 13    
            Prop{Textures.TreeGreen, Vector2{1300.f,1167.f}, PropType::TREE, Textures},    // 14    
            Prop{Textures.TreeGreen, Vector2{1400.f,1167.f}, PropType::TREE, Textures},    // 15    
            Prop{Textures.TreeGreen, Vector2{1500.f,1167.f}, PropType::TREE, Textures},    // 16    
            Prop{Textures.TreeGreen, Vector2{1600.f,1167.f}, PropType::TREE, Textures},    // 17    
            Prop{Textures.TreeGreen, Vector2{1700.f,1167.f}, PropType::TREE, Textures},    // 18    
            Prop{Textures.TreeGreen, Vector2{1800.f,1167.f}, PropType::TREE, Textures},    // 19    
            Prop{Textures.TreeGreen, Vector2{1900.f,1167.f}, PropType::TREE, Textures},    // 20    
            Prop{Textures.TreeGreen, Vector2{2000.f,1167.f}, PropType::TREE, Textures},    // 21    
            Prop{Textures.TreeGreen, Vector2{2100.f,1167.f}, PropType::TREE, Textures},    // 22    
            Prop{Textures.TreeGreen, Vector2{2200.f,1167.f}, PropType::TREE, Textures},    // 23    
            Prop{Textures.TreeGreen, Vector2{2300.f,1167.f}, PropType::TREE, Textures},    // 24    
            Prop{Textures.TreeGreen, Vector2{2400.f,1167.f}, PropType::TREE, Textures},    // 25    
            Prop{Textures.TreeGreen, Vector2{2500.f,1167.f}, PropType::TREE, Textures},    // 26    
            Prop{Textures.TreeGreen, Vector2{2800.f,1167.f}, PropType::TREE, Textures},    // 29    
            Prop{Textures.TreeGreen, Vector2{2900.f,1167.f}, PropType::TREE, Textures},    // 30    
            Prop{Textures.TreeGreen, Vector2{3000.f,1167.f}, PropType::TREE, Textures},    // 31    
            Prop{Textures.TreeGreen, Vector2{3100.f,1167.f}, PropType::TREE, Textures},    // 32    
            Prop{Textures.TreeGreen, Vector2{3300.f,1167.f}, PropType::TREE, Textures},    // 34    
            Prop{Textures.TreeGreen, Vector2{3400.f,1167.f}, PropType::TREE, Textures},    // 35    
            Prop{Textures.TreeGreen, Vector2{3720.f,1167.f}, PropType::TREE, Textures},    // 36    
            Prop{Textures.TreeGreen, Vector2{3820.f,1167.f}, PropType::TREE, Textures},    // 37    
            Prop{Textures.TreeGreen, Vector2{3920.f,1167.f}, PropType::TREE, Textures},    // 38    
            // row13
            Prop{Textures.TreeGreen, Vector2{120.f,1267.f}, PropType::TREE, Textures},     // 2 
            Prop{Textures.TreeGreen, Vector2{220.f,1267.f}, PropType::TREE, Textures},     // 3 
            Prop{Textures.TreeGreen, Vector2{320.f,1267.f}, PropType::TREE, Textures},     // 4 
            Prop{Textures.TreeGreen, Vector2{420.f,1267.f}, PropType::TREE, Textures},     // 5 
            Prop{Textures.TreeGreen, Vector2{520.f,1267.f}, PropType::TREE, Textures},     // 6 
            Prop{Textures.TreeGreen, Vector2{1020.f,1267.f}, PropType::TREE, Textures},    // 11
            Prop{Textures.TreeGreen, Vector2{1120.f,1267.f}, PropType::TREE, Textures},    // 12
            Prop{Textures.TreeGreen, Vector2{1220.f,1267.f}, PropType::TREE, Textures},    // 13
            Prop{Textures.TreeGreen, Vector2{1320.f,1267.f}, PropType::TREE, Textures},    // 14
            Prop{Textures.TreeGreen, Vector2{1420.f,1267.f}, PropType::TREE, Textures},    // 15
            Prop{Textures.TreeGreen, Vector2{1520.f,1267.f}, PropType::TREE, Textures},    // 16
            Prop{Textures.TreeGreen, Vector2{1620.f,1267.f}, PropType::TREE, Textures},    // 17
            Prop{Textures.TreeGreen, Vector2{1720.f,1267.f}, PropType::TREE, Textures},    // 18
            Prop{Textures.TreeGreen, Vector2{1820.f,1267.f}, PropType::TREE, Textures},    // 19
            Prop{Textures.TreeGreen, Vector2{1920.f,1267.f}, PropType::TREE, Textures},    // 20
            Prop{Textures.TreeGreen, Vector2{2420.f,1267.f}, PropType::TREE, Textures},    // 25
            Prop{Textures.TreeGreen, Vector2{2520.f,1267.f}, PropType::TREE, Textures},    // 26
            Prop{Textures.TreeGreen, Vector2{2620.f,1267.f}, PropType::TREE, Textures},    // 27
            Prop{Textures.TreeGreen, Vector2{2720.f,1267.f}, PropType::TREE, Textures},    // 28
            Prop{Textures.TreeGreen, Vector2{2820.f,1267.f}, PropType::TREE, Textures},    // 29
            Prop{Textures.TreeGreen, Vector2{2920.f,1267.f}, PropType::TREE, Textures},    // 30
            Prop{Textures.TreeGreen, Vector2{3020.f,1267.f}, PropType::TREE, Textures},    // 31
            Prop{Textures.TreeGreen, Vector2{3120.f,1267.f}, PropType::TREE, Textures},    // 32
            Prop{Textures.TreeGreen, Vector2{3220.f,1267.f}, PropType::TREE, Textures},    // 33
            Prop{Textures.TreeGreen, Vector2{3320.f,1267.f}, PropType::TREE, Textures},    // 34
            Prop{Textures.TreeGreen, Vector2{3420.f,1267.f}, PropType::TREE, Textures},    // 35
            Prop{Textures.TreeGreen, Vector2{3700.f,1267.f}, PropType::TREE, Textures},    // 36
            Prop{Textures.TreeGreen, Vector2{3800.f,1267.f}, PropType::TREE, Textures},    // 37
            Prop{Textures.TreeGreen, Vector2{3900.f,1267.f}, PropType::TREE, Textures},    // 38
            Prop{Textures.TreeGreen, Vector2{4000.f,1267.f}, PropType::TREE, Textures},    // 39
            // row14
            Prop{Textures.TreeGreen, Vector2{0.f,1367.f}, PropType::TREE, Textures},       // 1 
            Prop{Textures.TreeGreen, Vector2{100.f,1367.f}, PropType::TREE, Textures},     // 2     
            Prop{Textures.TreeGreen, Vector2{200.f,1367.f}, PropType::TREE, Textures},     // 3     
            Prop{Textures.TreeGreen, Vector2{300.f,1367.f}, PropType::TREE, Textures},     // 4     
            Prop{Textures.TreeGreen, Vector2{400.f,1367.f}, PropType::TREE, Textures},     // 5     
            Prop{Textures.TreeGreen, Vector2{500.f,1367.f}, PropType::TREE, Textures},     // 6     
            Prop{Textures.TreeGreen, Vector2{700.f,1367.f}, PropType::TREE, Textures},     // 8     
            Prop{Textures.TreeGreen, Vector2{800.f,1367.f}, PropType::TREE, Textures},     // 9     
            Prop{Textures.TreeGreen, Vector2{900.f,1367.f}, PropType::TREE, Textures},     // 10    
            Prop{Textures.TreeGreen, Vector2{1000.f,1367.f}, PropType::TREE, Textures},    // 11    
            Prop{Textures.TreeGreen, Vector2{1100.f,1367.f}, PropType::TREE, Textures},    // 12    
            Prop{Textures.TreeGreen, Vector2{1200.f,1367.f}, PropType::TREE, Textures},    // 13    
            Prop{Textures.TreeGreen, Vector2{1300.f,1367.f}, PropType::TREE, Textures},    // 14    
            Prop{Textures.TreeGreen, Vector2{1400.f,1367.f}, PropType::TREE, Textures},    // 15    
            Prop{Textures.TreeGreen, Vector2{1500.f,1367.f}, PropType::TREE, Textures},    // 16    
            Prop{Textures.TreeGreen, Vector2{1600.f,1367.f}, PropType::TREE, Textures},    // 17    
            Prop{Textures.TreeGreen, Vector2{1700.f,1367.f}, PropType::TREE, Textures},    // 18    
            Prop{Textures.TreeGreen, Vector2{1800.f,1367.f}, PropType::TREE, Textures},    // 19    
            Prop{Textures.TreeGreen, Vector2{2500.f,1367.f}, PropType::TREE, Textures},    // 26    
            Prop{Textures.TreeGreen, Vector2{2600.f,1367.f}, PropType::TREE, Textures},    // 27    
            Prop{Textures.TreeGreen, Vector2{2700.f,1367.f}, PropType::TREE, Textures},    // 28    
            Prop{Textures.TreeGreen, Vector2{2800.f,1367.f}, PropType::TREE, Textures},    // 29    
            Prop{Textures.TreeGreen, Vector2{2900.f,1367.f}, PropType::TREE, Textures},    // 30    
            Prop{Textures.TreeGreen, Vector2{3100.f,1367.f}, PropType::TREE, Textures},    // 32    
            Prop{Textures.TreeGreen, Vector2{3200.f,1367.f}, PropType::TREE, Textures},    // 33    
            Prop{Textures.TreeGreen, Vector2{3300.f,1367.f}, PropType::TREE, Textures},    // 34    
            Prop{Textures.TreeGreen, Vector2{3400.f,1367.f}, PropType::TREE, Textures},    // 35    
            Prop{Textures.TreeGreen, Vector2{3720.f,1367.f}, PropType::TREE, Textures},    // 36    
            Prop{Textures.TreeGreen, Vector2{3820.f,1367.f}, PropType::TREE, Textures},    // 37    
            Prop{Textures.TreeGreen, Vector2{3920.f,1367.f}, PropType::TREE, Textures},    // 38    
            // row15
            Prop{Textures.TreeGreen, Vector2{20.f,1467.f}, PropType::TREE, Textures},      // 1 
            Prop{Textures.TreeGreen, Vector2{120.f,1467.f}, PropType::TREE, Textures},     // 2 
            Prop{Textures.TreeGreen, Vector2{220.f,1467.f}, PropType::TREE, Textures},     // 3 
            Prop{Textures.TreeGreen, Vector2{320.f,1467.f}, PropType::TREE, Textures},     // 4 
            Prop{Textures.TreeGreen, Vector2{520.f,1467.f}, PropType::TREE, Textures},     // 6 
            Prop{Textures.TreeGreen, Vector2{720.f,1467.f}, PropType::TREE, Textures},     // 8 
            Prop{Textures.TreeGreen, Vector2{820.f,1467.f}, PropType::TREE, Textures},     // 9 
            Prop{Textures.TreeGreen, Vector2{920.f,1467.f}, PropType::TREE, Textures},     // 10
            Prop{Textures.TreeGreen, Vector2{1220.f,1467.f}, PropType::TREE, Textures},    // 13
            Prop{Textures.TreeGreen, Vector2{1320.f,1467.f}, PropType::TREE, Textures},    // 14
            Prop{Textures.TreeGreen, Vector2{1420.f,1467.f}, PropType::TREE, Textures},    // 15
            Prop{Textures.TreeGreen, Vector2{1520.f,1467.f}, PropType::TREE, Textures},    // 16
            Prop{Textures.TreeGreen, Vector2{1620.f,1467.f}, PropType::TREE, Textures},    // 17
            Prop{Textures.TreeGreen, Vector2{1720.f,1467.f}, PropType::TREE, Textures},    // 18
            Prop{Textures.TreeGreen, Vector2{1820.f,1467.f}, PropType::TREE, Textures},    // 19
            Prop{Textures.TreeGreen, Vector2{2420.f,1467.f}, PropType::TREE, Textures},    // 25
            Prop{Textures.TreeGreen, Vector2{2520.f,1467.f}, PropType::TREE, Textures},    // 26
            Prop{Textures.TreeGreen, Vector2{2620.f,1467.f}, PropType::TREE, Textures},    // 27
            Prop{Textures.TreeGreen, Vector2{2720.f,1467.f}, PropType::TREE, Textures},    // 28
            Prop{Textures.TreeGreen, Vector2{2820.f,1467.f}, PropType::TREE, Textures},    // 29
            Prop{Textures.TreeGreen, Vector2{2920.f,1467.f}, PropType::TREE, Textures},    // 30
            Prop{Textures.TreeGreen, Vector2{3020.f,1467.f}, PropType::TREE, Textures},    // 31
            Prop{Textures.TreeGreen, Vector2{3120.f,1467.f}, PropType::TREE, Textures},    // 32
            Prop{Textures.TreeGreen, Vector2{3220.f,1467.f}, PropType::TREE, Textures},    // 33
            Prop{Textures.TreeGreen, Vector2{3320.f,1467.f}, PropType::TREE, Textures},    // 34
            Prop{Textures.TreeGreen, Vector2{3420.f,1467.f}, PropType::TREE, Textures},    // 35
            Prop{Textures.TreeGreen, Vector2{3700.f,1467.f}, PropType::TREE, Textures},    // 36
            Prop{Textures.TreeGreen, Vector2{3800.f,1467.f}, PropType::TREE, Textures},    // 37
            Prop{Textures.TreeGreen, Vector2{3900.f,1467.f}, PropType::TREE, Textures},    // 38
            // row16
            Prop{Textures.TreeGreen, Vector2{0.f,1567.f}, PropType::TREE, Textures},       // 1 
            Prop{Textures.TreeGreen, Vector2{100.f,1567.f}, PropType::TREE, Textures},     // 2 
            Prop{Textures.TreeGreen, Vector2{200.f,1567.f}, PropType::TREE, Textures},     // 3 
            Prop{Textures.TreeGreen, Vector2{400.f,1567.f}, PropType::TREE, Textures},     // 5 
            Prop{Textures.TreeGreen, Vector2{500.f,1567.f}, PropType::TREE, Textures},     // 6 
            Prop{Textures.TreeGreen, Vector2{600.f,1567.f}, PropType::TREE, Textures},     // 7 
            Prop{Textures.TreeGreen, Vector2{700.f,1567.f}, PropType::TREE, Textures},     // 8 
            Prop{Textures.TreeGreen, Vector2{900.f,1567.f}, PropType::TREE, Textures},     // 10
            Prop{Textures.TreeGreen, Vector2{1000.f,1567.f}, PropType::TREE, Textures},    // 11    
            Prop{Textures.TreeGreen, Vector2{1100.f,1567.f}, PropType::TREE, Textures},    // 12
            Prop{Textures.TreeGreen, Vector2{1200.f,1567.f}, PropType::TREE, Textures},    // 13
            Prop{Textures.TreeGreen, Vector2{1300.f,1567.f}, PropType::TREE, Textures},    // 14
            Prop{Textures.TreeGreen, Vector2{1400.f,1567.f}, PropType::TREE, Textures},    // 15
            Prop{Textures.TreeGreen, Vector2{1500.f,1567.f}, PropType::TREE, Textures},    // 16
            Prop{Textures.TreeGreen, Vector2{1600.f,1567.f}, PropType::TREE, Textures},    // 17
            Prop{Textures.TreeGreen, Vector2{2500.f,1567.f}, PropType::TREE, Textures},    // 26
            Prop{Textures.TreeGreen, Vector2{2600.f,1567.f}, PropType::TREE, Textures},    // 27
            Prop{Textures.TreeGreen, Vector2{2700.f,1567.f}, PropType::TREE, Textures},    // 28
            Prop{Textures.TreeGreen, Vector2{2800.f,1567.f}, PropType::TREE, Textures},    // 29
            Prop{Textures.TreeGreen, Vector2{2900.f,1567.f}, PropType::TREE, Textures},    // 30
            Prop{Textures.TreeGreen, Vector2{3000.f,1567.f}, PropType::TREE, Textures},    // 31
            Prop{Textures.TreeGreen, Vector2{3100.f,1567.f}, PropType::TREE, Textures},    // 32
            Prop{Textures.TreeGreen, Vector2{3200.f,1567.f}, PropType::TREE, Textures},    // 33
            Prop{Textures.TreeGreen, Vector2{3300.f,1567.f}, PropType::TREE, Textures},    // 34
            Prop{Textures.TreeGreen, Vector2{3400.f,1567.f}, PropType::TREE, Textures},    // 35
            Prop{Textures.TreeGreen, Vector2{3720.f,1567.f}, PropType::TREE, Textures},    // 36 
            Prop{Textures.TreeGreen, Vector2{3820.f,1567.f}, PropType::TREE, Textures},    // 37
            Prop{Textures.TreeGreen, Vector2{3920.f,1567.f}, PropType::TREE, Textures},    // 38
            // row17
            Prop{Textures.TreeGreen, Vector2{20.f,1667.f}, PropType::TREE, Textures},      // 1 
            Prop{Textures.TreeGreen, Vector2{120.f,1667.f}, PropType::TREE, Textures},     // 2 
            Prop{Textures.TreeGreen, Vector2{220.f,1667.f}, PropType::TREE, Textures},     // 3 
            Prop{Textures.TreeGreen, Vector2{520.f,1667.f}, PropType::TREE, Textures},     // 6 
            Prop{Textures.TreeGreen, Vector2{620.f,1667.f}, PropType::TREE, Textures},     // 7 
            Prop{Textures.TreeGreen, Vector2{720.f,1667.f}, PropType::TREE, Textures},     // 8 
            Prop{Textures.TreeGreen, Vector2{820.f,1667.f}, PropType::TREE, Textures},     // 9 
            Prop{Textures.TreeGreen, Vector2{920.f,1667.f}, PropType::TREE, Textures},     // 10
            Prop{Textures.TreeGreen, Vector2{1120.f,1667.f}, PropType::TREE, Textures},    // 12
            Prop{Textures.TreeGreen, Vector2{1220.f,1667.f}, PropType::TREE, Textures},    // 13
            Prop{Textures.TreeGreen, Vector2{1320.f,1667.f}, PropType::TREE, Textures},    // 14
            Prop{Textures.TreeGreen, Vector2{2920.f,1667.f}, PropType::TREE, Textures},    // 30
            Prop{Textures.TreeGreen, Vector2{3020.f,1667.f}, PropType::TREE, Textures},    // 31
            Prop{Textures.TreeGreen, Vector2{3120.f,1667.f}, PropType::TREE, Textures},    // 32
            Prop{Textures.TreeGreen, Vector2{3220.f,1667.f}, PropType::TREE, Textures},    // 33
            Prop{Textures.TreeGreen, Vector2{3320.f,1667.f}, PropType::TREE, Textures},    // 34
            Prop{Textures.TreeGreen, Vector2{3420.f,1667.f}, PropType::TREE, Textures},    // 35
            Prop{Textures.TreeGreen, Vector2{3700.f,1667.f}, PropType::TREE, Textures},    // 36
            Prop{Textures.TreeGreen, Vector2{3800.f,1667.f}, PropType::TREE, Textures},    // 37
            Prop{Textures.TreeGreen, Vector2{3900.f,1667.f}, PropType::TREE, Textures},    // 38
            Prop{Textures.TreeGreen, Vector2{4000.f,1667.f}, PropType::TREE, Textures},    // 39
            // row18
            Prop{Textures.TreeGreen, Vector2{0.f,1767.f}, PropType::TREE, Textures},       // 1 
            Prop{Textures.TreeGreen, Vector2{100.f,1767.f}, PropType::TREE, Textures},     // 2 
            Prop{Textures.TreeGreen, Vector2{200.f,1767.f}, PropType::TREE, Textures},     // 3 
            Prop{Textures.TreeGreen, Vector2{300.f,1767.f}, PropType::TREE, Textures},     // 4 
            Prop{Textures.TreeGreen, Vector2{400.f,1767.f}, PropType::TREE, Textures},     // 5 
            Prop{Textures.TreeGreen, Vector2{500.f,1767.f}, PropType::TREE, Textures},     // 6 
            Prop{Textures.TreeGreen, Vector2{600.f,1767.f}, PropType::TREE, Textures},     // 7 
            Prop{Textures.TreeGreen, Vector2{700.f,1767.f}, PropType::TREE, Textures},     // 8 
            Prop{Textures.TreeGreen, Vector2{800.f,1767.f}, PropType::TREE, Textures},     // 9 
            Prop{Textures.TreeGreen, Vector2{900.f,1767.f}, PropType::TREE, Textures},     // 10
            Prop{Textures.TreeGreen, Vector2{1000.f,1767.f}, PropType::TREE, Textures},    // 11
            Prop{Textures.TreeGreen, Vector2{1100.f,1767.f}, PropType::TREE, Textures},    // 12
            Prop{Textures.TreeGreen, Vector2{1200.f,1767.f}, PropType::TREE, Textures},    // 13
            Prop{Textures.TreeGreen, Vector2{1300.f,1767.f}, PropType::TREE, Textures},    // 14
            Prop{Textures.TreeGreen, Vector2{1400.f,1767.f}, PropType::TREE, Textures},    // 15
            Prop{Textures.TreeGreen, Vector2{2500.f,1767.f}, PropType::TREE, Textures},    // 26
            Prop{Textures.TreeGreen, Vector2{2600.f,1767.f}, PropType::TREE, Textures},    // 27
            Prop{Textures.TreeGreen, Vector2{2700.f,1767.f}, PropType::TREE, Textures},    // 28
            Prop{Textures.TreeGreen, Vector2{2800.f,1767.f}, PropType::TREE, Textures},    // 29
            Prop{Textures.TreeGreen, Vector2{2900.f,1767.f}, PropType::TREE, Textures},    // 30
            Prop{Textures.TreeGreen, Vector2{3000.f,1767.f}, PropType::TREE, Textures},    // 31
            Prop{Textures.TreeGreen, Vector2{3100.f,1767.f}, PropType::TREE, Textures},    // 32
            Prop{Textures.TreeGreen, Vector2{3200.f,1767.f}, PropType::TREE, Textures},    // 33
            Prop{Textures.TreeGreen, Vector2{3300.f,1767.f}, PropType::TREE, Textures},    // 34
            Prop{Textures.TreeGreen, Vector2{3400.f,1767.f}, PropType::TREE, Textures},    // 35
            Prop{Textures.TreeGreen, Vector2{3720.f,1767.f}, PropType::TREE, Textures},    // 36
            Prop{Textures.TreeGreen, Vector2{3820.f,1767.f}, PropType::TREE, Textures},    // 37
            Prop{Textures.TreeGreen, Vector2{3920.f,1767.f}, PropType::TREE, Textures},    // 38
            // row19
            Prop{Textures.TreeGreen, Vector2{20.f,1867.f}, PropType::TREE, Textures},      // 1     
            Prop{Textures.TreeGreen, Vector2{220.f,1867.f}, PropType::TREE, Textures},     // 3     
            Prop{Textures.TreeGreen, Vector2{320.f,1867.f}, PropType::TREE, Textures},     // 4 
            Prop{Textures.TreeGreen, Vector2{420.f,1867.f}, PropType::TREE, Textures},     // 5 
            Prop{Textures.TreeGreen, Vector2{520.f,1867.f}, PropType::TREE, Textures},     // 6 
            Prop{Textures.TreeGreen, Vector2{620.f,1867.f}, PropType::TREE, Textures},     // 7 
            Prop{Textures.TreeGreen, Vector2{820.f,1867.f}, PropType::TREE, Textures},     // 9 
            Prop{Textures.TreeGreen, Vector2{920.f,1867.f}, PropType::TREE, Textures},     // 10
            Prop{Textures.TreeGreen, Vector2{1020.f,1867.f}, PropType::TREE, Textures},    // 11
            Prop{Textures.TreeGreen, Vector2{1120.f,1867.f}, PropType::TREE, Textures},    // 12
            Prop{Textures.TreeGreen, Vector2{1220.f,1867.f}, PropType::TREE, Textures},    // 13
            Prop{Textures.TreeGreen, Vector2{1320.f,1867.f}, PropType::TREE, Textures},    // 14
            Prop{Textures.TreeGreen, Vector2{1420.f,1867.f}, PropType::TREE, Textures},    // 15
            Prop{Textures.TreeGreen, Vector2{1520.f,1867.f}, PropType::TREE, Textures},    // 16
            Prop{Textures.TreeGreen, Vector2{1620.f,1867.f}, PropType::TREE, Textures},    // 17
            Prop{Textures.TreeGreen, Vector2{2720.f,1867.f}, PropType::TREE, Textures},    // 28
            Prop{Textures.TreeGreen, Vector2{2820.f,1867.f}, PropType::TREE, Textures},    // 29
            Prop{Textures.TreeGreen, Vector2{2920.f,1867.f}, PropType::TREE, Textures},    // 30
            Prop{Textures.TreeGreen, Vector2{3020.f,1867.f}, PropType::TREE, Textures},    // 31
            Prop{Textures.TreeGreen, Vector2{3120.f,1867.f}, PropType::TREE, Textures},    // 32
            Prop{Textures.TreeGreen, Vector2{3220.f,1867.f}, PropType::TREE, Textures},    // 33
            Prop{Textures.TreeGreen, Vector2{3320.f,1867.f}, PropType::TREE, Textures},    // 34
            Prop{Textures.TreeGreen, Vector2{3420.f,1867.f}, PropType::TREE, Textures},    // 35
            Prop{Textures.TreeGreen, Vector2{3700.f,1867.f}, PropType::TREE, Textures},    // 36
            Prop{Textures.TreeGreen, Vector2{3800.f,1867.f}, PropType::TREE, Textures},    // 37
            Prop{Textures.TreeGreen, Vector2{3900.f,1867.f}, PropType::TREE, Textures},    // 38
            Prop{Textures.TreeGreen, Vector2{4000.f,1867.f}, PropType::TREE, Textures},    // 39
            // row20
            Prop{Textures.TreeGreen, Vector2{0.f,1967.f}, PropType::TREE, Textures},       // 1     
            Prop{Textures.TreeGreen, Vector2{100.f,1967.f}, PropType::TREE, Textures},     // 2 
            Prop{Textures.TreeGreen, Vector2{200.f,1967.f}, PropType::TREE, Textures},     // 3 
            Prop{Textures.TreeGreen, Vector2{400.f,1967.f}, PropType::TREE, Textures},     // 5 
            Prop{Textures.TreeGreen, Vector2{500.f,1967.f}, PropType::TREE, Textures},     // 6 
            Prop{Textures.TreeGreen, Vector2{600.f,1967.f}, PropType::TREE, Textures},     // 7 
            Prop{Textures.TreeGreen, Vector2{700.f,1967.f}, PropType::TREE, Textures},     // 8 
            Prop{Textures.TreeGreen, Vector2{800.f,1967.f}, PropType::TREE, Textures},     // 9 
            Prop{Textures.TreeGreen, Vector2{1000.f,1967.f}, PropType::TREE, Textures},    // 11
            Prop{Textures.TreeGreen, Vector2{1100.f,1967.f}, PropType::TREE, Textures},    // 12
            Prop{Textures.TreeGreen, Vector2{1300.f,1967.f}, PropType::TREE, Textures},    // 14
            Prop{Textures.TreeGreen, Vector2{1400.f,1967.f}, PropType::TREE, Textures},    // 15
            Prop{Textures.TreeGreen, Vector2{1500.f,1967.f}, PropType::TREE, Textures},    // 16
            Prop{Textures.TreeGreen, Vector2{1600.f,1967.f}, PropType::TREE, Textures},    // 17
            Prop{Textures.TreeGreen, Vector2{1800.f,1967.f}, PropType::TREE, Textures},    // 19
            Prop{Textures.TreeGreen, Vector2{1900.f,1967.f}, PropType::TREE, Textures},    // 20
            Prop{Textures.TreeGreen, Vector2{2600.f,1967.f}, PropType::TREE, Textures},    // 27
            Prop{Textures.TreeGreen, Vector2{2700.f,1967.f}, PropType::TREE, Textures},    // 28
            Prop{Textures.TreeGreen, Vector2{2900.f,1967.f}, PropType::TREE, Textures},    // 30
            Prop{Textures.TreeGreen, Vector2{3000.f,1967.f}, PropType::TREE, Textures},    // 31
            Prop{Textures.TreeGreen, Vector2{3100.f,1967.f}, PropType::TREE, Textures},    // 32
            Prop{Textures.TreeGreen, Vector2{3200.f,1967.f}, PropType::TREE, Textures},    // 33
            Prop{Textures.TreeGreen, Vector2{3400.f,1967.f}, PropType::TREE, Textures},    // 35
            Prop{Textures.TreeGreen, Vector2{3500.f,1967.f}, PropType::TREE, Textures},    // 35
            Prop{Textures.TreeGreen, Vector2{3600.f,1967.f}, PropType::TREE, Textures},    // 35
            Prop{Textures.TreeGreen, Vector2{3720.f,1967.f}, PropType::TREE, Textures},    // 36
            Prop{Textures.TreeGreen, Vector2{3820.f,1967.f}, PropType::TREE, Textures},    // 37
            Prop{Textures.TreeGreen, Vector2{3920.f,1967.f}, PropType::TREE, Textures},    // 38
            // row21
            Prop{Textures.TreeGreen, Vector2{20.f,2067.f}, PropType::TREE, Textures},      // 1 
            Prop{Textures.TreeGreen, Vector2{220.f,2067.f}, PropType::TREE, Textures},     // 3 
            Prop{Textures.TreeGreen, Vector2{320.f,2067.f}, PropType::TREE, Textures},     // 4 
            Prop{Textures.TreeGreen, Vector2{420.f,2067.f}, PropType::TREE, Textures},     // 5 
            Prop{Textures.TreeGreen, Vector2{520.f,2067.f}, PropType::TREE, Textures},     // 6 
            Prop{Textures.TreeGreen, Vector2{620.f,2067.f}, PropType::TREE, Textures},     // 7 
            Prop{Textures.TreeGreen, Vector2{720.f,2067.f}, PropType::TREE, Textures},     // 8 
            Prop{Textures.TreeGreen, Vector2{820.f,2067.f}, PropType::TREE, Textures},     // 9 
            Prop{Textures.TreeGreen, Vector2{920.f,2067.f}, PropType::TREE, Textures},     // 10
            Prop{Textures.TreeGreen, Vector2{1020.f,2067.f}, PropType::TREE, Textures},    // 11
            Prop{Textures.TreeGreen, Vector2{1120.f,2067.f}, PropType::TREE, Textures},    // 12
            Prop{Textures.TreeGreen, Vector2{1320.f,2067.f}, PropType::TREE, Textures},    // 14
            Prop{Textures.TreeGreen, Vector2{1420.f,2067.f}, PropType::TREE, Textures},    // 15
            Prop{Textures.TreeGreen, Vector2{1820.f,2067.f}, PropType::TREE, Textures},    // 19
            Prop{Textures.TreeGreen, Vector2{2720.f,2067.f}, PropType::TREE, Textures},    // 28
            Prop{Textures.TreeGreen, Vector2{2820.f,2067.f}, PropType::TREE, Textures},    // 29
            Prop{Textures.TreeGreen, Vector2{2920.f,2067.f}, PropType::TREE, Textures},    // 30
            Prop{Textures.TreeGreen, Vector2{3220.f,2067.f}, PropType::TREE, Textures},    // 33
            Prop{Textures.TreeGreen, Vector2{3320.f,2067.f}, PropType::TREE, Textures},    // 34
            Prop{Textures.TreeGreen, Vector2{3420.f,2067.f}, PropType::TREE, Textures},    // 35
            Prop{Textures.TreeGreen, Vector2{3520.f,2067.f}, PropType::TREE, Textures},    // 35
            Prop{Textures.TreeGreen, Vector2{3620.f,2067.f}, PropType::TREE, Textures},    // 35
            Prop{Textures.TreeGreen, Vector2{3700.f,2067.f}, PropType::TREE, Textures},    // 36
            Prop{Textures.TreeGreen, Vector2{3800.f,2067.f}, PropType::TREE, Textures},    // 37
            Prop{Textures.TreeGreen, Vector2{3900.f,2067.f}, PropType::TREE, Textures},    // 38
            Prop{Textures.TreeGreen, Vector2{4000.f,2067.f}, PropType::TREE, Textures},    // 39
            // row22
            Prop{Textures.TreeGreen, Vector2{0.f,2167.f}, PropType::TREE, Textures},       // 1 
            Prop{Textures.TreeGreen, Vector2{100.f,2167.f}, PropType::TREE, Textures},     // 2 
            Prop{Textures.TreeGreen, Vector2{200.f,2167.f}, PropType::TREE, Textures},     // 3 
            Prop{Textures.TreeGreen, Vector2{300.f,2167.f}, PropType::TREE, Textures},     // 4 
            Prop{Textures.TreeGreen, Vector2{400.f,2167.f}, PropType::TREE, Textures},     // 5 
            Prop{Textures.TreeGreen, Vector2{600.f,2167.f}, PropType::TREE, Textures},     // 7 
            Prop{Textures.TreeGreen, Vector2{700.f,2167.f}, PropType::TREE, Textures},     // 8 
            Prop{Textures.TreeGreen, Vector2{800.f,2167.f}, PropType::TREE, Textures},     // 9 
            Prop{Textures.TreeGreen, Vector2{900.f,2167.f}, PropType::TREE, Textures},     // 10
            Prop{Textures.TreeGreen, Vector2{1000.f,2167.f}, PropType::TREE, Textures},    // 11
            Prop{Textures.TreeGreen, Vector2{1300.f,2167.f}, PropType::TREE, Textures},    // 14
            Prop{Textures.TreeGreen, Vector2{1400.f,2167.f}, PropType::TREE, Textures},    // 15
            Prop{Textures.TreeGreen, Vector2{1500.f,2167.f}, PropType::TREE, Textures},    // 16
            Prop{Textures.TreeGreen, Vector2{1600.f,2167.f}, PropType::TREE, Textures},    // 17
            Prop{Textures.TreeGreen, Vector2{1700.f,2167.f}, PropType::TREE, Textures},    // 18
            Prop{Textures.TreeGreen, Vector2{1800.f,2167.f}, PropType::TREE, Textures},    // 19
            Prop{Textures.TreeGreen, Vector2{1900.f,2167.f}, PropType::TREE, Textures},    // 20
            Prop{Textures.TreeGreen, Vector2{2600.f,2167.f}, PropType::TREE, Textures},    // 27
            Prop{Textures.TreeGreen, Vector2{2700.f,2167.f}, PropType::TREE, Textures},    // 28
            Prop{Textures.TreeGreen, Vector2{2800.f,2167.f}, PropType::TREE, Textures},    // 29
            Prop{Textures.TreeGreen, Vector2{2900.f,2167.f}, PropType::TREE, Textures},    // 30
            Prop{Textures.TreeGreen, Vector2{3000.f,2167.f}, PropType::TREE, Textures},    // 31
            Prop{Textures.TreeGreen, Vector2{3100.f,2167.f}, PropType::TREE, Textures},    // 32
            Prop{Textures.TreeGreen, Vector2{3200.f,2167.f}, PropType::TREE, Textures},    // 33
            Prop{Textures.TreeGreen, Vector2{3300.f,2167.f}, PropType::TREE, Textures},    // 34
            Prop{Textures.TreeGreen, Vector2{3400.f,2167.f}, PropType::TREE, Textures},    // 35
            Prop{Textures.TreeGreen, Vector2{3500.f,2167.f}, PropType::TREE, Textures},    // 35
            Prop{Textures.TreeGreen, Vector2{3600.f,2167.f}, PropType::TREE, Textures},    // 35
            Prop{Textures.TreeGreen, Vector2{3720.f,2167.f}, PropType::TREE, Textures},    // 36
            Prop{Textures.TreeGreen, Vector2{3820.f,2167.f}, PropType::TREE, Textures},    // 37    
            Prop{Textures.TreeGreen, Vector2{3920.f,2167.f}, PropType::TREE, Textures},    // 38
            // row21
            Prop{Textures.TreeGreen, Vector2{20.f,2267.f}, PropType::TREE, Textures},      // 1
            Prop{Textures.TreeGreen, Vector2{120.f,2267.f}, PropType::TREE, Textures},     // 2
            Prop{Textures.TreeGreen, Vector2{220.f,2267.f}, PropType::TREE, Textures},     // 3   
            Prop{Textures.TreeGreen, Vector2{320.f,2267.f}, PropType::TREE, Textures},     // 4
            Prop{Textures.TreeGreen, Vector2{420.f,2267.f}, PropType::TREE, Textures},     // 5    
            Prop{Textures.TreeGreen, Vector2{520.f,2267.f}, PropType::TREE, Textures},     // 6
            Prop{Textures.TreeGreen, Vector2{620.f,2267.f}, PropType::TREE, Textures},     // 7
            Prop{Textures.TreeGreen, Vector2{720.f,2267.f}, PropType::TREE, Textures},     // 8
            Prop{Textures.TreeGreen, Vector2{820.f,2267.f}, PropType::TREE, Textures},     // 9
            Prop{Textures.TreeGreen, Vector2{920.f,2267.f}, PropType::TREE, Textures},     // 10
            Prop{Textures.TreeGreen, Vector2{1020.f,2267.f}, PropType::TREE, Textures},    // 11
            Prop{Textures.TreeGreen, Vector2{1120.f,2267.f}, PropType::TREE, Textures},    // 12
            Prop{Textures.TreeGreen, Vector2{1220.f,2267.f}, PropType::TREE, Textures},    // 13
            Prop{Textures.TreeGreen, Vector2{1320.f,2267.f}, PropType::TREE, Textures},    // 14
            Prop{Textures.TreeGreen, Vector2{1420.f,2267.f}, PropType::TREE, Textures},    // 15
            Prop{Textures.TreeGreen, Vector2{1520.f,2267.f}, PropType::TREE, Textures},    // 16
            Prop{Textures.TreeGreen, Vector2{1620.f,2267.f}, PropType::TREE, Textures},    // 17
            Prop{Textures.TreeGreen, Vector2{2620.f,2267.f}, PropType::TREE, Textures},    // 27
            Prop{Textures.TreeGreen, Vector2{2720.f,2267.f}, PropType::TREE, Textures},    // 28
            Prop{Textures.TreeGreen, Vector2{2820.f,2267.f}, PropType::TREE, Textures},    // 29
            Prop{Textures.TreeGreen, Vector2{2920.f,2267.f}, PropType::TREE, Textures},    // 30
            Prop{Textures.TreeGreen, Vector2{3020.f,2267.f}, PropType::TREE, Textures},    // 31
            Prop{Textures.TreeGreen, Vector2{3120.f,2267.f}, PropType::TREE, Textures},    // 32
            Prop{Textures.TreeGreen, Vector2{3220.f,2267.f}, PropType::TREE, Textures},    // 33
            Prop{Textures.TreeGreen, Vector2{3320.f,2267.f}, PropType::TREE, Textures},    // 34
            Prop{Textures.TreeGreen, Vector2{3420.f,2267.f}, PropType::TREE, Textures},    // 35
            Prop{Textures.TreeGreen, Vector2{3520.f,2267.f}, PropType::TREE, Textures},    // 35
            Prop{Textures.TreeGreen, Vector2{3620.f,2267.f}, PropType::TREE, Textures},    // 35
            Prop{Textures.TreeGreen, Vector2{3700.f,2267.f}, PropType::TREE, Textures},    // 36
            Prop{Textures.TreeGreen, Vector2{3800.f,2267.f}, PropType::TREE, Textures},    // 37
            Prop{Textures.TreeGreen, Vector2{3900.f,2267.f}, PropType::TREE, Textures},    // 38
            Prop{Textures.TreeGreen, Vector2{4000.f,2267.f}, PropType::TREE, Textures},    // 39
            // row22
            Prop{Textures.TreeGreen, Vector2{50.f,2367.f}, PropType::TREE, Textures},      // 1 
            Prop{Textures.TreeClear, Vector2{550.f,2303.f}, PropType::TREE, Textures},     // 6 Invisible Wall
            Prop{Textures.TreeGreen, Vector2{550.f,2367.f}, PropType::TREE, Textures},     // 6 
            Prop{Textures.TreeGreen, Vector2{700.f,2367.f}, PropType::TREE, Textures},     // 8 
            Prop{Textures.TreeGreen, Vector2{800.f,2367.f}, PropType::TREE, Textures},     // 9 
            Prop{Textures.TreeGreen, Vector2{900.f,2367.f}, PropType::TREE, Textures},     // 10
            Prop{Textures.TreeGreen, Vector2{1000.f,2367.f}, PropType::TREE, Textures},    // 11
            Prop{Textures.TreeGreen, Vector2{1100.f,2367.f}, PropType::TREE, Textures},    // 12
            Prop{Textures.TreeGreen, Vector2{1200.f,2367.f}, PropType::TREE, Textures},    // 13
            Prop{Textures.TreeGreen, Vector2{1300.f,2367.f}, PropType::TREE, Textures},    // 14
            Prop{Textures.TreeGreen, Vector2{1400.f,2367.f}, PropType::TREE, Textures},    // 15
            Prop{Textures.TreeGreen, Vector2{1500.f,2367.f}, PropType::TREE, Textures},    // 16
            Prop{Textures.TreeGreen, Vector2{1600.f,2367.f}, PropType::TREE, Textures},    // 17
            Prop{Textures.TreeGreen, Vector2{1700.f,2367.f}, PropType::TREE, Textures},    // 18
            Prop{Textures.TreeGreen, Vector2{1800.f,2367.f}, PropType::TREE, Textures},    // 19
            Prop{Textures.TreeGreen, Vector2{2600.f,2340.f}, PropType::TREE, Textures},    // 27
            Prop{Textures.TreeGreen, Vector2{2700.f,2340.f}, PropType::TREE, Textures},    // 28
            Prop{Textures.TreeGreen, Vector2{2800.f,2340.f}, PropType::TREE, Textures},    // 29
            Prop{Textures.TreeGreen, Vector2{2900.f,2340.f}, PropType::TREE, Textures},    // 30
            Prop{Textures.TreeGreen, Vector2{3000.f,2340.f}, PropType::TREE, Textures},    // 31
            Prop{Textures.TreeGreen, Vector2{3100.f,2340.f}, PropType::TREE, Textures},    // 32
            Prop{Textures.TreeGreen, Vector2{3200.f,2340.f}, PropType::TREE, Textures},    // 33
            Prop{Textures.TreeGreen, Vector2{3300.f,2340.f}, PropType::TREE, Textures},    // 34
            Prop{Textures.TreeGreen, Vector2{3400.f,2340.f}, PropType::TREE, Textures},    // 35
            Prop{Textures.TreeGreen, Vector2{3500.f,2340.f}, PropType::TREE, Textures},    // 36
            Prop{Textures.TreeGreen, Vector2{3600.f,2340.f}, PropType::TREE, Textures},    // 37
            Prop{Textures.TreeGreen, Vector2{3720.f,2340.f}, PropType::TREE, Textures},    // 38
            Prop{Textures.TreeGreen, Vector2{3820.f,2340.f}, PropType::TREE, Textures},    // 39
            Prop{Textures.TreeGreen, Vector2{3920.f,2340.f}, PropType::TREE, Textures},    // 40
            // row23
            Prop{Textures.TreeGreen, Vector2{20.f,2467.f}, PropType::TREE, Textures},      // 1 
            Prop{Textures.TreeClear, Vector2{620.f,2403.f}, PropType::TREE, Textures},     // 7 Invisible Wall
            Prop{Textures.TreeGreen, Vector2{620.f,2467.f}, PropType::TREE, Textures},     // 7 
            Prop{Textures.TreeGreen, Vector2{720.f,2467.f}, PropType::TREE, Textures},     // 8 
            Prop{Textures.TreeGreen, Vector2{820.f,2467.f}, PropType::TREE, Textures},     // 9 
            Prop{Textures.TreeGreen, Vector2{920.f,2467.f}, PropType::TREE, Textures},     // 10
            Prop{Textures.TreeGreen, Vector2{1020.f,2467.f}, PropType::TREE, Textures},    // 11    
            Prop{Textures.TreeGreen, Vector2{1120.f,2467.f}, PropType::TREE, Textures},    // 12    
            Prop{Textures.TreeGreen, Vector2{1220.f,2467.f}, PropType::TREE, Textures},    // 13
            Prop{Textures.TreeGreen, Vector2{1320.f,2467.f}, PropType::TREE, Textures},    // 14
            Prop{Textures.TreeGreen, Vector2{1420.f,2467.f}, PropType::TREE, Textures},    // 15
            Prop{Textures.TreeGreen, Vector2{1520.f,2467.f}, PropType::TREE, Textures},    // 16
            Prop{Textures.TreeGreen, Vector2{1620.f,2467.f}, PropType::TREE, Textures},    // 17
            Prop{Textures.TreeGreen, Vector2{1720.f,2467.f}, PropType::TREE, Textures},    // 18
            Prop{Textures.TreeGreen, Vector2{1820.f,2467.f}, PropType::TREE, Textures},    // 19
            Prop{Textures.TreeGreen, Vector2{1920.f,2467.f}, PropType::TREE, Textures},    // 20
            // row24
            Prop{Textures.TreeGreen, Vector2{50.f,2567.f}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{650.f,2567.f}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{1980.f,2567.f}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{2490.f,2567.f}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{2590.f,2567.f}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{2800.f,2567.f}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{3920.f,2567.f}, PropType::TREE, Textures},
            // row25
            Prop{Textures.TreeGreen, Vector2{20.f,2667.f}, PropType::TREE, Textures}, 
            Prop{Textures.TreeGreen, Vector2{620.f,2667.f}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{2420.f,2667.f}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{2520.f,2667.f}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{2820.f,2667.f}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{4000.f,2667.f}, PropType::TREE, Textures},
            // row26
            Prop{Textures.TreeGreen, Vector2{50.f,2767.f}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{100.f,2767.f}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{550.f,2767.f}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{650.f,2767.f}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{1980.f,2767.f}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{2490.f,2767.f}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{2590.f,2767.f}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{2800.f,2767.f}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{3920.f,2767.f}, PropType::TREE, Textures},
            // row27
            Prop{Textures.TreeGreen, Vector2{20.f,2867.f}, PropType::TREE, Textures}, 
            Prop{Textures.TreeGreen, Vector2{120.f,2867.f}, PropType::TREE, Textures}, 
            Prop{Textures.TreeGreen, Vector2{520.f,2867.f}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{620.f,2867.f}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{2420.f,2867.f}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{2520.f,2867.f}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{2820.f,2867.f}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{4000.f,2867.f}, PropType::TREE, Textures},
            // row28
            Prop{Textures.TreeGreen, Vector2{50.f,2967.f}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{150.f,2967.f}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{450.f,2967.f}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{550.f,2967.f}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{650.f,2967.f}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{1980.f,2967.f}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{2490.f,2967.f}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{2590.f,2967.f}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{3920.f,2967.f}, PropType::TREE, Textures},
            // row29
            Prop{Textures.TreeGreen, Vector2{20.f,3067.f}, PropType::TREE, Textures}, 
            Prop{Textures.TreeGreen, Vector2{120.f,3067.f}, PropType::TREE, Textures}, 
            Prop{Textures.TreeGreen, Vector2{220.f,3067.f}, PropType::TREE, Textures}, 
            Prop{Textures.TreeGreen, Vector2{420.f,3067.f}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{520.f,3067.f}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{620.f,3067.f}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{2420.f,3067.f}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{2520.f,3067.f}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{4000.f,3067.f}, PropType::TREE, Textures},
            // row30
            Prop{Textures.TreeGreen, Vector2{50.f,3167.f}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{150.f,3167.f}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{250.f,3167.f}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{450.f,3167.f}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{550.f,3167.f}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{650.f,3167.f}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{1980.f,3167.f}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{2490.f,3167.f}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{2590.f,3167.f}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{3920.f,3167.f}, PropType::TREE, Textures},
            // row31
            Prop{Textures.TreeGreen, Vector2{20.f,3267.f}, PropType::TREE, Textures}, 
            Prop{Textures.TreeGreen, Vector2{120.f,3267.f}, PropType::TREE, Textures}, 
            Prop{Textures.TreeGreen, Vector2{220.f,3267.f}, PropType::TREE, Textures}, 
            Prop{Textures.TreeGreen, Vector2{530.f,3267.f}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{620.f,3267.f}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{2420.f,3267.f}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{2520.f,3267.f}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{4000.f,3267.f}, PropType::TREE, Textures},
            // row32
            Prop{Textures.TreeGreen, Vector2{50.f,3367.f}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{150.f,3367.f}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{250.f,3367.f}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{350.f,3367.f}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{650.f,3367.f}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{2490.f,3367.f}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{2590.f,3367.f}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{3920.f,3367.f}, PropType::TREE, Textures},
            // row33
            Prop{Textures.TreeGreen, Vector2{20.f,3467.f}, PropType::TREE, Textures},      
            Prop{Textures.TreeGreen, Vector2{120.f,3467.f}, PropType::TREE, Textures},         
            Prop{Textures.TreeGreen, Vector2{220.f,3467.f}, PropType::TREE, Textures},         
            Prop{Textures.TreeGreen, Vector2{320.f,3467.f}, PropType::TREE, Textures},     
            Prop{Textures.TreeGreen, Vector2{420.f,3467.f}, PropType::TREE, Textures},     
            Prop{Textures.TreeGreen, Vector2{620.f,3467.f}, PropType::TREE, Textures},     
            Prop{Textures.TreeGreen, Vector2{4000.f,3467.f}, PropType::TREE, Textures},    
            // row34
            Prop{Textures.TreeGreen, Vector2{0.f,3567.f}, PropType::TREE, Textures},       // 1 
            Prop{Textures.TreeGreen, Vector2{150.f,3567.f}, PropType::TREE, Textures},     // 2 
            Prop{Textures.TreeGreen, Vector2{250.f,3567.f}, PropType::TREE, Textures},     // 3 
            Prop{Textures.TreeGreen, Vector2{350.f,3567.f}, PropType::TREE, Textures},     // 4 
            Prop{Textures.TreeGreen, Vector2{665.f,3530.f}, PropType::TREE, Textures},     // 6 
            Prop{Textures.TreeGreen, Vector2{3920.f,3567.f}, PropType::TREE, Textures},    // 21
            // row35
            Prop{Textures.TreeGreen, Vector2{100.f,3740.f}, PropType::TREE, Textures}, 
            Prop{Textures.TreeGreen, Vector2{200.f,3740.f}, PropType::TREE, Textures}, 
            Prop{Textures.TreeGreen, Vector2{300.f,3740.f}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{500.f,3740.f}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{800.f,3740.f}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{900.f,3740.f}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{1600.f,3740.f}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{1700.f,3740.f}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{1800.f,3740.f}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{1900.f,3740.f}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{2000.f,3740.f}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{2200.f,3740.f}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{2300.f,3740.f}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{2400.f,3740.f}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{2600.f,3740.f}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{2700.f,3740.f}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{2900.f,3740.f}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{3000.f,3740.f}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{3100.f,3740.f}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{3300.f,3740.f}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{3800.f,3740.f}, PropType::TREE, Textures},
            Prop{Textures.TreeGreen, Vector2{3900.f,3740.f}, PropType::TREE, Textures},
            // row36
            Prop{Textures.TreeGreen, Vector2{20.f,3840.f}, PropType::TREE, Textures},      // 1 
            Prop{Textures.TreeGreen, Vector2{120.f,3840.f}, PropType::TREE, Textures},     // 2 
            Prop{Textures.TreeGreen, Vector2{220.f,3840.f}, PropType::TREE, Textures},     // 3 
            Prop{Textures.TreeGreen, Vector2{420.f,3840.f}, PropType::TREE, Textures},     // 5 
            Prop{Textures.TreeGreen, Vector2{520.f,3840.f}, PropType::TREE, Textures},     // 6 
            Prop{Textures.TreeGreen, Vector2{720.f,3840.f}, PropType::TREE, Textures},     // 8 
            Prop{Textures.TreeGreen, Vector2{820.f,3840.f}, PropType::TREE, Textures},     // 9 
            Prop{Textures.TreeGreen, Vector2{1020.f,3840.f}, PropType::TREE, Textures},    // 11
            Prop{Textures.TreeGreen, Vector2{1120.f,3840.f}, PropType::TREE, Textures},    // 12
            Prop{Textures.TreeGreen, Vector2{1220.f,3840.f}, PropType::TREE, Textures},    // 13
            Prop{Textures.TreeGreen, Vector2{1420.f,3840.f}, PropType::TREE, Textures},    // 15
            Prop{Textures.TreeGreen, Vector2{1520.f,3840.f}, PropType::TREE, Textures},    // 16
            Prop{Textures.TreeGreen, Vector2{1620.f,3840.f}, PropType::TREE, Textures},    // 17
            Prop{Textures.TreeGreen, Vector2{1720.f,3840.f}, PropType::TREE, Textures},    // 18
            Prop{Textures.TreeGreen, Vector2{1920.f,3840.f}, PropType::TREE, Textures},    // 20
            Prop{Textures.TreeGreen, Vector2{2020.f,3840.f}, PropType::TREE, Textures},    // 21
            Prop{Textures.TreeGreen, Vector2{2120.f,3840.f}, PropType::TREE, Textures},    // 22
            Prop{Textures.TreeGreen, Vector2{2320.f,3840.f}, PropType::TREE, Textures},    // 24
            Prop{Textures.TreeGreen, Vector2{2420.f,3840.f}, PropType::TREE, Textures},    // 25
            Prop{Textures.TreeGreen, Vector2{2520.f,3840.f}, PropType::TREE, Textures},    // 26
            Prop{Textures.TreeGreen, Vector2{2720.f,3840.f}, PropType::TREE, Textures},    // 28
            Prop{Textures.TreeGreen, Vector2{2820.f,3840.f}, PropType::TREE, Textures},    // 29
            Prop{Textures.TreeGreen, Vector2{2920.f,3840.f}, PropType::TREE, Textures},    // 30
            Prop{Textures.TreeGreen, Vector2{3020.f,3840.f}, PropType::TREE, Textures},    // 31
            Prop{Textures.TreeGreen, Vector2{3120.f,3840.f}, PropType::TREE, Textures},    // 32
            Prop{Textures.TreeGreen, Vector2{3420.f,3840.f}, PropType::TREE, Textures},    // 35
            Prop{Textures.TreeGreen, Vector2{3520.f,3840.f}, PropType::TREE, Textures},    // 36
            Prop{Textures.TreeGreen, Vector2{3720.f,3840.f}, PropType::TREE, Textures},    // 38
            Prop{Textures.TreeGreen, Vector2{3820.f,3840.f}, PropType::TREE, Textures},    // 39
            Prop{Textures.TreeGreen, Vector2{3920.f,3840.f}, PropType::TREE, Textures},    // 40
            // row37
            Prop{Textures.TreeGreen, Vector2{0.f,3940.f}, PropType::TREE, Textures},       // 1 
            Prop{Textures.TreeGreen, Vector2{100.f,3940.f}, PropType::TREE, Textures},     // 2 
            Prop{Textures.TreeGreen, Vector2{200.f,3940.f}, PropType::TREE, Textures},     // 3 
            Prop{Textures.TreeGreen, Vector2{300.f,3940.f}, PropType::TREE, Textures},     // 4  
            Prop{Textures.TreeGreen, Vector2{400.f,3940.f}, PropType::TREE, Textures},     // 5 
            Prop{Textures.TreeGreen, Vector2{500.f,3940.f}, PropType::TREE, Textures},     // 6 
            Prop{Textures.TreeGreen, Vector2{600.f,3940.f}, PropType::TREE, Textures},     // 7 
            Prop{Textures.TreeGreen, Vector2{700.f,3940.f}, PropType::TREE, Textures},     // 8 
            Prop{Textures.TreeGreen, Vector2{800.f,3940.f}, PropType::TREE, Textures},     // 9 
            Prop{Textures.TreeGreen, Vector2{900.f,3940.f}, PropType::TREE, Textures},     // 10
            Prop{Textures.TreeGreen, Vector2{1000.f,3940.f}, PropType::TREE, Textures},    // 11  
            Prop{Textures.TreeGreen, Vector2{1100.f,3940.f}, PropType::TREE, Textures},    // 12
            Prop{Textures.TreeGreen, Vector2{1200.f,3940.f}, PropType::TREE, Textures},    // 13
            Prop{Textures.TreeGreen, Vector2{1300.f,3940.f}, PropType::TREE, Textures},    // 14
            Prop{Textures.TreeGreen, Vector2{1400.f,3940.f}, PropType::TREE, Textures},    // 15
            Prop{Textures.TreeGreen, Vector2{1500.f,3940.f}, PropType::TREE, Textures},    // 16
            Prop{Textures.TreeGreen, Vector2{1600.f,3940.f}, PropType::TREE, Textures},    // 17
            Prop{Textures.TreeGreen, Vector2{1700.f,3940.f}, PropType::TREE, Textures},    // 18
            Prop{Textures.TreeGreen, Vector2{1800.f,3940.f}, PropType::TREE, Textures},    // 19
            Prop{Textures.TreeGreen, Vector2{1900.f,3940.f}, PropType::TREE, Textures},    // 20
            Prop{Textures.TreeGreen, Vector2{2000.f,3940.f}, PropType::TREE, Textures},    // 21
            Prop{Textures.TreeGreen, Vector2{2100.f,3940.f}, PropType::TREE, Textures},    // 22
            Prop{Textures.TreeGreen, Vector2{2200.f,3940.f}, PropType::TREE, Textures},    // 23
            Prop{Textures.TreeGreen, Vector2{2300.f,3940.f}, PropType::TREE, Textures},    // 24
            Prop{Textures.TreeGreen, Vector2{2400.f,3940.f}, PropType::TREE, Textures},    // 25
            Prop{Textures.TreeGreen, Vector2{2500.f,3940.f}, PropType::TREE, Textures},    // 26
            Prop{Textures.TreeGreen, Vector2{2600.f,3940.f}, PropType::TREE, Textures},    // 27 
            Prop{Textures.TreeGreen, Vector2{2700.f,3940.f}, PropType::TREE, Textures},    // 28
            Prop{Textures.TreeGreen, Vector2{2800.f,3940.f}, PropType::TREE, Textures},    // 29
            Prop{Textures.TreeGreen, Vector2{2900.f,3940.f}, PropType::TREE, Textures},    // 30
            Prop{Textures.TreeGreen, Vector2{3000.f,3940.f}, PropType::TREE, Textures},    // 31
            Prop{Textures.TreeGreen, Vector2{3100.f,3940.f}, PropType::TREE, Textures},    // 32
            Prop{Textures.TreeGreen, Vector2{3200.f,3940.f}, PropType::TREE, Textures},    // 33
            Prop{Textures.TreeGreen, Vector2{3300.f,3940.f}, PropType::TREE, Textures},    // 34
            Prop{Textures.TreeGreen, Vector2{3400.f,3940.f}, PropType::TREE, Textures},    // 35 
            Prop{Textures.TreeGreen, Vector2{3500.f,3940.f}, PropType::TREE, Textures},    // 36 
            Prop{Textures.TreeGreen, Vector2{3600.f,3940.f}, PropType::TREE, Textures},    // 37
            Prop{Textures.TreeGreen, Vector2{3700.f,3940.f}, PropType::TREE, Textures},    // 38
            Prop{Textures.TreeGreen, Vector2{3800.f,3940.f}, PropType::TREE, Textures},    // 39
            Prop{Textures.TreeGreen, Vector2{3900.f,3940.f}, PropType::TREE, Textures},    // 40 
            Prop{Textures.TreeGreen, Vector2{4000.f,3940.f}, PropType::TREE, Textures},    // 41
            // large tree
            Prop{Textures.TreeGreen, Vector2{2185.f,2050.f}, PropType::TREE, Textures, 8.f}
        };
    }

    std::vector<Enemy> InitializeEnemies(Background& MapBG, const Window& Window, const GameTexture& Textures, Randomizer& RandomEngine)
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
            EnemyType::BEAR, EnemyType::NORMAL, Vector2{2940.f, 740.f}, Window, MapBG, Textures, RandomEngine
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
            EnemyType::BEAR, EnemyType::NORMAL, Vector2{2927.f, 1090.f}, Window, MapBG, Textures, RandomEngine
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
            EnemyType::BEAR, EnemyType::NORMAL, Vector2{3102.f, 899.f}, Window, MapBG, Textures, RandomEngine
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
            EnemyType::BEAR, EnemyType::NORMAL, Vector2{2835.f, 905.f}, Window, MapBG, Textures, RandomEngine
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
            EnemyType::TOAD, EnemyType::NORMAL, Vector2{1215.f, 230.f}, Window, MapBG, Textures, RandomEngine
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
            EnemyType::TOAD, EnemyType::NORMAL, Vector2{1107.f, 346.f}, Window, MapBG, Textures, RandomEngine
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
            EnemyType::TOAD, EnemyType::NORMAL, Vector2{1387.f, 356.f}, Window, MapBG, Textures, RandomEngine
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
            EnemyType::MUSHROOM, EnemyType::NORMAL, Vector2{555.f, 335.f}, Window, MapBG, Textures, RandomEngine, 2
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
            EnemyType::MUSHROOM, EnemyType::NORMAL, Vector2{628.f, 236.f}, Window, MapBG, Textures, RandomEngine, 2
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
            EnemyType::MUSHROOM, EnemyType::NORMAL, Vector2{1782.f, 226.f}, Window, MapBG, Textures, RandomEngine, 2
        };
        Enemies.emplace_back(BlueMushroomTwo);

        // ----------------------------------- Test enemies below ------------------------------------
        Enemy GreyBossBear
        {
            Sprite{Textures.BearGreyIdle, 4, 4},
            Sprite{Textures.BearGreyWalk, 4, 4},
            Sprite{Textures.BearGreyAttack, 4, 4},
            Sprite{Textures.BearGreyHurt, 1, 4},
            Sprite{Textures.BearGreyDeath, 11, 4},
            Sprite{Textures.Placeholder, 0, 0},
            EnemyType::BEAR, EnemyType::BOSS, Vector2{2163.f, 1518.f}, Window, MapBG, Textures, RandomEngine, 5, 5.f
        };
        Enemies.emplace_back(GreyBossBear);
    
        Enemy RedNecro
        {
            Sprite{Textures.NecromancerRedIdle, 4, 4},
            Sprite{Textures.NecromancerRedWalk, 4, 4},
            Sprite{Textures.NecromancerRedAttack, 4, 4},
            Sprite{Textures.NecromancerRedHurt, 1, 4},
            Sprite{Textures.NecromancerRedDeath, 11, 4},
            Sprite{Textures.NecromancerRedProjectile, 4, 4},
            EnemyType::NECROMANCER, EnemyType::NORMAL, Vector2{2038.f, 2345.f}, Window, MapBG, Textures, RandomEngine
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
            EnemyType::CREATURE, EnemyType::NORMAL, Vector2{1660.f, 3366.f}, Window, MapBG, Textures, RandomEngine
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
            EnemyType::IMP, EnemyType::NORMAL, Vector2{1800.f, 3666.f}, Window, MapBG, Textures, RandomEngine
        };
        Enemies.emplace_back(GreenImp);

        // ------------------- Wildlife NPCs ---------------------//
        // -------------------- Fox Family -----------------------//
        Enemy FoxFamOne
        {
            Sprite{Textures.FoxFamilyIdle, 5, 2},
            Sprite{Textures.FoxFamilyIdleTwo, 14, 2},
            Sprite{Textures.FoxFamilyWalk, 8, 2},
            Sprite{Textures.FoxFamilyLazy, 13, 2},
            Sprite{Textures.FoxFamilySleep, 6, 2},
            EnemyType::FOX, Vector2{473.f, 2524.f}, Window, MapBG, Textures, RandomEngine
        };
        Enemies.emplace_back(FoxFamOne);
        
        Enemy FoxFamTwo
        {
            Sprite{Textures.FoxFamilyIdle, 5, 2},
            Sprite{Textures.FoxFamilyIdleTwo, 14, 2},
            Sprite{Textures.FoxFamilyWalk, 8, 2},
            Sprite{Textures.FoxFamilyLazy, 13, 2},
            Sprite{Textures.FoxFamilySleep, 6, 2},
            EnemyType::FOX, Vector2{247.f, 2524.f}, Window, MapBG, Textures, RandomEngine
        };
        Enemies.emplace_back(FoxFamTwo);

        Enemy FoxFamThree
        {
            Sprite{Textures.FoxFamilyIdle, 5, 2},
            Sprite{Textures.FoxFamilyIdleTwo, 14, 2},
            Sprite{Textures.FoxFamilyWalk, 8, 2},
            Sprite{Textures.FoxFamilyLazy, 13, 2},
            Sprite{Textures.FoxFamilySleep, 6, 2},
            EnemyType::FOX, Vector2{428.f, 2840.f}, Window, MapBG, Textures, RandomEngine, 2.5f
        };
        Enemies.emplace_back(FoxFamThree);

        Enemy FoxFamFour
        {
            Sprite{Textures.FoxFamilyIdle, 5, 2},
            Sprite{Textures.FoxFamilyIdleTwo, 14, 2},
            Sprite{Textures.FoxFamilyWalk, 8, 2},
            Sprite{Textures.FoxFamilyLazy, 13, 2},
            Sprite{Textures.FoxFamilySleep, 6, 2},
            EnemyType::FOX, Vector2{354.f, 2661.f}, Window, MapBG, Textures, RandomEngine, 2.5f
        };
        Enemies.emplace_back(FoxFamFour);

        // ------------------- Squirrels ---------------------
        Enemy SquirrelOne
        {
            Sprite{Textures.SquirrelIdle, 6, 2},
            Sprite{Textures.SquirrelIdleTwo, 6, 2},
            Sprite{Textures.SquirrelWalk, 4, 2},
            Sprite{Textures.SquirrelEat, 2, 2},
            Sprite{Textures.SquirrelEat, 2, 2},
            EnemyType::SQUIRREL, Vector2{2839.f, 2041.f}, Window, MapBG, Textures, RandomEngine, 2.0f
        };
        Enemies.emplace_back(SquirrelOne);

        Enemy SquirrelTwo
        {
            Sprite{Textures.SquirrelIdle, 6, 2},
            Sprite{Textures.SquirrelIdleTwo, 6, 2},
            Sprite{Textures.SquirrelWalk, 4, 2},
            Sprite{Textures.SquirrelEat, 2, 2},
            Sprite{Textures.SquirrelEat, 2, 2},
            EnemyType::SQUIRREL, Vector2{3050.f, 2147.f}, Window, MapBG, Textures, RandomEngine, 2.0f
        };
        Enemies.emplace_back(SquirrelTwo);

        Enemy SquirrelThree
        {
            Sprite{Textures.SquirrelIdle, 6, 2},
            Sprite{Textures.SquirrelIdleTwo, 6, 2},
            Sprite{Textures.SquirrelWalk, 4, 2},
            Sprite{Textures.SquirrelEat, 2, 2},
            Sprite{Textures.SquirrelEat, 2, 2},
            EnemyType::SQUIRREL, Vector2{3155.f, 2133.f}, Window, MapBG, Textures, RandomEngine, 2.0f
        };
        Enemies.emplace_back(SquirrelThree);

        Enemy SquirrelFour
        {
            Sprite{Textures.SquirrelIdle, 6, 2},
            Sprite{Textures.SquirrelIdleTwo, 6, 2},
            Sprite{Textures.SquirrelWalk, 4, 2},
            Sprite{Textures.SquirrelEat, 2, 2},
            Sprite{Textures.SquirrelEat, 2, 2},
            EnemyType::SQUIRREL, Vector2{2392.f, 244.f}, Window, MapBG, Textures, RandomEngine, 2.0f
        };
        Enemies.emplace_back(SquirrelFour);

        Enemy SquirrelFive
        {
            Sprite{Textures.SquirrelIdle, 6, 2},
            Sprite{Textures.SquirrelIdleTwo, 6, 2},
            Sprite{Textures.SquirrelWalk, 4, 2},
            Sprite{Textures.SquirrelEat, 2, 2},
            Sprite{Textures.SquirrelEat, 2, 2},
            EnemyType::SQUIRREL, Vector2{3251.f, 339.f}, Window, MapBG, Textures, RandomEngine, 2.0f
        };
        Enemies.emplace_back(SquirrelFive);

        Enemy SquirrelSix
        {
            Sprite{Textures.SquirrelIdle, 6, 2},
            Sprite{Textures.SquirrelIdleTwo, 6, 2},
            Sprite{Textures.SquirrelWalk, 4, 2},
            Sprite{Textures.SquirrelEat, 2, 2},
            Sprite{Textures.SquirrelEat, 2, 2},
            EnemyType::SQUIRREL, Vector2{2767.f, 338.f}, Window, MapBG, Textures, RandomEngine, 2.0f
        };
        Enemies.emplace_back(SquirrelSix);

        Enemy SquirrelSeven
        {
            Sprite{Textures.SquirrelIdle, 6, 2},
            Sprite{Textures.SquirrelIdleTwo, 6, 2},
            Sprite{Textures.SquirrelWalk, 4, 2},
            Sprite{Textures.SquirrelEat, 2, 2},
            Sprite{Textures.SquirrelEat, 2, 2},
            EnemyType::SQUIRREL, Vector2{916.f, 940.f}, Window, MapBG, Textures, RandomEngine, 2.0f
        };
        Enemies.emplace_back(SquirrelSeven);

        Enemy SquirrelEight
        {
            Sprite{Textures.SquirrelIdle, 6, 2},
            Sprite{Textures.SquirrelIdleTwo, 6, 2},
            Sprite{Textures.SquirrelWalk, 4, 2},
            Sprite{Textures.SquirrelEat, 2, 2},
            Sprite{Textures.SquirrelEat, 2, 2},
            EnemyType::SQUIRREL, Vector2{539.f, 1221.f}, Window, MapBG, Textures, RandomEngine, 2.0f
        };
        Enemies.emplace_back(SquirrelEight);

        Enemy SquirrelNine
        {
            Sprite{Textures.SquirrelIdle, 6, 2},
            Sprite{Textures.SquirrelIdleTwo, 6, 2},
            Sprite{Textures.SquirrelWalk, 4, 2},
            Sprite{Textures.SquirrelEat, 2, 2},
            Sprite{Textures.SquirrelEat, 2, 2},
            EnemyType::SQUIRREL, Vector2{932.f, 1265.f}, Window, MapBG, Textures, RandomEngine, 2.0f
        };
        Enemies.emplace_back(SquirrelNine);

        Enemy SquirrelTen
        {
            Sprite{Textures.SquirrelIdle, 6, 2},
            Sprite{Textures.SquirrelIdleTwo, 6, 2},
            Sprite{Textures.SquirrelWalk, 4, 2},
            Sprite{Textures.SquirrelEat, 2, 2},
            Sprite{Textures.SquirrelEat, 2, 2},
            EnemyType::SQUIRREL, Vector2{345.f, 1663.f}, Window, MapBG, Textures, RandomEngine, 2.0f
        };
        Enemies.emplace_back(SquirrelTen);

        Enemy SquirrelEleven
        {
            Sprite{Textures.SquirrelIdle, 6, 2},
            Sprite{Textures.SquirrelIdleTwo, 6, 2},
            Sprite{Textures.SquirrelWalk, 4, 2},
            Sprite{Textures.SquirrelEat, 2, 2},
            Sprite{Textures.SquirrelEat, 2, 2},
            EnemyType::SQUIRREL, Vector2{254.f, 532.f}, Window, MapBG, Textures, RandomEngine, 2.0f
        };
        Enemies.emplace_back(SquirrelEleven);

        Enemy SquirrelTwelve
        {
            Sprite{Textures.SquirrelIdle, 6, 2},
            Sprite{Textures.SquirrelIdleTwo, 6, 2},
            Sprite{Textures.SquirrelWalk, 4, 2},
            Sprite{Textures.SquirrelEat, 2, 2},
            Sprite{Textures.SquirrelEat, 2, 2},
            EnemyType::SQUIRREL, Vector2{334.f, 432.f}, Window, MapBG, Textures, RandomEngine, 2.0f
        };
        Enemies.emplace_back(SquirrelTwelve);

        Enemy SquirrelThirteen
        {
            Sprite{Textures.SquirrelIdle, 6, 2},
            Sprite{Textures.SquirrelIdleTwo, 6, 2},
            Sprite{Textures.SquirrelWalk, 4, 2},
            Sprite{Textures.SquirrelEat, 2, 2},
            Sprite{Textures.SquirrelEat, 2, 2},
            EnemyType::SQUIRREL, Vector2{452.f, 533.f}, Window, MapBG, Textures, RandomEngine, 2.0f
        };
        Enemies.emplace_back(SquirrelThirteen);

        Enemy SquirrelFourteen
        {
            Sprite{Textures.SquirrelIdle, 6, 2},
            Sprite{Textures.SquirrelIdleTwo, 6, 2},
            Sprite{Textures.SquirrelWalk, 4, 2},
            Sprite{Textures.SquirrelEat, 2, 2},
            Sprite{Textures.SquirrelEat, 2, 2},
            EnemyType::SQUIRREL, Vector2{1588.f, 1780.f}, Window, MapBG, Textures, RandomEngine, 2.0f
        };
        Enemies.emplace_back(SquirrelFourteen);

        Enemy SquirrelFifteen
        {
            Sprite{Textures.SquirrelIdle, 6, 2},
            Sprite{Textures.SquirrelIdleTwo, 6, 2},
            Sprite{Textures.SquirrelWalk, 4, 2},
            Sprite{Textures.SquirrelEat, 2, 2},
            Sprite{Textures.SquirrelEat, 2, 2},
            EnemyType::SQUIRREL, Vector2{1739.f, 2002.f}, Window, MapBG, Textures, RandomEngine, 2.0f
        };
        Enemies.emplace_back(SquirrelFifteen);

        Enemy SquirrelSixteen
        {
            Sprite{Textures.SquirrelIdle, 6, 2},
            Sprite{Textures.SquirrelIdleTwo, 6, 2},
            Sprite{Textures.SquirrelWalk, 4, 2},
            Sprite{Textures.SquirrelEat, 2, 2},
            Sprite{Textures.SquirrelEat, 2, 2},
            EnemyType::SQUIRREL, Vector2{2534.f, 2520.f}, Window, MapBG, Textures, RandomEngine, 2.0f
        };
        Enemies.emplace_back(SquirrelSixteen);

        Enemy SquirrelSeventeen
        {
            Sprite{Textures.SquirrelIdle, 6, 2},
            Sprite{Textures.SquirrelIdleTwo, 6, 2},
            Sprite{Textures.SquirrelWalk, 4, 2},
            Sprite{Textures.SquirrelEat, 2, 2},
            Sprite{Textures.SquirrelEat, 2, 2},
            EnemyType::SQUIRREL, Vector2{2086.f, 2928.f}, Window, MapBG, Textures, RandomEngine, 2.0f
        };
        Enemies.emplace_back(SquirrelSeventeen);

        Enemy SquirrelEighteen
        {
            Sprite{Textures.SquirrelIdle, 6, 2},
            Sprite{Textures.SquirrelIdleTwo, 6, 2},
            Sprite{Textures.SquirrelWalk, 4, 2},
            Sprite{Textures.SquirrelEat, 2, 2},
            Sprite{Textures.SquirrelEat, 2, 2},
            EnemyType::SQUIRREL, Vector2{3964.f, 2537.f}, Window, MapBG, Textures, RandomEngine, 2.0f
        };
        Enemies.emplace_back(SquirrelEighteen);

        Enemy SquirrelNineTeen
        {
            Sprite{Textures.SquirrelIdle, 6, 2},
            Sprite{Textures.SquirrelIdleTwo, 6, 2},
            Sprite{Textures.SquirrelWalk, 4, 2},
            Sprite{Textures.SquirrelEat, 2, 2},
            Sprite{Textures.SquirrelEat, 2, 2},
            EnemyType::SQUIRREL, Vector2{3658.f, 3859.f}, Window, MapBG, Textures, RandomEngine, 2.0f
        };
        Enemies.emplace_back(SquirrelNineTeen);

        Enemy SquirrelTwenty
        {
            Sprite{Textures.SquirrelIdle, 6, 2},
            Sprite{Textures.SquirrelIdleTwo, 6, 2},
            Sprite{Textures.SquirrelWalk, 4, 2},
            Sprite{Textures.SquirrelEat, 2, 2},
            Sprite{Textures.SquirrelEat, 2, 2},
            EnemyType::SQUIRREL, Vector2{480.f, 3813.f}, Window, MapBG, Textures, RandomEngine, 2.0f
        };
        Enemies.emplace_back(SquirrelTwenty);

        Enemy SquirrelTwentyOne
        {
            Sprite{Textures.SquirrelIdle, 6, 2},
            Sprite{Textures.SquirrelIdleTwo, 6, 2},
            Sprite{Textures.SquirrelWalk, 4, 2},
            Sprite{Textures.SquirrelEat, 2, 2},
            Sprite{Textures.SquirrelEat, 2, 2},
            EnemyType::SQUIRREL, Vector2{661.f, 3847.f}, Window, MapBG, Textures, RandomEngine, 2.0f
        };
        Enemies.emplace_back(SquirrelTwentyOne);

        Enemy SquirrelTwentyTwo
        {
            Sprite{Textures.SquirrelIdle, 6, 2},
            Sprite{Textures.SquirrelIdleTwo, 6, 2},
            Sprite{Textures.SquirrelWalk, 4, 2},
            Sprite{Textures.SquirrelEat, 2, 2},
            Sprite{Textures.SquirrelEat, 2, 2},
            EnemyType::SQUIRREL, Vector2{1354.f, 3846.f}, Window, MapBG, Textures, RandomEngine, 2.0f
        };
        Enemies.emplace_back(SquirrelTwentyTwo);

        Enemy SquirrelTwentyThree
        {
            Sprite{Textures.SquirrelIdle, 6, 2},
            Sprite{Textures.SquirrelIdleTwo, 6, 2},
            Sprite{Textures.SquirrelWalk, 4, 2},
            Sprite{Textures.SquirrelEat, 2, 2},
            Sprite{Textures.SquirrelEat, 2, 2},
            EnemyType::SQUIRREL, Vector2{155.f, 1929.f}, Window, MapBG, Textures, RandomEngine, 2.0f
        };
        Enemies.emplace_back(SquirrelTwentyThree);

        Enemy SquirrelTwentyFour
        {
            Sprite{Textures.SquirrelIdle, 6, 2},
            Sprite{Textures.SquirrelIdleTwo, 6, 2},
            Sprite{Textures.SquirrelWalk, 4, 2},
            Sprite{Textures.SquirrelEat, 2, 2},
            Sprite{Textures.SquirrelEat, 2, 2},
            EnemyType::SQUIRREL, Vector2{337.f, 2032.f}, Window, MapBG, Textures, RandomEngine, 2.0f
        };
        Enemies.emplace_back(SquirrelTwentyFour);

        Enemy SquirrelTwentyFive
        {
            Sprite{Textures.SquirrelIdle, 6, 2},
            Sprite{Textures.SquirrelIdleTwo, 6, 2},
            Sprite{Textures.SquirrelWalk, 4, 2},
            Sprite{Textures.SquirrelEat, 2, 2},
            Sprite{Textures.SquirrelEat, 2, 2},
            EnemyType::SQUIRREL, Vector2{162.f, 2127.f}, Window, MapBG, Textures, RandomEngine, 2.0f
        };
        Enemies.emplace_back(SquirrelTwentyFive);

        Enemy SquirrelTwentySix
        {
            Sprite{Textures.SquirrelIdle, 6, 2},
            Sprite{Textures.SquirrelIdleTwo, 6, 2},
            Sprite{Textures.SquirrelWalk, 4, 2},
            Sprite{Textures.SquirrelEat, 2, 2},
            Sprite{Textures.SquirrelEat, 2, 2},
            EnemyType::SQUIRREL, Vector2{752.f, 1933.f}, Window, MapBG, Textures, RandomEngine, 2.0f
        };
        Enemies.emplace_back(SquirrelTwentySix);

        Enemy SquirrelTwentySeven
        {
            Sprite{Textures.SquirrelIdle, 6, 2},
            Sprite{Textures.SquirrelIdleTwo, 6, 2},
            Sprite{Textures.SquirrelWalk, 4, 2},
            Sprite{Textures.SquirrelEat, 2, 2},
            Sprite{Textures.SquirrelEat, 2, 2},
            EnemyType::SQUIRREL, Vector2{934.f, 2033.f}, Window, MapBG, Textures, RandomEngine, 2.0f
        };
        Enemies.emplace_back(SquirrelTwentySeven);

        Enemy SquirrelTwentyEight
        {
            Sprite{Textures.SquirrelIdle, 6, 2},
            Sprite{Textures.SquirrelIdleTwo, 6, 2},
            Sprite{Textures.SquirrelWalk, 4, 2},
            Sprite{Textures.SquirrelEat, 2, 2},
            Sprite{Textures.SquirrelEat, 2, 2},
            EnemyType::SQUIRREL, Vector2{333.f, 1631.f}, Window, MapBG, Textures, RandomEngine, 2.0f
        };
        Enemies.emplace_back(SquirrelTwentyEight);

        Enemy SquirrelTwentyNine
        {
            Sprite{Textures.SquirrelIdle, 6, 2},
            Sprite{Textures.SquirrelIdleTwo, 6, 2},
            Sprite{Textures.SquirrelWalk, 4, 2},
            Sprite{Textures.SquirrelEat, 2, 2},
            Sprite{Textures.SquirrelEat, 2, 2},
            EnemyType::SQUIRREL, Vector2{838.f, 1630.f}, Window, MapBG, Textures, RandomEngine, 2.0f
        };
        Enemies.emplace_back(SquirrelTwentyNine);

        Enemy SquirrelThirty
        {
            Sprite{Textures.SquirrelIdle, 6, 2},
            Sprite{Textures.SquirrelIdleTwo, 6, 2},
            Sprite{Textures.SquirrelWalk, 4, 2},
            Sprite{Textures.SquirrelEat, 2, 2},
            Sprite{Textures.SquirrelEat, 2, 2},
            EnemyType::SQUIRREL, Vector2{1107.f, 1525.f}, Window, MapBG, Textures, RandomEngine, 2.0f
        };
        Enemies.emplace_back(SquirrelThirty);

        Enemy SquirrelThirtyOne
        {
            Sprite{Textures.SquirrelIdle, 6, 2},
            Sprite{Textures.SquirrelIdleTwo, 6, 2},
            Sprite{Textures.SquirrelWalk, 4, 2},
            Sprite{Textures.SquirrelEat, 2, 2},
            Sprite{Textures.SquirrelEat, 2, 2},
            EnemyType::SQUIRREL, Vector2{1061.f, 1728.f}, Window, MapBG, Textures, RandomEngine, 2.0f
        };
        Enemies.emplace_back(SquirrelThirtyOne);

        Enemy SquirrelThirtyTwo
        {
            Sprite{Textures.SquirrelIdle, 6, 2},
            Sprite{Textures.SquirrelIdleTwo, 6, 2},
            Sprite{Textures.SquirrelWalk, 4, 2},
            Sprite{Textures.SquirrelEat, 2, 2},
            Sprite{Textures.SquirrelEat, 2, 2},
            EnemyType::SQUIRREL, Vector2{728.f, 912.f}, Window, MapBG, Textures, RandomEngine, 2.0f
        };
        Enemies.emplace_back(SquirrelThirtyTwo);

        Enemy SquirrelThirtyThree
        {
            Sprite{Textures.SquirrelIdle, 6, 2},
            Sprite{Textures.SquirrelIdleTwo, 6, 2},
            Sprite{Textures.SquirrelWalk, 4, 2},
            Sprite{Textures.SquirrelEat, 2, 2},
            Sprite{Textures.SquirrelEat, 2, 2},
            EnemyType::SQUIRREL, Vector2{1145.f, 772.f}, Window, MapBG, Textures, RandomEngine, 2.0f
        };
        Enemies.emplace_back(SquirrelThirtyThree);

        return Enemies;
    }

    std::vector<Enemy> InitializeCrows(Background& MapBG, const Window& Window, const GameTexture& Textures, Randomizer& RandomEngine)
    {
        std::vector<Enemy> Crows{};

        Enemy CrowOne
        {
            Sprite{Textures.CrowIdle, 8, 2},
            Sprite{Textures.CrowIdleTwo, 5, 2},
            Sprite{Textures.CrowFlying, 5, 2},
            Sprite{Textures.CrowWalk, 4, 2},
            Sprite{Textures.CrowIdleTwo, 5, 2},
            EnemyType::CROW, Vector2{3740.f, 2149.f}, Window, MapBG, Textures, RandomEngine, 2.5f
        };
        Crows.emplace_back(CrowOne);

        Enemy CrowTwo
        {
            Sprite{Textures.CrowIdle, 8, 2},
            Sprite{Textures.CrowIdleTwo, 5, 2},
            Sprite{Textures.CrowFlying, 5, 2},
            Sprite{Textures.CrowWalk, 4, 2},
            Sprite{Textures.CrowIdleTwo, 5, 2},
            EnemyType::CROW, Vector2{3546.f, 2032.f}, Window, MapBG, Textures, RandomEngine, 2.5f
        };
        Crows.emplace_back(CrowTwo);

        Enemy CrowThree
        {
            Sprite{Textures.CrowIdle, 8, 2},
            Sprite{Textures.CrowIdleTwo, 5, 2},
            Sprite{Textures.CrowFlying, 5, 2},
            Sprite{Textures.CrowWalk, 4, 2},
            Sprite{Textures.CrowIdleTwo, 5, 2},
            EnemyType::CROW, Vector2{3339.f, 2132.f}, Window, MapBG, Textures, RandomEngine, 2.5f
        };
        Crows.emplace_back(CrowThree);

        Enemy CrowFour
        {
            Sprite{Textures.CrowIdle, 8, 2},
            Sprite{Textures.CrowIdleTwo, 5, 2},
            Sprite{Textures.CrowFlying, 5, 2},
            Sprite{Textures.CrowWalk, 4, 2},
            Sprite{Textures.CrowIdleTwo, 5, 2},
            EnemyType::CROW, Vector2{3848.f, 1936.f}, Window, MapBG, Textures, RandomEngine, 2.5f
        };
        Crows.emplace_back(CrowFour);
        
        Enemy CrowFive
        {
            Sprite{Textures.CrowIdle, 8, 2},
            Sprite{Textures.CrowIdleTwo, 5, 2},
            Sprite{Textures.CrowFlying, 5, 2},
            Sprite{Textures.CrowWalk, 4, 2},
            Sprite{Textures.CrowIdleTwo, 5, 2},
            EnemyType::CROW, Vector2{3947.f, 1842.f}, Window, MapBG, Textures, RandomEngine, 2.5f
        };
        Crows.emplace_back(CrowFive);
        
        Enemy CrowSix
        {
            Sprite{Textures.CrowIdle, 8, 2},
            Sprite{Textures.CrowIdleTwo, 5, 2},
            Sprite{Textures.CrowFlying, 5, 2},
            Sprite{Textures.CrowWalk, 4, 2},
            Sprite{Textures.CrowIdleTwo, 5, 2},
            EnemyType::CROW, Vector2{3338.f, 1931.f}, Window, MapBG, Textures, RandomEngine, 2.5f
        };
        Crows.emplace_back(CrowSix);

        Enemy CrowSeven
        {
            Sprite{Textures.CrowIdle, 8, 2},
            Sprite{Textures.CrowIdleTwo, 5, 2},
            Sprite{Textures.CrowFlying, 5, 2},
            Sprite{Textures.CrowWalk, 4, 2},
            Sprite{Textures.CrowIdleTwo, 5, 2},
            EnemyType::CROW, Vector2{3458.f, 1729.f}, Window, MapBG, Textures, RandomEngine, 2.5f
        };
        Crows.emplace_back(CrowSeven);

        Enemy CrowEight
        {
            Sprite{Textures.CrowIdle, 8, 2},
            Sprite{Textures.CrowIdleTwo, 5, 2},
            Sprite{Textures.CrowFlying, 5, 2},
            Sprite{Textures.CrowWalk, 4, 2},
            Sprite{Textures.CrowIdleTwo, 5, 2},
            EnemyType::CROW, Vector2{3750.f, 1607.f}, Window, MapBG, Textures, RandomEngine, 2.5f
        };
        Crows.emplace_back(CrowEight);

        Enemy CrowNine
        {
            Sprite{Textures.CrowIdle, 8, 2},
            Sprite{Textures.CrowIdleTwo, 5, 2},
            Sprite{Textures.CrowFlying, 5, 2},
            Sprite{Textures.CrowWalk, 4, 2},
            Sprite{Textures.CrowIdleTwo, 5, 2},
            EnemyType::CROW, Vector2{3322.f, 1539.f}, Window, MapBG, Textures, RandomEngine, 2.5f
        };
        Crows.emplace_back(CrowNine);

        Enemy CrowTen
        {
            Sprite{Textures.CrowIdle, 8, 2},
            Sprite{Textures.CrowIdleTwo, 5, 2},
            Sprite{Textures.CrowFlying, 5, 2},
            Sprite{Textures.CrowWalk, 4, 2},
            Sprite{Textures.CrowIdleTwo, 5, 2},
            EnemyType::CROW, Vector2{3763.f, 1434.f}, Window, MapBG, Textures, RandomEngine, 2.5f
        };
        Crows.emplace_back(CrowTen);
        
        Enemy CrowEleven
        {
            Sprite{Textures.CrowIdle, 8, 2},
            Sprite{Textures.CrowIdleTwo, 5, 2},
            Sprite{Textures.CrowFlying, 5, 2},
            Sprite{Textures.CrowWalk, 4, 2},
            Sprite{Textures.CrowIdleTwo, 5, 2},
            EnemyType::CROW, Vector2{3339.f, 1355.f}, Window, MapBG, Textures, RandomEngine, 2.5f
        };
        Crows.emplace_back(CrowEleven);
        
        Enemy CrowTwelve
        {
            Sprite{Textures.CrowIdle, 8, 2},
            Sprite{Textures.CrowIdleTwo, 5, 2},
            Sprite{Textures.CrowFlying, 5, 2},
            Sprite{Textures.CrowWalk, 4, 2},
            Sprite{Textures.CrowIdleTwo, 5, 2},
            EnemyType::CROW, Vector2{3860.f, 1249.f}, Window, MapBG, Textures, RandomEngine, 2.5f
        };
        Crows.emplace_back(CrowTwelve);
        
        Enemy CrowThirteen
        {
            Sprite{Textures.CrowIdle, 8, 2},
            Sprite{Textures.CrowIdleTwo, 5, 2},
            Sprite{Textures.CrowFlying, 5, 2},
            Sprite{Textures.CrowWalk, 4, 2},
            Sprite{Textures.CrowIdleTwo, 5, 2},
            EnemyType::CROW, Vector2{3832.f, 939.f}, Window, MapBG, Textures, RandomEngine, 2.5f
        };
        Crows.emplace_back(CrowThirteen);
        
        Enemy CrowFourteen
        {
            Sprite{Textures.CrowIdle, 8, 2},
            Sprite{Textures.CrowIdleTwo, 5, 2},
            Sprite{Textures.CrowFlying, 5, 2},
            Sprite{Textures.CrowWalk, 4, 2},
            Sprite{Textures.CrowIdleTwo, 5, 2},
            EnemyType::CROW, Vector2{3742.f, 747.f}, Window, MapBG, Textures, RandomEngine, 2.5f
        };
        Crows.emplace_back(CrowFourteen);
        
        Enemy CrowFifteen
        {
            Sprite{Textures.CrowIdle, 8, 2},
            Sprite{Textures.CrowIdleTwo, 5, 2},
            Sprite{Textures.CrowFlying, 5, 2},
            Sprite{Textures.CrowWalk, 4, 2},
            Sprite{Textures.CrowIdleTwo, 5, 2},
            EnemyType::CROW, Vector2{3915.f, 747.f}, Window, MapBG, Textures, RandomEngine, 2.5f
        };
        Crows.emplace_back(CrowFifteen);
        
        Enemy CrowSixteen
        {
            Sprite{Textures.CrowIdle, 8, 2},
            Sprite{Textures.CrowIdleTwo, 5, 2},
            Sprite{Textures.CrowFlying, 5, 2},
            Sprite{Textures.CrowWalk, 4, 2},
            Sprite{Textures.CrowIdleTwo, 5, 2},
            EnemyType::CROW, Vector2{3763.f, 635.f}, Window, MapBG, Textures, RandomEngine, 2.5f
        };
        Crows.emplace_back(CrowSixteen);
        
        Enemy CrowSeventeen
        {
            Sprite{Textures.CrowIdle, 8, 2},
            Sprite{Textures.CrowIdleTwo, 5, 2},
            Sprite{Textures.CrowFlying, 5, 2},
            Sprite{Textures.CrowWalk, 4, 2},
            Sprite{Textures.CrowIdleTwo, 5, 2},
            EnemyType::CROW, Vector2{3850.f, 544.f}, Window, MapBG, Textures, RandomEngine, 2.5f
        };
        Crows.emplace_back(CrowSeventeen);
        
        Enemy CrowEighteen
        {
            Sprite{Textures.CrowIdle, 8, 2},
            Sprite{Textures.CrowIdleTwo, 5, 2},
            Sprite{Textures.CrowFlying, 5, 2},
            Sprite{Textures.CrowWalk, 4, 2},
            Sprite{Textures.CrowIdleTwo, 5, 2},
            EnemyType::CROW, Vector2{3751.f, 441.f}, Window, MapBG, Textures, RandomEngine, 2.5f
        };
        Crows.emplace_back(CrowEighteen);
        
        Enemy CrowNineteen
        {
            Sprite{Textures.CrowIdle, 8, 2},
            Sprite{Textures.CrowIdleTwo, 5, 2},
            Sprite{Textures.CrowFlying, 5, 2},
            Sprite{Textures.CrowWalk, 4, 2},
            Sprite{Textures.CrowIdleTwo, 5, 2},
            EnemyType::CROW, Vector2{3928.f, 361.f}, Window, MapBG, Textures, RandomEngine, 2.5f
        };
        Crows.emplace_back(CrowNineteen);
        
        Enemy CrowTwenty
        {
            Sprite{Textures.CrowIdle, 8, 2},
            Sprite{Textures.CrowIdleTwo, 5, 2},
            Sprite{Textures.CrowFlying, 5, 2},
            Sprite{Textures.CrowWalk, 4, 2},
            Sprite{Textures.CrowIdleTwo, 5, 2},
            EnemyType::CROW, Vector2{4040.f, 551.f}, Window, MapBG, Textures, RandomEngine, 2.5f
        };
        Crows.emplace_back(CrowTwenty);
        
        Enemy CrowTwentyone
        {
            Sprite{Textures.CrowIdle, 8, 2},
            Sprite{Textures.CrowIdleTwo, 5, 2},
            Sprite{Textures.CrowFlying, 5, 2},
            Sprite{Textures.CrowWalk, 4, 2},
            Sprite{Textures.CrowIdleTwo, 5, 2},
            EnemyType::CROW, Vector2{3459.f, 1170.f}, Window, MapBG, Textures, RandomEngine, 2.5f
        };
        Crows.emplace_back(CrowTwentyone);
        
        Enemy CrowTwentytwo
        {
            Sprite{Textures.CrowIdle, 8, 2},
            Sprite{Textures.CrowIdleTwo, 5, 2},
            Sprite{Textures.CrowFlying, 5, 2},
            Sprite{Textures.CrowWalk, 4, 2},
            Sprite{Textures.CrowIdleTwo, 5, 2},
            EnemyType::CROW, Vector2{3463.f, 952.f}, Window, MapBG, Textures, RandomEngine, 2.5f
        };
        Crows.emplace_back(CrowTwentytwo);
        
        Enemy CrowTwentythree
        {
            Sprite{Textures.CrowIdle, 8, 2},
            Sprite{Textures.CrowIdleTwo, 5, 2},
            Sprite{Textures.CrowFlying, 5, 2},
            Sprite{Textures.CrowWalk, 4, 2},
            Sprite{Textures.CrowIdleTwo, 5, 2},
            EnemyType::CROW, Vector2{3340.f, 648.f}, Window, MapBG, Textures, RandomEngine, 2.5f
        };
        Crows.emplace_back(CrowTwentythree);
        
        Enemy CrowTwentyfour
        {
            Sprite{Textures.CrowIdle, 8, 2},
            Sprite{Textures.CrowIdleTwo, 5, 2},
            Sprite{Textures.CrowFlying, 5, 2},
            Sprite{Textures.CrowWalk, 4, 2},
            Sprite{Textures.CrowIdleTwo, 5, 2},
            EnemyType::CROW, Vector2{3450.f, 470.f}, Window, MapBG, Textures, RandomEngine, 2.5f
        };
        Crows.emplace_back(CrowTwentyfour);
        
        Enemy CrowTwentyfive
        {
            Sprite{Textures.CrowIdle, 8, 2},
            Sprite{Textures.CrowIdleTwo, 5, 2},
            Sprite{Textures.CrowFlying, 5, 2},
            Sprite{Textures.CrowWalk, 4, 2},
            Sprite{Textures.CrowIdleTwo, 5, 2},
            EnemyType::CROW, Vector2{3357.f, 371.f}, Window, MapBG, Textures, RandomEngine, 2.5f
        };
        Crows.emplace_back(CrowTwentyfive);
        
        Enemy CrowTwentysix
        {
            Sprite{Textures.CrowIdle, 8, 2},
            Sprite{Textures.CrowIdleTwo, 5, 2},
            Sprite{Textures.CrowFlying, 5, 2},
            Sprite{Textures.CrowWalk, 4, 2},
            Sprite{Textures.CrowIdleTwo, 5, 2},
            EnemyType::CROW, Vector2{3260.f, 1739.f}, Window, MapBG, Textures, RandomEngine, 2.5f
        };
        Crows.emplace_back(CrowTwentysix);
        
        Enemy CrowTwentyseven
        {
            Sprite{Textures.CrowIdle, 8, 2},
            Sprite{Textures.CrowIdleTwo, 5, 2},
            Sprite{Textures.CrowFlying, 5, 2},
            Sprite{Textures.CrowWalk, 4, 2},
            Sprite{Textures.CrowIdleTwo, 5, 2},
            EnemyType::CROW, Vector2{3039.f, 1835.f}, Window, MapBG, Textures, RandomEngine, 2.5f
        };
        Crows.emplace_back(CrowTwentyseven);

        Enemy CrowTwentyeight
        {
            Sprite{Textures.CrowIdle, 8, 2},
            Sprite{Textures.CrowIdleTwo, 5, 2},
            Sprite{Textures.CrowFlying, 5, 2},
            Sprite{Textures.CrowWalk, 4, 2},
            Sprite{Textures.CrowIdleTwo, 5, 2},
            EnemyType::CROW, Vector2{2838.f, 1553.f}, Window, MapBG, Textures, RandomEngine, 2.5f
        };
        Crows.emplace_back(CrowTwentyeight);

        Enemy CrowTwentynine
        {
            Sprite{Textures.CrowIdle, 8, 2},
            Sprite{Textures.CrowIdleTwo, 5, 2},
            Sprite{Textures.CrowFlying, 5, 2},
            Sprite{Textures.CrowWalk, 4, 2},
            Sprite{Textures.CrowIdleTwo, 5, 2},
            EnemyType::CROW, Vector2{2638.f, 1440.f}, Window, MapBG, Textures, RandomEngine, 2.5f
        };
        Crows.emplace_back(CrowTwentynine);

        Enemy CrowThirty
        {
            Sprite{Textures.CrowIdle, 8, 2},
            Sprite{Textures.CrowIdleTwo, 5, 2},
            Sprite{Textures.CrowFlying, 5, 2},
            Sprite{Textures.CrowWalk, 4, 2},
            Sprite{Textures.CrowIdleTwo, 5, 2},
            EnemyType::CROW, Vector2{638.f, 1791.f}, Window, MapBG, Textures, RandomEngine, 2.5f
        };
        Crows.emplace_back(CrowThirty);

        Enemy CrowThirtyone
        {
            Sprite{Textures.CrowIdle, 8, 2},
            Sprite{Textures.CrowIdleTwo, 5, 2},
            Sprite{Textures.CrowFlying, 5, 2},
            Sprite{Textures.CrowWalk, 4, 2},
            Sprite{Textures.CrowIdleTwo, 5, 2},
            EnemyType::CROW, Vector2{549.f, 1951.f}, Window, MapBG, Textures, RandomEngine, 2.5f
        };
        Crows.emplace_back(CrowThirtyone);

        Enemy CrowThirtytwo
        {
            Sprite{Textures.CrowIdle, 8, 2},
            Sprite{Textures.CrowIdleTwo, 5, 2},
            Sprite{Textures.CrowFlying, 5, 2},
            Sprite{Textures.CrowWalk, 4, 2},
            Sprite{Textures.CrowIdleTwo, 5, 2},
            EnemyType::CROW, Vector2{1436.f, 1431.f}, Window, MapBG, Textures, RandomEngine, 2.5f
        };
        Crows.emplace_back(CrowThirtytwo);

        Enemy CrowThirtythree
        {
            Sprite{Textures.CrowIdle, 8, 2},
            Sprite{Textures.CrowIdleTwo, 5, 2},
            Sprite{Textures.CrowFlying, 5, 2},
            Sprite{Textures.CrowWalk, 4, 2},
            Sprite{Textures.CrowIdleTwo, 5, 2},
            EnemyType::CROW, Vector2{1761.f, 1146.f}, Window, MapBG, Textures, RandomEngine, 2.5f
        };
        Crows.emplace_back(CrowThirtythree);

        Enemy CrowThirtyfour
        {
            Sprite{Textures.CrowIdle, 8, 2},
            Sprite{Textures.CrowIdleTwo, 5, 2},
            Sprite{Textures.CrowFlying, 5, 2},
            Sprite{Textures.CrowWalk, 4, 2},
            Sprite{Textures.CrowIdleTwo, 5, 2},
            EnemyType::CROW, Vector2{1453.f, 960.f}, Window, MapBG, Textures, RandomEngine, 2.5f
        };
        Crows.emplace_back(CrowThirtyfour);

        Enemy CrowThirtyfive
        {
            Sprite{Textures.CrowIdle, 8, 2},
            Sprite{Textures.CrowIdleTwo, 5, 2},
            Sprite{Textures.CrowFlying, 5, 2},
            Sprite{Textures.CrowWalk, 4, 2},
            Sprite{Textures.CrowIdleTwo, 5, 2},
            EnemyType::CROW, Vector2{246.f, 774.f}, Window, MapBG, Textures, RandomEngine, 2.5f
        };
        Crows.emplace_back(CrowThirtyfive);

        Enemy CrowThirtysix
        {
            Sprite{Textures.CrowIdle, 8, 2},
            Sprite{Textures.CrowIdleTwo, 5, 2},
            Sprite{Textures.CrowFlying, 5, 2},
            Sprite{Textures.CrowWalk, 4, 2},
            Sprite{Textures.CrowIdleTwo, 5, 2},
            EnemyType::CROW, Vector2{255.f, 1186.f}, Window, MapBG, Textures, RandomEngine, 2.5f
        };
        Crows.emplace_back(CrowThirtysix);

        Enemy CrowThirtyseven
        {
            Sprite{Textures.CrowIdle, 8, 2},
            Sprite{Textures.CrowIdleTwo, 5, 2},
            Sprite{Textures.CrowFlying, 5, 2},
            Sprite{Textures.CrowWalk, 4, 2},
            Sprite{Textures.CrowIdleTwo, 5, 2},
            EnemyType::CROW, Vector2{154.f, 1563.f}, Window, MapBG, Textures, RandomEngine, 2.5f
        };
        Crows.emplace_back(CrowThirtyseven);

        Enemy CrowThirtyeight
        {
            Sprite{Textures.CrowIdle, 8, 2},
            Sprite{Textures.CrowIdleTwo, 5, 2},
            Sprite{Textures.CrowFlying, 5, 2},
            Sprite{Textures.CrowWalk, 4, 2},
            Sprite{Textures.CrowIdleTwo, 5, 2},
            EnemyType::CROW, Vector2{860.f, 363.f}, Window, MapBG, Textures, RandomEngine, 2.5f
        };
        Crows.emplace_back(CrowThirtyeight);

        Enemy CrowThirtynine
        {
            Sprite{Textures.CrowIdle, 8, 2},
            Sprite{Textures.CrowIdleTwo, 5, 2},
            Sprite{Textures.CrowFlying, 5, 2},
            Sprite{Textures.CrowWalk, 4, 2},
            Sprite{Textures.CrowIdleTwo, 5, 2},
            EnemyType::CROW, Vector2{2183.f, 1059.f}, Window, MapBG, Textures, RandomEngine, 2.5f
        };
        Crows.emplace_back(CrowThirtynine);

        Enemy CrowForty
        {
            Sprite{Textures.CrowIdle, 8, 2},
            Sprite{Textures.CrowIdleTwo, 5, 2},
            Sprite{Textures.CrowFlying, 5, 2},
            Sprite{Textures.CrowWalk, 4, 2},
            Sprite{Textures.CrowIdleTwo, 5, 2},
            EnemyType::CROW, Vector2{2761.f, 560.f}, Window, MapBG, Textures, RandomEngine, 2.5f
        };
        Crows.emplace_back(CrowForty);

        Enemy CrowFortyone
        {
            Sprite{Textures.CrowIdle, 8, 2},
            Sprite{Textures.CrowIdleTwo, 5, 2},
            Sprite{Textures.CrowFlying, 5, 2},
            Sprite{Textures.CrowWalk, 4, 2},
            Sprite{Textures.CrowIdleTwo, 5, 2},
            EnemyType::CROW, Vector2{1647.f, 563.f}, Window, MapBG, Textures, RandomEngine, 2.5f
        };
        Crows.emplace_back(CrowFortyone);

        return Crows;
    }

    // Debugging --------------------
    void DrawCollisionRecs(Prop& Prop, const Vector2 CharacterWorldPos, Color RecColor)
    {
        if (Prop.IsInteractable()) {
            DrawRectangle(
                Prop.GetInteractRec(CharacterWorldPos).x,
                Prop.GetInteractRec(CharacterWorldPos).y,
                Prop.GetInteractRec(CharacterWorldPos).width,
                Prop.GetInteractRec(CharacterWorldPos).height, 
                Color{ 200, 122, 255, 100 }        
            );
            DrawRectangle(
                Prop.GetCollisionRec(CharacterWorldPos).x,
                Prop.GetCollisionRec(CharacterWorldPos).y,
                Prop.GetCollisionRec(CharacterWorldPos).width,
                Prop.GetCollisionRec(CharacterWorldPos).height, 
                Color{ 51, 255, 255, 100 }
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
    void DrawCollisionRecs(Object& Type, Color RecColor)
    {   
        DrawRectangle(
            Type.GetCollisionRec().x,
            Type.GetCollisionRec().y,
            Type.GetCollisionRec().width,
            Type.GetCollisionRec().height, 
            RecColor
        );
    }

    template <typename Object>
    void DrawAttackRecs(Object& Type, Color RecColor)
    {
        if (Type.IsAttacking())
        {
            DrawRectangle(
                Type.GetAttackRec().x,
                Type.GetAttackRec().y,
                Type.GetAttackRec().width,
                Type.GetAttackRec().height,
                RecColor
            );
        }
    }

} // namespace Game