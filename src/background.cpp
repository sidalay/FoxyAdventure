#include "headers/background.hpp"

Background::Background()
{
    
}

Background::~Background()
{
    UnloadTexture(Map);
}

void Background::Tick(Vector2 WorldPos) 
{
    MapPos = Vector2Scale(WorldPos, -1.f);

    if (IsKeyPressed(KEY_M)) {
        IsMiniMap = !IsMiniMap;
    }
}

void Background::Draw() 
{
    DrawTextureEx(Map, MapPos, 0.0f, Scale, WHITE);
}

void Background::DrawMiniMap(Vector2 CharWorldPos)
{
    Vector2 Position {
        (float)(GetScreenWidth()/2 - (MiniMap.width/2) * .35f),
        (float)(GetScreenHeight()/2 - (MiniMap.height/2) * .35f)
    };

    if (IsMiniMap) {
        // Draw MiniMap Border
        DrawRectangle(Position.x - 10, Position.y - 10, 95*Scale,95*Scale,Color{41,29,43,255});
        // Draw MiniMap
        DrawTextureEx(MiniMap, Position, 0.0f, .35f, WHITE);
        // Draw MiniMap Character Indicator
        DrawRectangle(Position.x + 54.f + ((CharWorldPos.x/4.f) * .35f), Position.y + 30.f + ((CharWorldPos.y/4.f) * .35f), 5, 5, RED);  
    }
}