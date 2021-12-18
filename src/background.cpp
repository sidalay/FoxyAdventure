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
}

void Background::Draw() 
{
    DrawTextureEx(Map, MapPos, 0.0, Scale, WHITE);
}