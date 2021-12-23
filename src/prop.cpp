#include "headers/prop.hpp"

// Constructor for inanimate props
Prop::Prop(const char* TexturePath, Vector2 Pos, PropType Type, float Scale, bool Interactable)
    : Object{TexturePath}, Type{Type}, WorldPos{Pos}, Scale{Scale}, Interactable{Interactable}
{
    if (Type == PropType::BOULDER ||
        Type == PropType::BUSH ||
        Type == PropType::STUMP ||
        Type == PropType::TREE)
    {
        Collidable = true;
    }
}

// Constructor for animated props
Prop::Prop(Sprite Object, Vector2 Pos, PropType Type, float Scale, bool Interactable)
    : Object{Object}, Type{Type}, WorldPos{Pos}, Scale{Scale}, Interactable{Interactable}
{
    if (Type == PropType::BOULDER ||
        Type == PropType::BUSH ||
        Type == PropType::STUMP ||
        Type == PropType::TREE)
    {
        Collidable = true;
    }
}

void Prop::Tick(float DeltaTime)
{
    Object.Tick(DeltaTime);
}

void Prop::Draw(Vector2 CharacterWorldPos)
{
    Vector2 ScreenPos {Vector2Subtract(WorldPos, CharacterWorldPos)}; // Where the prop is drawn on the screen
    // DrawTextureEx(Object.Texture, ScreenPos, 0.f, Scale, WHITE);
    DrawTexturePro(Object.Texture, Object.GetSourceRec(), Object.GetPosRec(ScreenPos, Scale), Vector2{}, 0.f, WHITE);
}

Rectangle Prop::GetCollisionRec(Vector2 CharacterWorldPos)
{
    Vector2 ScreenPos {Vector2Subtract(WorldPos, CharacterWorldPos)};

    switch (Type)
    {
        case PropType::TREE:
        {
            return Rectangle
            {
                ScreenPos.x + ((Object.Texture.width*.23f) * Scale),
                ScreenPos.y + ((Object.Texture.height*.75f) * Scale),
                (Object.Texture.width - Object.Texture.width*.47f) * Scale,
                (Object.Texture.height - Object.Texture.height*.75f) * Scale
            };
            break;
        }
        case PropType::ROCK:
        {
            return Rectangle
            {
                ScreenPos.x,
                ScreenPos.y,
                Object.Texture.width * Scale,
                Object.Texture.height * Scale
            };
            break;
        }
        case PropType::BOULDER:
        {
            return Rectangle
            {
                ScreenPos.x,
                ScreenPos.y,
                (Object.Texture.width - Object.Texture.width*.125f) * Scale,
                (Object.Texture.height - Object.Texture.height*.125f) * Scale   
            };
        }
        case PropType::FLOWER:
        {
            return Rectangle
            {
                ScreenPos.x,
                ScreenPos.y,
                Object.Texture.width * Scale,
                Object.Texture.height * Scale
            };
        }
        case PropType::FENCE:
        {
            return Rectangle
            {
                ScreenPos.x,
                ScreenPos.y,
                Object.Texture.width * Scale,
                Object.Texture.height * Scale
            };
        }
        case PropType::GRASS:
        {
            return Rectangle
            {
                ScreenPos.x,
                ScreenPos.y,
                (Object.Texture.width * Scale / 2),
                Object.Texture.height * Scale
            };
        }
        case PropType::WALL:
        {
            return Rectangle
            {
                ScreenPos.x,
                ScreenPos.y,
                Object.Texture.width * Scale,
                Object.Texture.height * Scale
            };
        }
        case PropType::STUMP:
        {
            return Rectangle
            {
                ScreenPos.x,
                ScreenPos.y,
                Object.Texture.width * Scale,
                Object.Texture.height * Scale
            };
        }
        default:
        {
            return Rectangle{};
        }
    }
}

void Prop::SetWorldPos(Vector2 Direction)
{
    DrawText("Im setting the world position!", 20, 0, 20, BLUE);
    WorldPos = Vector2Add(WorldPos, Direction);
    DrawText(TextFormat("Prop.WorldPos.x: %i, Prop.WorldPos.y: %i", (int)WorldPos.x, (int)WorldPos.y), 20, 100, 20, WHITE);
}

// ---------------------------------------------------------------------

Props::Props(std::vector<std::vector<Prop>>* Under, std::vector<std::vector<Prop>>* Over)
    : Under{Under}, Over{Over}
{

}