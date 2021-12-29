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
            // DrawRectangle(Prop.GetCollisionRec(Character.GetWorldPos()).x,Prop.GetCollisionRec(Character.GetWorldPos()).y,Prop.GetCollisionRec(Character.GetWorldPos()).width,Prop.GetCollisionRec(Character.GetWorldPos()).height, PURPLE);
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
        // Prop{"sprites/props/Flowers.png", Vector2{200,800}, PropType::FLOWER}, 
        // Prop{"sprites/props/Flowers.png", Vector2{270,800}, PropType::FLOWER},
        // Prop{"sprites/props/Flowers.png", Vector2{200,868}, PropType::FLOWER}, 
        // Prop{"sprites/props/Flowers.png", Vector2{290,880}, PropType::FLOWER, 2.f}
    };
    Props.emplace_back(Flowers);

    std::vector<Prop> Grass
    {
        // Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{790,675}, PropType::GRASS, 4.f, true},
        // Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{815,725}, PropType::GRASS, 4.f, true},

        // Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{790+64,675}, PropType::GRASS, 4.f, true},
        // Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{815+64,725}, PropType::GRASS, 4.f, true},

        // Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{790,675+64}, PropType::GRASS, 4.f, true},
        // Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{815,725+64}, PropType::GRASS, 4.f, true},

        // Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{790+64,675+64}, PropType::GRASS, 4.f, true},
        // Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{815+64,725+64}, PropType::GRASS, 4.f, true},
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
        Prop{"sprites/props/DoorRed.png", Vector2{1612,2815}, PropType::DOOR}, 
        Prop{"sprites/props/DoorBlue.png", Vector2{3120,2685}, PropType::DOOR}



    };
    Props.emplace_back(Walls);

    std::vector<Prop> Moveable
    {
        Prop{"sprites/props/Boulder.png", Vector2{1025,3050}, PropType::BOULDER, 4.f, true},
        // Prop{"sprites/props/TreeStump.png", Vector2{1025,3500}, PropType::STUMP, 4.f, true}
    };
    Props.emplace_back(Moveable);
    
    return Props;
}

