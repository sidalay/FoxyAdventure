#include "headers/background.hpp"

Background::~Background()
{
    UnloadTexture(Map);
    UnloadTexture(MiniMap);
    UnloadTexture(SquareContainer);
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
        (float)(GetScreenWidth()/1.3 - (MiniMap.width/2) * .35f),
        (float)(GetScreenHeight()/2 - (MiniMap.height/2) * .35f)
    };

    if (IsMiniMap) {

        // Draw MiniMap Border
        DrawTextureEx(SquareContainer, Vector2Subtract(Position, Vector2{10, 10}), 0.f, 7.9f, WHITE);
        
        // Draw MiniMap
        DrawTextureEx(MiniMap, Position, 0.0f, .35f, WHITE);

        // Draw MiniMap Character Indicator
        DrawRectangle(Position.x + 54.f + ((CharWorldPos.x/4.f) * .35f), Position.y + 30.f + ((CharWorldPos.y/4.f) * .35f), 5, 5, RED);  
    }
}