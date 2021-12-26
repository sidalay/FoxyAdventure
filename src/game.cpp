#include "headers/game.hpp"
#include <assert.h>
#include <vector>

void Game::Run() 
{

    // Initialization ---------------------------
    Window Window {800, 800};                               // Window Dimensions
    int FPS {144};                                          // Frames Per Second
    Game::Initialize(Window, FPS, "Cryptex Adventure");     // Create Window

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

    for (auto& Proptype:*Props.Under)
        for (auto& Prop:Proptype)
            Prop.Tick(DeltaTime, Map);

    for (auto& Proptype:*Props.Over)
        for (auto& Prop:Proptype)
            Prop.Tick(DeltaTime, Map);
}

void Game::Draw(Background& Map, Character& Character, Props& Props)
{
    Map.Draw();

    for (auto& PropType:*Props.Under)
        for (auto& Prop:PropType)
        {
            Prop.Draw(Character.GetWorldPos());
            // DrawRectangle(Prop.GetCollisionRec(Character.GetWorldPos()).x,Prop.GetCollisionRec(Character.GetWorldPos()).y,Prop.GetCollisionRec(Character.GetWorldPos()).width,Prop.GetCollisionRec(Character.GetWorldPos()).height, BLUE);
        }

    // DrawRectangle(Character.GetCollisionRec().x,Character.GetCollisionRec().y,Character.GetCollisionRec().width,Character.GetCollisionRec().height, RED);
    Character.Draw();
    
    for (auto& PropType:*Props.Over)
        for (auto& Prop:PropType)
        {
            Prop.Draw(Character.GetWorldPos());
            // DrawRectangle(Prop.GetCollisionRec(Character.GetWorldPos()).x,Prop.GetCollisionRec(Character.GetWorldPos()).y,Prop.GetCollisionRec(Character.GetWorldPos()).width,Prop.GetCollisionRec(Character.GetWorldPos()).height, BLUE);
        }
    
    DrawText(TextFormat("WorldPos.x: %i", (int)Character.GetWorldPos().x), 20, 20, 20, WHITE);
    DrawText(TextFormat("WorldPos.y: %i", (int)Character.GetWorldPos().y), 20, 40, 20, WHITE);

    Map.DrawMiniMap(Character.GetWorldPos());
}

// Initialize props drawn under character
std::vector<std::vector<Prop>> Game::InitializePropsUnder()
{
    std::vector<std::vector<Prop>> Props{};

    std::vector<Prop> Flowers
    {
        Prop{"sprites/props/Flowers.png", Vector2{200,800}, PropType::FLOWER}, 
        Prop{"sprites/props/Flowers.png", Vector2{270,800}, PropType::FLOWER},
        Prop{"sprites/props/Flowers.png", Vector2{200,868}, PropType::FLOWER}, 
        Prop{"sprites/props/Flowers.png", Vector2{290,880}, PropType::FLOWER, 2.f}
    };
    Props.emplace_back(Flowers);

    std::vector<Prop> Grass
    {
        Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{790,675}, PropType::GRASS, 4.f, true},
        Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{815,725}, PropType::GRASS, 4.f, true},

        Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{790+64,675}, PropType::GRASS, 4.f, true},
        Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{815+64,725}, PropType::GRASS, 4.f, true},

        Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{790,675+64}, PropType::GRASS, 4.f, true},
        Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{815,725+64}, PropType::GRASS, 4.f, true},

        Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{790+64,675+64}, PropType::GRASS, 4.f, true},
        Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{815+64,725+64}, PropType::GRASS, 4.f, true},
    };
    Props.emplace_back(Grass);

    return Props;
}