// Initialize props drawn over character
std::vector<std::vector<Prop>> Game::InitializePropsOver()
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
        // Prop{"sprites/props/TreeStump.png", Vector2{1025,3500}, PropType::STUMP, 4.f, true},
        Prop{"sprites/props/Boulder.png", Vector2{1025,3050}, PropType::BOULDER, 4.f, true},
    };
    Props.emplace_back(Moveable);

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
        // Prop{"sprites/props/TreeGreen.png", Vector2{100,167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{200,167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{300,167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{400,167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{500,167}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{600,167}, PropType::TREE},
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
        // Prop{"sprites/props/TreeGreen.png", Vector2{1700,167}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{1800,167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1900,167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2000,167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2100,167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2200,167}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{2300,167}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{2400,167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2500,167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2600,167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2700,167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2800,167}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{2900,167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3000,167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3100,167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3200,167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3300,167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3400,167}, PropType::TREE},

        Prop{"sprites/props/TreeGreen.png", Vector2{3720,167}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{3820,167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3920,167}, PropType::TREE},

        // row3
        Prop{"sprites/props/TreeGreen.png", Vector2{20,267}, PropType::TREE}, 
        Prop{"sprites/props/TreeGreen.png", Vector2{120,267}, PropType::TREE}, 
        Prop{"sprites/props/TreeGreen.png", Vector2{220,267}, PropType::TREE}, 
        Prop{"sprites/props/TreeGreen.png", Vector2{320,267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{420,267}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{520,267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{620,267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{720,267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{820,267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{920,267}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{1020,267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1120,267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1220,267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1320,267}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{1420,267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1520,267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1620,267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1720,267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1820,267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1920,267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2020,267}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{2120,267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2220,267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2320,267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2420,267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2520,267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2620,267}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{2720,267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2820,267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2920,267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3020,267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3120,267}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{3220,267}, PropType::TREE},
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
        // Prop{"sprites/props/TreeGreen.png", Vector2{300,367}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{400,367}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{500,367}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{600,367}, PropType::TREE},
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
        // Prop{"sprites/props/TreeGreen.png", Vector2{1700,367}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1800,367}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1900,367}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2000,367}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2100,367}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2200,367}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2300,367}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{2400,367}, PropType::TREE},
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

        // Prop{"sprites/props/TreeGreen.png", Vector2{3720,367}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3820,367}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3920,367}, PropType::TREE},

        //row5
        Prop{"sprites/props/TreeGreen.png", Vector2{20,467}, PropType::TREE}, 
        Prop{"sprites/props/TreeGreen.png", Vector2{120,467}, PropType::TREE}, 
        // Prop{"sprites/props/TreeGreen.png", Vector2{220,467}, PropType::TREE}, 
        Prop{"sprites/props/TreeGreen.png", Vector2{320,467}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{420,467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{520,467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{620,467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{720,467}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{820,467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{920,467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1020,467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1120,467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1220,467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1320,467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1420,467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1520,467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1620,467}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{1720,467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1820,467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1920,467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2020,467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2120,467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2220,467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2320,467}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{2420,467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2520,467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2620,467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2720,467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2820,467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2920,467}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{3020,467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3120,467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3220,467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3320,467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3420,467}, PropType::TREE},

        Prop{"sprites/props/TreeGreen.png", Vector2{3700,467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3800,467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3900,467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{4000,467}, PropType::TREE},

        // row6
        Prop{"sprites/props/TreeGreen.png", Vector2{0,567}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{100,567}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{200,567}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{300,567}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{400,567}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{500,567}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{600,567}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{700,567}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{800,567}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{900,567}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1000,567}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1100,567}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1200,567}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{1300,567}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1400,567}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1500,567}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1600,567}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1700,567}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1800,567}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1900,567}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2000,567}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2100,567}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2200,567}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2300,567}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2400,567}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2500,567}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{2600,567}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2700,567}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2800,567}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2900,567}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3000,567}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3100,567}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3200,567}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3300,567}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3400,567}, PropType::TREE},

        // Prop{"sprites/props/TreeGreen.png", Vector2{3720,567}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3820,567}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3920,567}, PropType::TREE},

        // row7
        Prop{"sprites/props/TreeGreen.png", Vector2{20,667}, PropType::TREE}, 
        Prop{"sprites/props/TreeGreen.png", Vector2{120,667}, PropType::TREE}, 
        Prop{"sprites/props/TreeGreen.png", Vector2{220,667}, PropType::TREE}, 
        Prop{"sprites/props/TreeGreen.png", Vector2{320,667}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{420,667}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{520,667}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{620,667}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{720,667}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{820,667}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{920,667}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1020,667}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{1120,667}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1220,667}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1320,667}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1420,667}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1520,667}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{1620,667}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1720,667}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1820,667}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1920,667}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2020,667}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2120,667}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2220,667}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{2320,667}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2420,667}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2520,667}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2620,667}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{2720,667}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2820,667}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2920,667}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3020,667}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{3120,667}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3220,667}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3320,667}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3420,667}, PropType::TREE},

        // Prop{"sprites/props/TreeGreen.png", Vector2{3700,667}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3800,667}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3900,667}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{4000,667}, PropType::TREE},

        // row8
        Prop{"sprites/props/TreeGreen.png", Vector2{0,767}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{100,767}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{200,767}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{300,767}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{400,767}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{500,767}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{600,767}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{700,767}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{800,767}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{900,767}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1000,767}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{1100,767}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1200,767}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1300,767}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1400,767}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1500,767}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1600,767}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1700,767}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{1800,767}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1900,767}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2000,767}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2100,767}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2200,767}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2300,767}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2400,767}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2500,767}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2600,767}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{2700,767}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2800,767}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2900,767}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3000,767}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3100,767}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3200,767}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{3300,767}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3400,767}, PropType::TREE},

        // Prop{"sprites/props/TreeGreen.png", Vector2{3720,767}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{3820,767}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3920,767}, PropType::TREE},

        // row9
        Prop{"sprites/props/TreeGreen.png", Vector2{20,867}, PropType::TREE}, 
        Prop{"sprites/props/TreeGreen.png", Vector2{120,867}, PropType::TREE}, 
        Prop{"sprites/props/TreeGreen.png", Vector2{220,867}, PropType::TREE}, 
        Prop{"sprites/props/TreeGreen.png", Vector2{320,867}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{420,867}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{520,867}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{620,867}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{720,867}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{820,867}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{920,867}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1020,867}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1120,867}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1220,867}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1320,867}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1420,867}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1520,867}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1620,867}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1720,867}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1820,867}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1920,867}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2020,867}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2120,867}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2220,867}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2320,867}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2420,867}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2520,867}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2620,867}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2720,867}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2820,867}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2920,867}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3020,867}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3120,867}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3220,867}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3320,867}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3420,867}, PropType::TREE},

        Prop{"sprites/props/TreeGreen.png", Vector2{3700,867}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3800,867}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3900,867}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{4000,867}, PropType::TREE},

        // row10
        Prop{"sprites/props/TreeGreen.png", Vector2{0,967}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{100,967}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{200,967}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{300,967}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{400,967}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{500,967}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{600,967}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{700,967}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{800,967}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{900,967}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1000,967}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1100,967}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1200,967}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1300,967}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1400,967}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1500,967}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1600,967}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1700,967}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1800,967}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1900,967}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2000,967}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2100,967}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2200,967}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2300,967}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2400,967}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2500,967}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2600,967}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2700,967}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2800,967}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2900,967}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3000,967}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3100,967}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3200,967}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3300,967}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3400,967}, PropType::TREE},

        Prop{"sprites/props/TreeGreen.png", Vector2{3720,967}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3820,967}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3920,967}, PropType::TREE},

        // row11
        Prop{"sprites/props/TreeGreen.png", Vector2{20,1067}, PropType::TREE}, 
        Prop{"sprites/props/TreeGreen.png", Vector2{120,1067}, PropType::TREE}, 
        Prop{"sprites/props/TreeGreen.png", Vector2{220,1067}, PropType::TREE}, 
        Prop{"sprites/props/TreeGreen.png", Vector2{320,1067}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{420,1067}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{520,1067}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{620,1067}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{720,1067}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{820,1067}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{920,1067}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1020,1067}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1120,1067}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1220,1067}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1320,1067}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1420,1067}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1520,1067}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1620,1067}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1720,1067}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1820,1067}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1920,1067}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2020,1067}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2120,1067}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2220,1067}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2320,1067}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2420,1067}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2520,1067}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2620,1067}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2720,1067}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2820,1067}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2920,1067}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3020,1067}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3120,1067}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3220,1067}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3320,1067}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3420,1067}, PropType::TREE},

        Prop{"sprites/props/TreeGreen.png", Vector2{3700,1067}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3800,1067}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3900,1067}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{4000,1067}, PropType::TREE},

        // row12
        Prop{"sprites/props/TreeGreen.png", Vector2{0,1167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{100,1167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{200,1167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{300,1167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{400,1167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{500,1167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{600,1167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{700,1167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{800,1167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{900,1167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1000,1167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1100,1167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1200,1167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1300,1167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1400,1167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1500,1167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1600,1167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1700,1167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1800,1167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1900,1167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2000,1167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2100,1167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2200,1167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2300,1167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2400,1167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2500,1167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2600,1167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2700,1167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2800,1167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2900,1167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3000,1167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3100,1167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3200,1167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3300,1167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3400,1167}, PropType::TREE},

        Prop{"sprites/props/TreeGreen.png", Vector2{3720,1167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3820,1167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3920,1167}, PropType::TREE},

        // row13
        Prop{"sprites/props/TreeGreen.png", Vector2{20,1267}, PropType::TREE}, 
        Prop{"sprites/props/TreeGreen.png", Vector2{120,1267}, PropType::TREE}, 
        Prop{"sprites/props/TreeGreen.png", Vector2{220,1267}, PropType::TREE}, 
        Prop{"sprites/props/TreeGreen.png", Vector2{320,1267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{420,1267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{520,1267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{620,1267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{720,1267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{820,1267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{920,1267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1020,1267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1120,1267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1220,1267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1320,1267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1420,1267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1520,1267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1620,1267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1720,1267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1820,1267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1920,1267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2020,1267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2120,1267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2220,1267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2320,1267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2420,1267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2520,1267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2620,1267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2720,1267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2820,1267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2920,1267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3020,1267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3120,1267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3220,1267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3320,1267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3420,1267}, PropType::TREE},

        Prop{"sprites/props/TreeGreen.png", Vector2{3700,1267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3800,1267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3900,1267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{4000,1267}, PropType::TREE},

        // row14
        Prop{"sprites/props/TreeGreen.png", Vector2{0,1367}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{100,1367}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{200,1367}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{300,1367}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{400,1367}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{500,1367}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{600,1367}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{700,1367}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{800,1367}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{900,1367}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1000,1367}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1100,1367}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1200,1367}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1300,1367}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1400,1367}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1500,1367}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1600,1367}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1700,1367}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1800,1367}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1900,1367}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2000,1367}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2100,1367}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2200,1367}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2300,1367}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2400,1367}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2500,1367}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2600,1367}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2700,1367}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2800,1367}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2900,1367}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3000,1367}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3100,1367}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3200,1367}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3300,1367}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3400,1367}, PropType::TREE},

        Prop{"sprites/props/TreeGreen.png", Vector2{3720,1367}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3820,1367}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3920,1367}, PropType::TREE},

        // row15
        Prop{"sprites/props/TreeGreen.png", Vector2{20,1467}, PropType::TREE}, 
        Prop{"sprites/props/TreeGreen.png", Vector2{120,1467}, PropType::TREE}, 
        Prop{"sprites/props/TreeGreen.png", Vector2{220,1467}, PropType::TREE}, 
        Prop{"sprites/props/TreeGreen.png", Vector2{320,1467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{420,1467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{520,1467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{620,1467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{720,1467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{820,1467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{920,1467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1020,1467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1120,1467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1220,1467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1320,1467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1420,1467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1520,1467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1620,1467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1720,1467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1820,1467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1920,1467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2020,1467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2120,1467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2220,1467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2320,1467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2420,1467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2520,1467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2620,1467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2720,1467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2820,1467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2920,1467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3020,1467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3120,1467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3220,1467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3320,1467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3420,1467}, PropType::TREE},

        Prop{"sprites/props/TreeGreen.png", Vector2{3700,1467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3800,1467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3900,1467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{4000,1467}, PropType::TREE},

        // row16
        Prop{"sprites/props/TreeGreen.png", Vector2{0,1567}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{100,1567}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{200,1567}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{300,1567}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{400,1567}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{500,1567}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{600,1567}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{700,1567}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{800,1567}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{900,1567}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1000,1567}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1100,1567}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1200,1567}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1300,1567}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1400,1567}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1500,1567}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1600,1567}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1700,1567}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1800,1567}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1900,1567}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2000,1567}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2100,1567}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2200,1567}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2300,1567}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2400,1567}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2500,1567}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2600,1567}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2700,1567}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2800,1567}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2900,1567}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3000,1567}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3100,1567}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3200,1567}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3300,1567}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3400,1567}, PropType::TREE},

        Prop{"sprites/props/TreeGreen.png", Vector2{3720,1567}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3820,1567}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3920,1567}, PropType::TREE},

        // row17
        Prop{"sprites/props/TreeGreen.png", Vector2{20,1667}, PropType::TREE}, 
        Prop{"sprites/props/TreeGreen.png", Vector2{120,1667}, PropType::TREE}, 
        Prop{"sprites/props/TreeGreen.png", Vector2{220,1667}, PropType::TREE}, 
        Prop{"sprites/props/TreeGreen.png", Vector2{320,1667}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{420,1667}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{520,1667}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{620,1667}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{720,1667}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{820,1667}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{920,1667}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1020,1667}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1120,1667}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1220,1667}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1320,1667}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1420,1667}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1520,1667}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1620,1667}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1720,1667}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1820,1667}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1920,1667}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2020,1667}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2120,1667}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2220,1667}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2320,1667}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2420,1667}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2520,1667}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2620,1667}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2720,1667}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2820,1667}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2920,1667}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3020,1667}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3120,1667}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3220,1667}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3320,1667}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3420,1667}, PropType::TREE},

        Prop{"sprites/props/TreeGreen.png", Vector2{3700,1667}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3800,1667}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3900,1667}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{4000,1667}, PropType::TREE},

        // row18
        Prop{"sprites/props/TreeGreen.png", Vector2{0,1767}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{100,1767}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{200,1767}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{300,1767}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{400,1767}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{500,1767}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{600,1767}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{700,1767}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{800,1767}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{900,1767}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1000,1767}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1100,1767}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1200,1767}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1300,1767}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1400,1767}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1500,1767}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1600,1767}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1700,1767}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1800,1767}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1900,1767}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2000,1767}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2100,1767}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2200,1767}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2300,1767}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2400,1767}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2500,1767}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2600,1767}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2700,1767}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2800,1767}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2900,1767}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3000,1767}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3100,1767}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3200,1767}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3300,1767}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3400,1767}, PropType::TREE},

        Prop{"sprites/props/TreeGreen.png", Vector2{3720,1767}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3820,1767}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3920,1767}, PropType::TREE},

        // row19
        Prop{"sprites/props/TreeGreen.png", Vector2{20,1867}, PropType::TREE}, 
        Prop{"sprites/props/TreeGreen.png", Vector2{120,1867}, PropType::TREE}, 
        Prop{"sprites/props/TreeGreen.png", Vector2{220,1867}, PropType::TREE}, 
        Prop{"sprites/props/TreeGreen.png", Vector2{320,1867}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{420,1867}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{520,1867}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{620,1867}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{720,1867}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{820,1867}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{920,1867}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1020,1867}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1120,1867}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1220,1867}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1320,1867}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1420,1867}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1520,1867}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1620,1867}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1720,1867}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1820,1867}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1920,1867}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2020,1867}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2120,1867}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2220,1867}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2320,1867}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2420,1867}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2520,1867}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2620,1867}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2720,1867}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2820,1867}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2920,1867}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3020,1867}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3120,1867}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3220,1867}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3320,1867}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3420,1867}, PropType::TREE},

        Prop{"sprites/props/TreeGreen.png", Vector2{3700,1867}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3800,1867}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3900,1867}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{4000,1867}, PropType::TREE},

        // row20
        Prop{"sprites/props/TreeGreen.png", Vector2{0,1967}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{100,1967}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{200,1967}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{300,1967}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{400,1967}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{500,1967}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{600,1967}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{700,1967}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{800,1967}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{900,1967}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1000,1967}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1100,1967}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1200,1967}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1300,1967}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1400,1967}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1500,1967}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1600,1967}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1700,1967}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1800,1967}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1900,1967}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2000,1967}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2100,1967}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2200,1967}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2300,1967}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2400,1967}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2500,1967}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2600,1967}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2700,1967}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2800,1967}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2900,1967}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3000,1967}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3100,1967}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3200,1967}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3300,1967}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3400,1967}, PropType::TREE},

        Prop{"sprites/props/TreeGreen.png", Vector2{3720,1967}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3820,1967}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3920,1967}, PropType::TREE},

        // row21
        Prop{"sprites/props/TreeGreen.png", Vector2{20,2067}, PropType::TREE}, 
        Prop{"sprites/props/TreeGreen.png", Vector2{120,2067}, PropType::TREE}, 
        Prop{"sprites/props/TreeGreen.png", Vector2{220,2067}, PropType::TREE}, 
        Prop{"sprites/props/TreeGreen.png", Vector2{320,2067}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{420,2067}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{520,2067}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{620,2067}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{720,2067}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{820,2067}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{920,2067}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1020,2067}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1120,2067}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1220,2067}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1320,2067}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1420,2067}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1520,2067}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1620,2067}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1720,2067}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1820,2067}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1920,2067}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2020,2067}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2120,2067}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2220,2067}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2320,2067}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2420,2067}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2520,2067}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2620,2067}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2720,2067}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2820,2067}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2920,2067}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3020,2067}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3120,2067}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3220,2067}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3320,2067}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3420,2067}, PropType::TREE},

        Prop{"sprites/props/TreeGreen.png", Vector2{3700,2067}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3800,2067}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3900,2067}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{4000,2067}, PropType::TREE},

        // row22
        Prop{"sprites/props/TreeGreen.png", Vector2{0,2167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{100,2167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{200,2167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{300,2167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{400,2167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{500,2167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{600,2167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{700,2167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{800,2167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{900,2167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1000,2167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1100,2167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1200,2167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1300,2167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1400,2167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1500,2167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1600,2167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1700,2167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1800,2167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1900,2167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2000,2167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2100,2167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2200,2167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2300,2167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2400,2167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2500,2167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2600,2167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2700,2167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2800,2167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2900,2167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3000,2167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3100,2167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3200,2167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3300,2167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3400,2167}, PropType::TREE},

        Prop{"sprites/props/TreeGreen.png", Vector2{3720,2167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3820,2167}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3920,2167}, PropType::TREE},

        // row21
        Prop{"sprites/props/TreeGreen.png", Vector2{20,2267}, PropType::TREE}, 
        Prop{"sprites/props/TreeGreen.png", Vector2{120,2267}, PropType::TREE}, 
        // Prop{"sprites/props/TreeGreen.png", Vector2{220,2267}, PropType::TREE}, 
        Prop{"sprites/props/TreeGreen.png", Vector2{320,2267}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{420,2267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{520,2267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{620,2267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{720,2267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{820,2267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{920,2267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1020,2267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1120,2267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1220,2267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1320,2267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1420,2267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1520,2267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1620,2267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1720,2267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1820,2267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1920,2267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2020,2267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2120,2267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2220,2267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2320,2267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2420,2267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2520,2267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2620,2267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2720,2267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2820,2267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2920,2267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3020,2267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3120,2267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3220,2267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3320,2267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3420,2267}, PropType::TREE},

        Prop{"sprites/props/TreeGreen.png", Vector2{3700,2267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3800,2267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3900,2267}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{4000,2267}, PropType::TREE},

        // row22
        Prop{"sprites/props/TreeGreen.png", Vector2{50,2367}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{100,2367}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{200,2367}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{300,2367}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{400,2367}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{550,2367}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{600,2367}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{700,2367}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{800,2367}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{900,2367}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{1000,2367}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{1100,2367}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{1200,2367}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{1300,2367}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{1400,2367}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{1500,2367}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{1600,2367}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{1700,2367}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{1800,2367}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{1900,2367}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{2000,2367}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{2100,2367}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{2200,2367}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{2300,2367}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{2400,2367}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{2500,2367}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{2600,2367}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{2700,2367}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{2800,2367}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{2900,2367}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{3000,2367}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{3100,2367}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{3200,2367}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{3300,2367}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{3400,2367}, PropType::TREE},

        // Prop{"sprites/props/TreeGreen.png", Vector2{3720,2367}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{3820,2367}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{3920,2367}, PropType::TREE},

        // row23
        Prop{"sprites/props/TreeGreen.png", Vector2{20,2467}, PropType::TREE}, 
        // Prop{"sprites/props/TreeGreen.png", Vector2{120,2467}, PropType::TREE}, 
        // Prop{"sprites/props/TreeGreen.png", Vector2{220,2467}, PropType::TREE}, 
        // Prop{"sprites/props/TreeGreen.png", Vector2{320,2467}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{420,2467}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{520,2467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{620,2467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{720,2467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{820,2467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{920,2467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1020,2467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1120,2467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1220,2467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1320,2467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1420,2467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1520,2467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1620,2467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1720,2467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1820,2467}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1920,2467}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{2020,2467}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{2120,2467}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{2220,2467}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{2320,2467}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{2420,2467}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{2520,2467}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{2620,2467}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{2720,2467}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{2820,2467}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{2920,2467}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{3020,2467}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{3120,2467}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{3220,2467}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{3320,2467}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{3420,2467}, PropType::TREE},

        // Prop{"sprites/props/TreeGreen.png", Vector2{3700,2467}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{3800,2467}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{3900,2467}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{4000,2467}, PropType::TREE},

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
        Prop{"sprites/props/TreeGreen.png", Vector2{0,3567}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{150,3567}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{250,3567}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{350,3567}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{450,3567}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{665,3530}, PropType::TREE},

        // Prop{"sprites/props/TreeGreen.png", Vector2{2100,3567}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{2200,3567}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{2490,3567}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{2590,3567}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{2700,3567}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{2800,3567}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{2900,3567}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{3000,3567}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{3100,3567}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{3200,3567}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{3300,3567}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{3400,3567}, PropType::TREE},

        // Prop{"sprites/props/TreeGreen.png", Vector2{3720,3567}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{3820,3567}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3920,3567}, PropType::TREE},

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
        Prop{"sprites/props/TreeGreen.png", Vector2{20,3840}, PropType::TREE}, 
        Prop{"sprites/props/TreeGreen.png", Vector2{120,3840}, PropType::TREE}, 
        Prop{"sprites/props/TreeGreen.png", Vector2{220,3840}, PropType::TREE}, 
        // Prop{"sprites/props/TreeGreen.png", Vector2{320,3840}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{420,3840}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{520,3840}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{620,3840}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{720,3840}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{820,3840}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{920,3840}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1020,3840}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1120,3840}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1220,3840}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{1320,3840}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1420,3840}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1520,3840}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1620,3840}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1720,3840}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{1820,3840}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1920,3840}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2020,3840}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2120,3840}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{2220,3840}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2320,3840}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2420,3840}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2520,3840}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{2620,3840}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2720,3840}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2820,3840}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2920,3840}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3020,3840}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3120,3840}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{3220,3840}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{3320,3840}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3420,3840}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3520,3840}, PropType::TREE},
        // Prop{"sprites/props/TreeGreen.png", Vector2{3620,3840}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3720,3840}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3820,3840}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3920,3840}, PropType::TREE},

        // row37
        Prop{"sprites/props/TreeGreen.png", Vector2{0,3940}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{100,3940}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{200,3940}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{300,3940}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{400,3940}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{500,3940}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{600,3940}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{700,3940}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{800,3940}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{900,3940}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1000,3940}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1100,3940}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1200,3940}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1300,3940}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1400,3940}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1500,3940}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1600,3940}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1700,3940}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1800,3940}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{1900,3940}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2000,3940}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2100,3940}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2200,3940}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2300,3940}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2400,3940}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2500,3940}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2600,3940}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2700,3940}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2800,3940}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{2900,3940}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3000,3940}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3100,3940}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3200,3940}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3300,3940}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3400,3940}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3500,3940}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3600,3940}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3700,3940}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3800,3940}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{3900,3940}, PropType::TREE},
        Prop{"sprites/props/TreeGreen.png", Vector2{4000,3940}, PropType::TREE},

        // large tree
        Prop{"sprites/props/TreeGreen.png", Vector2{2185,2050}, PropType::TREE, 8.f},
    };
    Props.emplace_back(Trees);

    std::vector<Prop> Grass
    {
        // Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{825,680}, PropType::GRASS, 4.f, true},
        // Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{800,700}, PropType::GRASS, 4.f, true},
        // Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{830,710}, PropType::GRASS, 4.f, true},
        
        // Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{825+64,680}, PropType::GRASS, 4.f, true},
        // Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{800+64,700}, PropType::GRASS, 4.f, true},
        // Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{830+64,710}, PropType::GRASS, 4.f, true},

        // Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{825,680+64}, PropType::GRASS, 4.f, true},
        // Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{800,700+64}, PropType::GRASS, 4.f, true},
        // Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{830,710+64}, PropType::GRASS, 4.f, true},

        // Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{825+64,680+64}, PropType::GRASS, 4.f, true},
        // Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{800+64,700+64}, PropType::GRASS, 4.f, true},
        // Prop{Sprite{"sprites/props/GrassAnimation.png", 4, 1, 1.f/3.f}, Vector2{830+64,710+64}, PropType::GRASS, 4.f, true},
    };
    Props.emplace_back(Grass);
    
    return Props;
}

