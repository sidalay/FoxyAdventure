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
        MiniMap = !MiniMap;
    }
}

void Background::Draw() 
{
    DrawTextureEx(Map, MapPos, 0.0f, Scale, WHITE);
}

void Background::DrawMiniMap(Vector2 CharWorldPos)
{
    Vector2 Position {
        (float)(400 - (Map.width/2) * .35f),
        (float)(400 - (Map.height/2) * .35f)
    };

    if (MiniMap) {
        DrawRectangle(Position.x - 10, Position.y - 10, 95*Scale,95*Scale,BLACK);
        DrawTextureEx(Map, Position, 0.0f, .35f, WHITE);
        DrawRectangle(Position.x + 33.f + ((CharWorldPos.x/4.f) * .35f), Position.y + 35.f + ((CharWorldPos.y/4.f) * .35f), 5, 5, RED);  
    }
}