// Initialize props drawn over character
std::vector<std::vector<Prop>> Game::InitializePropsOver()
{
    std::vector<std::vector<Prop>> Props{};

    std::vector<Prop> Boulder
    {
        Prop{"sprites/props/Boulder.png", Vector2{1000,1000}, PropType::BOULDER}, 
        Prop{"sprites/props/Boulder.png", Vector2{1112,1000}, PropType::BOULDER},
        Prop{"sprites/props/Boulder.png", Vector2{1172,1000}, PropType::BOULDER, 4.f, true},
        Prop{"sprites/props/Boulder.png", Vector2{750,900}, PropType::BOULDER, 4.f, true},
        
        Prop{"sprites/props/Boulder.png", Vector2{960,2950}, PropType::BOULDER},
        Prop{"sprites/props/Boulder.png", Vector2{1090,2950}, PropType::BOULDER},
        Prop{"sprites/props/Boulder.png", Vector2{1025,3050}, PropType::BOULDER, 4.f, true},

    };
    Props.emplace_back(Boulder);

    std::vector<Prop> Stump
    {
        Prop{"sprites/props/TreeStump.png", Vector2{600,900}, PropType::STUMP, 4.f, true}
    };
    Props.emplace_back(Stump);

    std::vector<Prop> Trees
    {
        // row1
        Prop{"sprites/props/TreeGreen.png", Vector2{20,67}, PropType::TREE}, 
        Prop{"sprites/props/TreeGreen.png", Vector2{120,67}, PropType::TREE}, 
        Prop{"sprites/props/TreeGreen.png", Vector2{220,67}, PropType::TREE}, 
        Prop{"sprites/props/TreeGreen.png", Vector2{320,67}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{420,67}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{520,67}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{620,67}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{720,67}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{820,67}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{920,67}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1020,67}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1120,67}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1220,67}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1320,67}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1420,67}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1520,67}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1620,67}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1720,67}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1820,67}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1920,67}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2020,67}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2120,67}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2220,67}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2320,67}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2420,67}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2520,67}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2620,67}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2720,67}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2820,67}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2920,67}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3020,67}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3120,67}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3220,67}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3320,67}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3420,67}, PropType::TREE},

        Prop{"sprites/props/TreeGreen.png", Vector2{3700,67}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3800,67}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3900,67}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{4000,67}, PropType::TREE},

        // row2
        Prop{"sprites/props/TreeGreen.png", Vector2{0,167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{100,167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{200,167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{300,167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{400,167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{500,167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{600,167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{700,167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{800,167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{900,167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1000,167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1100,167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1200,167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1300,167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1400,167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1500,167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1600,167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1700,167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1800,167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1900,167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2000,167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2100,167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2200,167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2300,167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2400,167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2500,167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2600,167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2700,167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2800,167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2900,167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3000,167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3100,167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3200,167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3300,167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3400,167}, PropType::TREE},

        Prop{"sprites/props/TreeGreen.png", Vector2{3720,167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3820,167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3920,167}, PropType::TREE},

        // row3
        Prop{"sprites/props/TreeGreen.png", Vector2{20,267}, PropType::TREE}, 
        Prop{"sprites/props/TreeGreen.png", Vector2{120,267}, PropType::TREE}, 
        Prop{"sprites/props/TreeGreen.png", Vector2{220,267}, PropType::TREE}, 
        Prop{"sprites/props/TreeGreen.png", Vector2{320,267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{420,267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{520,267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{620,267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{720,267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{820,267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{920,267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1020,267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1120,267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1220,267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1320,267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1420,267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1520,267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1620,267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1720,267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1820,267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1920,267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2020,267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2120,267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2220,267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2320,267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2420,267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2520,267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2620,267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2720,267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2820,267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2920,267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3020,267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3120,267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3220,267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3320,267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3420,267}, PropType::TREE},

        Prop{"sprites/props/TreeGreen.png", Vector2{3700,267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3800,267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3900,267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{4000,267}, PropType::TREE},

        // row4
        Prop{"sprites/props/TreeGreen.png", Vector2{0,367}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{100,367}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{200,367}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{300,367}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{400,367}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{500,367}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{600,367}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{700,367}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{800,367}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{900,367}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1000,367}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1100,367}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1200,367}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1300,367}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1400,367}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1500,367}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1600,367}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1700,367}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1800,367}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1900,367}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2000,367}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2100,367}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2200,367}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2300,367}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2400,367}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2500,367}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2600,367}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2700,367}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2800,367}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2900,367}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3000,367}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3100,367}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3200,367}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3300,367}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3400,367}, PropType::TREE},

        Prop{"sprites/props/TreeGreen.png", Vector2{3720,367}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3820,367}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3920,367}, PropType::TREE},

        //row5
        Prop{"sprites/props/TreeGreen.png", Vector2{20,467}, PropType::TREE}, 
        Prop{"sprites/props/TreeGreen.png", Vector2{120,467}, PropType::TREE}, 
        Prop{"sprites/props/TreeGreen.png", Vector2{220,467}, PropType::TREE}, 
        Prop{"sprites/props/TreeGreen.png", Vector2{320,467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{420,467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{520,467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{620,467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{720,467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{820,467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{920,467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1020,467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1120,467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1220,467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1320,467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1420,467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1520,467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1620,467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1720,467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1820,467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1920,467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2020,467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2120,467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2220,467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2320,467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2420,467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2520,467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2620,467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2720,467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2820,467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2920,467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3020,467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3120,467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3220,467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3320,467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3420,467}, PropType::TREE},

        Prop{"sprites/props/TreeGreen.png", Vector2{3700,467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3800,467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3900,467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{4000,467}, PropType::TREE},

        Prop{"sprites/props/TreeGreen.png", Vector2{2050,2050}, PropType::TREE, 8.f},
    };
    Props.emplace_back(Trees);

    std::vector<Prop> Grass
    {
        Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{825,680}, PropType::GRASS, 4.f, true},
        Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{800,700}, PropType::GRASS, 4.f, true},
        Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{830,710}, PropType::GRASS, 4.f, true},
        
        Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{825+64,680}, PropType::GRASS, 4.f, true},
        Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{800+64,700}, PropType::GRASS, 4.f, true},
        Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{830+64,710}, PropType::GRASS, 4.f, true},

        Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{825,680+64}, PropType::GRASS, 4.f, true},
        Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{800,700+64}, PropType::GRASS, 4.f, true},
        Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{830,710+64}, PropType::GRASS, 4.f, true},

        Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{825+64,680+64}, PropType::GRASS, 4.f, true},
        Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{800+64,700+64}, PropType::GRASS, 4.f, true},
        Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{830+64,710+64}, PropType::GRASS, 4.f, true},
    };
    Props.emplace_back(Grass);
    
    return Props;
}

