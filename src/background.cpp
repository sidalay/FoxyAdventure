#include "headers/background.hpp"

Background::Background(GameTexture& GameTextures)
    : GameTextures{&GameTextures}
{

}

void Background::Tick(Vector2 WorldPos) 
{
    MapPos = Vector2Scale(WorldPos, -1.f);

    if (IsKeyPressed(KEY_M)) {
        MiniMapOpen = !MiniMapOpen;
    }
}

void Background::Draw() 
{
    DrawTextureEx(GameTextures->Map, MapPos, 0.0f, Scale, WHITE);
}

void Background::DrawMiniMap(Vector2 CharWorldPos)
{
    Vector2 Position {
        static_cast<float>(GetScreenWidth()/1.3f - (GameTextures->MiniMap.width/2) * .35f),
        static_cast<float>(GetScreenHeight()/2.f - (GameTextures->MiniMap.height/2) * .35f)
    };

    if (MiniMapOpen) {

        // Draw MiniMap Border
        DrawTextureEx(GameTextures->SquareContainer, Vector2Subtract(Position, Vector2{10, 10}), 0.f, 7.9f, WHITE);
        
        // Draw MiniMap
        DrawTextureEx(GameTextures->MiniMap, Position, 0.0f, .35f, WHITE);

        // Draw MiniMap Character Indicator
        DrawRectangle(Position.x + 54.f + ((CharWorldPos.x/4.f) * .35f), Position.y + 30.f + ((CharWorldPos.y/4.f) * .35f), 5, 5, RED);  
    }
}

Vector2 Background::GetMapSize()
{
    return Vector2{static_cast<float>(GameTextures->Map.width), static_cast<float>(GameTextures->Map.height)};
}