#include "headers/game.hpp"
#include <assert.h>
#include <vector>

void Game::Run() 
{

    // Initialization ---------------------------
    Window Window {800, 800};                      // Window Dimensions
    int FPS {144};                                  // Frames Per Second
    Game::Initialize(Window, FPS, "Template");      // Create Window

    // Initialize Background
    Background MapBG{};

    // Initialize Character
    Character Champion{Sprite{"sprites/characters/Fox_idle.png", 4, 4}, 
                       Sprite{"sprites/characters/Fox_walk.png", 4, 4},
                       Sprite{"sprites/characters/Fox_run.png", 4, 4}, 
                       Sprite{"sprites/characters/Fox_melee.png", 4, 4},
                       Sprite{"sprites/characters/Fox_hit.png", 2, 4}, 
                       Sprite{"sprites/characters/Fox_die.png", 1, 4},
                       Sprite{"sprites/characters/Fox_push.png", 4, 4},
                       Sprite{"sprites/characters/Fox_sleep.png", 4, 1}, 
                       Sprite{"sprites/characters/Fox_itemGot.png", 1, 4},
                       &Window, &MapBG};

    // Initialize Props
    std::vector<std::vector<Prop>> Props{Game::InitializeProps()};

    // Start Game Loop
    while (!WindowShouldClose()) 
    {
        Game::Tick(Window, MapBG, Champion, &Props);
    }

    // Clean-up
    assert(GetWindowHandle());
    CloseWindow();
}

// Set the Game's window configurations
void Game::Initialize(Window& Window, int FPS, std::string Title)
{
    assert(!GetWindowHandle());
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(Window.x, Window.y, Title.c_str());
    SetTargetFPS(FPS);
    SetExitKey(0);
}

void Game::Tick(Window& Window, Background& Map, Character& Character, std::vector<std::vector<Prop>>* Props)
{
    // Check if Window has been resized or fullscreened
    Game::CheckScreenSizing(Window);

    // BEGIN DRAWING
    BeginDrawing();
    ClearBackground(BLACK);

    // Tick & Draw Functions
    Game::Update(Map, Character, Props);
    Game::Draw(Map, Character, Props);

    // END DRAWING
    EndDrawing();
}

void Game::CheckScreenSizing(Window& Window)
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

void Game::SetFullScreen(Window& Window)
{
    // see what display we are on right now
    int display = GetCurrentMonitor();

    if (IsWindowFullscreen()) 
    {
        // if we are full screen, then go back to the windowed size
        ToggleFullscreen();                  
        SetWindowSize(Window.x, Window.y); 
    }
    else
    {                                        
        // if we are not full screen, set the window size to match the monitor we are on
        SetWindowSize(GetMonitorWidth(display), GetMonitorHeight(display));
        ToggleFullscreen();
    }
}


void Game::Update(Background& Map, Character& Character, std::vector<std::vector<Prop>>* Props)
{
    // Create DeltaTime
    float DeltaTime{GetFrameTime()};

    // Call Ticks
    Map.Tick(Character.GetWorldPos());
    Character.Tick(DeltaTime, Props);
}

void Game::Draw(Background& Map, Character& Character, std::vector<std::vector<Prop>>* Props)
{
    Map.Draw();
    // DrawRectangle(Character.GetCollisionRec().x,Character.GetCollisionRec().y,Character.GetCollisionRec().width,Character.GetCollisionRec().height, RED);
    Character.Draw();
    
    for (auto PropType:*Props)
    {
        for (auto Prop:PropType)
        {
            Prop.Draw(Character.GetWorldPos());
            // DrawRectangle(Prop.GetCollisionRec(Character.GetWorldPos()).x,Prop.GetCollisionRec(Character.GetWorldPos()).y,Prop.GetCollisionRec(Character.GetWorldPos()).width,Prop.GetCollisionRec(Character.GetWorldPos()).height, BLUE);
        }
    }

    
    DrawText(TextFormat("WorldPos.x: %i", (int)Character.GetWorldPos().x), 20, 20, 20, WHITE);
    DrawText(TextFormat("WorldPos.y: %i", (int)Character.GetWorldPos().y), 20, 40, 20, WHITE);
}

std::vector<std::vector<Prop>> Game::InitializeProps()
{
    std::vector<std::vector<Prop>> Props{};

    std::vector<Prop> Trees
    {
        Prop{"sprites/props/PinkTree.png", Vector2{40,100}, PropType::TREE}, Prop{"sprites/props/PinkTree.png", Vector2{80,125}, PropType::TREE},
        Prop{"sprites/props/PinkTree.png", Vector2{200,80}, PropType::TREE}, Prop{"sprites/props/BlueTree.png", Vector2{320,67}, PropType::TREE},
        Prop{"sprites/props/FallTree.png", Vector2{400,400}, PropType::TREE, 8.f}
    };
    Props.emplace_back(Trees);
    

    return Props;
}

