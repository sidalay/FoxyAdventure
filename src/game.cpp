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
                       Sprite{"sprites/characters/Fox_die.png", 1, 4}, 
                       &Window, &MapBG};

    // Start Game Loop
    while (!WindowShouldClose()) 
    {
        Game::Tick(Window, MapBG, Champion);
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

void Game::Tick(Window& Window, Background& Map, Character& Character)
{
    // Check if Window has been resized or fullscreened
    Game::CheckScreenSizing(Window);

    // BEGIN DRAWING
    BeginDrawing();
    ClearBackground(BLACK);

    // Tick & Draw Functions
    Game::Update(Map, Character);
    Game::Draw(Map, Character);

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


void Game::Update(Background& Map, Character& Character)
{
    // Create DeltaTime
    float DeltaTime{GetFrameTime()};

    // Call Ticks
    Map.Tick(Character.GetWorldPos());
    Character.Tick(DeltaTime);
}

void Game::Draw(Background& Map, Character& Character)
{
    Map.Draw();
    Character.Draw();
    
    DrawText(TextFormat("WorldPos.x: %i", (int)Character.GetWorldPos().x), 20, 20, 20, WHITE);
    DrawText(TextFormat("WorldPos.y: %i", (int)Character.GetWorldPos().y), 20, 40, 20, WHITE);
}

