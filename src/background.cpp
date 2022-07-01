#include "background.hpp"

Background::Background(const GameTexture& GameTextures, const GameAudio& Audio)
    : GameTextures{GameTextures}, Audio{Audio}
{

}

void Background::Tick(const Vector2 WorldPos) 
{
    MapPos = Vector2Scale(WorldPos, -1.f);

    if (IsKeyPressed(KEY_M)) {
        MiniMapOpen = !MiniMapOpen;
        SetSoundVolume(Audio.Transition, 0.2f);
        PlaySound(Audio.Transition);
    }
}

void Background::DrawForest() 
{
    DrawTextureEx(GameTextures.Map, MapPos, 0.0f, Scale, WHITE);
}

void Background::DrawDungeon()
{
    DrawTextureEx(GameTextures.DungeonMap, MapPos, 0.0f, Scale, WHITE);
}

void Background::DrawMiniMap(const Vector2 CharWorldPos)
{
    Vector2 Position {
        static_cast<float>(GetScreenWidth()/1.3f - (GameTextures.MiniMap.width/2) * .35f),
        static_cast<float>(GetScreenHeight()/2.f - (GameTextures.MiniMap.height/2) * .35f)
    };

    if (MiniMapOpen) {
        DrawTextureEx(GameTextures.SquareContainer, Vector2Subtract(Position, Vector2{10.f, 10.f}), 0.f, 7.9f, WHITE);
        DrawTextureEx(GameTextures.MiniMap, Position, 0.0f, .35f, WHITE);
        // Draw MiniMap Character Indicator
        DrawRectangle(Position.x + 54.f + ((CharWorldPos.x/4.f) * .35f), Position.y + 30.f + ((CharWorldPos.y/4.f) * .35f), 5, 5, RED);  
    }
}

Vector2 Background::GetForestMapSize()
{
    return Vector2{static_cast<float>(GameTextures.Map.width) * Scale, static_cast<float>(GameTextures.Map.height) * Scale};
}

Vector2 Background::GetDungeonMapSize()
{
    return Vector2{static_cast<float>(GameTextures.DungeonMap.width) * Scale, static_cast<float>(GameTextures.DungeonMap.height) * Scale};
}