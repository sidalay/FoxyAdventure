#include "headers/game.hpp"
#include <assert.h>
#include <vector>

void Game::Run() 
{

    // Initialization ---------------------------
    Window Window {800, 800};                       // Window Dimensions
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

    // Initialize Props & Create a Props Struct
    std::vector<std::vector<Prop>> UnderProps{Game::InitializePropsUnder()};
    std::vector<std::vector<Prop>> OverProps{Game::InitializePropsOver()};
    Props Props{&UnderProps, &OverProps};

    // Start Game Loop
    while (!WindowShouldClose()) 
    {
        Game::Tick(Window, MapBG, Champion, Props);
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

void Game::Tick(Window& Window, Background& Map, Character& Character, Props& Props)
{
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

// Check if Window has been resized or fullscreened
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

// Full screen with current resolution settings
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


void Game::Update(Background& Map, Character& Character, Props& Props)
{
    // Create DeltaTime
    float DeltaTime{GetFrameTime()};

    // Call Ticks
    Map.Tick(Character.GetWorldPos());
    Character.Tick(DeltaTime, Props);
}

void Game::Draw(Background& Map, Character& Character, Props& Props)
{
    Map.Draw();

    for (auto PropType:*Props.Under)
        for (auto Prop:PropType)
        {
            Prop.Draw(Character.GetWorldPos());
            // DrawRectangle(Prop.GetCollisionRec(Character.GetWorldPos()).x,Prop.GetCollisionRec(Character.GetWorldPos()).y,Prop.GetCollisionRec(Character.GetWorldPos()).width,Prop.GetCollisionRec(Character.GetWorldPos()).height, BLUE);
        }

    // DrawRectangle(Character.GetCollisionRec().x,Character.GetCollisionRec().y,Character.GetCollisionRec().width,Character.GetCollisionRec().height, RED);
    Character.Draw();
    
    for (auto PropType:*Props.Over)
        for (auto Prop:PropType)
        {
            Prop.Draw(Character.GetWorldPos());
            // DrawRectangle(Prop.GetCollisionRec(Character.GetWorldPos()).x,Prop.GetCollisionRec(Character.GetWorldPos()).y,Prop.GetCollisionRec(Character.GetWorldPos()).width,Prop.GetCollisionRec(Character.GetWorldPos()).height, BLUE);
        }
    
    DrawText(TextFormat("WorldPos.x: %i", (int)Character.GetWorldPos().x), 20, 20, 20, WHITE);
    DrawText(TextFormat("WorldPos.y: %i", (int)Character.GetWorldPos().y), 20, 40, 20, WHITE);
}

// Initialize props drawn under character
std::vector<std::vector<Prop>> Game::InitializePropsUnder()
{
    std::vector<std::vector<Prop>> Props{};

    std::vector<Prop> Flowers
    {
        Prop{"sprites/props/Flowers.png", Vector2{200,300}, PropType::FLOWER}, 
        Prop{"sprites/props/Flowers.png", Vector2{270,300}, PropType::FLOWER},
        Prop{"sprites/props/Flowers.png", Vector2{200,368}, PropType::FLOWER}, 
        Prop{"sprites/props/Flowers.png", Vector2{290,380}, PropType::FLOWER, 2.f}
    };
    Props.emplace_back(Flowers);

    std::vector<Prop> Grass
    {
        Prop{Sprite{"sprites/props/GrassAnimation.png", 2, 1}, Vector2{400,300}, PropType::GRASS}
    };
    Props.emplace_back(Grass);

    return Props;
}

// Initialize props drawn over character
std::vector<std::vector<Prop>> Game::InitializePropsOver()
{
    std::vector<std::vector<Prop>> Props{};

    std::vector<Prop> Trees
    {
        Prop{"sprites/props/TreePink.png", Vector2{40,100}, PropType::TREE}, 
        Prop{"sprites/props/TreePink.png", Vector2{80,125}, PropType::TREE},
        Prop{"sprites/props/TreePink.png", Vector2{200,80}, PropType::TREE}, 
        Prop{"sprites/props/TreeBlue.png", Vector2{320,67}, PropType::TREE},
        Prop{"sprites/props/TreeFall.png", Vector2{400,400}, PropType::TREE, 8.f},
    };
    Props.emplace_back(Trees);
    
    std::vector<Prop> Boulder
    {
        Prop{"sprites/props/Boulder.png", Vector2{1000,500}, PropType::BOULDER}, 
        Prop{"sprites/props/Boulder.png", Vector2{1112,500}, PropType::BOULDER},
        Prop{"sprites/props/Boulder.png", Vector2{1172,500}, PropType::BOULDER},
        Prop{"sprites/props/Boulder.png", Vector2{750,200}, PropType::BOULDER, 4.f, true}
    };
    Props.emplace_back(Boulder);

    std::vector<Prop> Stump
    {
        Prop{"sprites/props/TreeStump.png", Vector2{600,200}, PropType::STUMP, 4.f, true}
    };
    Props.emplace_back(Stump);
    
    return Props;
}

