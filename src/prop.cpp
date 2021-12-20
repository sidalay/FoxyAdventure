#include "headers/prop.hpp"

Prop::Prop(const char* TexturePath, Vector2 Pos, PropType Type, float Scale)
    : Object{TexturePath}, Type{Type}, WorldPos{Pos}, Scale{Scale}
{

}

Prop::Prop(Sprite Object, Vector2 Pos, PropType Type, float Scale)
    : Object{Object}, Type{Type}, WorldPos{Pos}, Scale{Scale}
{

}

void Prop::Tick(float DeltaTime)
{
    Object.Tick(DeltaTime);
}

void Prop::Draw(Vector2 CharacterWorldPos)
{
    Vector2 ScreenPos {Vector2Subtract(WorldPos, CharacterWorldPos)};    // Where the prop is drawn on the screen
    DrawTextureEx(Object.Texture, ScreenPos, 0.f, Scale, WHITE);
}

Rectangle Prop::GetCollisionRec(Vector2 CharacterWorldPos)
{
    Vector2 ScreenPos {Vector2Subtract(WorldPos, CharacterWorldPos)};
    return Rectangle
    {
        ScreenPos.x + ((Object.Texture.width*.23f) * Scale),
        ScreenPos.y + ((Object.Texture.height*.75f) * Scale),
        (Object.Texture.width - Object.Texture.width*.47f) * Scale,
        (Object.Texture.height - Object.Texture.height*.75f) * Scale
    };
}