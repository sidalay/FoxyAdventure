#include "headers/npc.hpp"

NPC::NPC(Sprite Idle, Vector2 WorldPos)
    : Idle{Idle}, WorldPos{WorldPos}
{

}

NPC::~NPC()
{
    UnloadTexture(Idle.Texture);
}

void NPC::Tick()
{
    CurrentSprite->Tick(GetFrameTime());
}

void NPC::Draw(Vector2 CharacterWorldPos)
{
    Vector2 ScreenPos {Vector2Subtract(WorldPos, CharacterWorldPos)}; // Where the prop is drawn on the screen

    if ((WorldPos.x >= (CharacterWorldPos.x + 615) - (GetScreenWidth()/2 + (CurrentSprite->Texture.width * Scale))) && (WorldPos.x <= (CharacterWorldPos.x + 615) + (GetScreenWidth()/2 + (CurrentSprite->Texture.width * Scale))) &&
       (WorldPos.y >= (CharacterWorldPos.y + 335) - (GetScreenHeight()/2 + (CurrentSprite->Texture.height * Scale))) && (WorldPos.y <= (CharacterWorldPos.y + 335) + (GetScreenHeight()/2 + (CurrentSprite->Texture.height * Scale))))
    {
        DrawTexturePro(CurrentSprite->Texture, CurrentSprite->GetSourceRec(), CurrentSprite->GetPosRec(ScreenPos, Scale), Vector2{}, 0.f, WHITE);
    }
}

// Update which portion of the spritesheet is drawn
void NPC::UpdateSource()
{
    Source.x = CurrentSprite->FrameX * CurrentSprite->Texture.width / CurrentSprite->MaxFramesX;
    Source.y = CurrentSprite->FrameY * CurrentSprite->Texture.height / CurrentSprite->MaxFramesY;
    Source.width = CurrentSprite->Texture.width/CurrentSprite->MaxFramesX;
    Source.height = CurrentSprite->Texture.height/CurrentSprite->MaxFramesY;
}