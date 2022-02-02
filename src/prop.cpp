#include "headers/prop.hpp"

// Constructor for inanimate props
Prop::Prop(const char* TexturePath, Vector2 Pos, PropType Type, float Scale, bool Moveable, bool Interactable)
    : Object{TexturePath}, Type{Type}, WorldPos{Pos}, Scale{Scale}, Interactable{Interactable}, Moveable{Moveable}
{
    if (Type == PropType::BOULDER ||
        Type == PropType::BUSH ||
        Type == PropType::STUMP ||
        Type == PropType::TREE ||
        Type == PropType::FENCE ||
        Type == PropType::rHOUSELEFT ||
        Type == PropType::rHOUSERIGHT ||
        Type == PropType::bHOUSELEFT ||
        Type == PropType::bHOUSERIGHT ||
        Type == PropType::DOOR ||
        Type == PropType::GRASS ||
        Type == PropType::TOPWALL ||
        Type == PropType::LEFTSIDEWALL ||
        Type == PropType::RIGHTSIDEWALL ||
        Type == PropType::BOTTOMWALL ||
        Type == PropType::HOLE ||
        Type == PropType::DUNGEONLEFT ||
        Type == PropType::DUNGEONRIGHT ||
        Type == PropType::DUNGEON ||
        Type == PropType::TREASURE ||
        Type == PropType::NPC_A ||
        Type == PropType::NPC_B)
    {
        Collidable = true;
    }
}

// Constructor for animated props
Prop::Prop(Sprite Object, Vector2 Pos, PropType Type, float Scale, bool Moveable, bool Interactable, Texture2D Item, float ItemScale)
    : Object{Object}, Type{Type}, WorldPos{Pos}, Scale{Scale}, Interactable{Interactable}, Moveable{Moveable}, Item{Item}, ItemScale{ItemScale}
{
    if (Type == PropType::BOULDER ||
        Type == PropType::BUSH ||
        Type == PropType::STUMP ||
        Type == PropType::TREE ||
        Type == PropType::FENCE ||
        Type == PropType::rHOUSELEFT ||
        Type == PropType::rHOUSERIGHT ||
        Type == PropType::bHOUSELEFT ||
        Type == PropType::bHOUSERIGHT ||
        Type == PropType::DOOR ||
        Type == PropType::GRASS ||
        Type == PropType::TOPWALL ||
        Type == PropType::LEFTSIDEWALL ||
        Type == PropType::RIGHTSIDEWALL ||
        Type == PropType::BOTTOMWALL ||
        Type == PropType::HOLE ||
        Type == PropType::DUNGEONLEFT ||
        Type == PropType::DUNGEONRIGHT ||
        Type == PropType::DUNGEON ||
        Type == PropType::TREASURE ||
        Type == PropType::NPC_A ||
        Type == PropType::NPC_B)
    {
        Collidable = true;
    }

    if (Type == PropType::TREASURE)
    {
        if (ItemScale == 1.f)
            ItemPos.x = 24;
        else if (ItemScale == 2.f)
            ItemPos.x = 16;
        else if (ItemScale == 3.f)
            ItemPos.x = 8;
        else if (ItemScale == 4.f)
            ItemPos.x = 0;
        else 
            ItemPos.x = 0;
    }
}

void Prop::Tick(float DeltaTime, Background& Map)
{
    if ((Type == PropType::NPC_A || Type == PropType::NPC_B) && !Talking)
    {
        Object.Tick(DeltaTime);
    }

    if (Active)
    {
        if (Type == PropType::GRASS)
        {
            Object.Tick(DeltaTime);
        }
        else if (Type == PropType::TREASURE)
        {
            if (!Opened)
            {
                Object.Tick(DeltaTime);
            }

            RunningTime += DeltaTime;

            if (RunningTime > Object.UpdateTime * 4 && RunningTime < Object.UpdateTime * 8)
            {
                Opened = true;
                Opening = true;
                RunningTime = 0.f; 
            }
        }
        else if (Type == PropType::DOOR)
        {
            Opened = true;
        }
        else if ((Type == PropType::NPC_A || Type == PropType::NPC_B))
        {
            // Object.Tick(DeltaTime);
            Talking = true;
        }
    }
    else {
        // Draw treasure item for (UpdateTime * X) seconds
        if (Opening)
        {
            RunningTime += DeltaTime;
            if (RunningTime >= Object.UpdateTime * 10)
            {
                Opening = false;
                RunningTime = 0.f; 
            }
        }
    }
}

void Prop::Draw(Vector2 CharacterWorldPos)
{
    Vector2 ScreenPos {Vector2Subtract(WorldPos, CharacterWorldPos)}; // Where the prop is drawn on the screen
    // Vector2 MaxItemDistance {0,-20};

    // Draw only if Prop is viewable in the screen frame
    if ((WorldPos.x >= (CharacterWorldPos.x + 615) - (GetScreenWidth()/2 + (Object.Texture.width * Scale))) && (WorldPos.x <= (CharacterWorldPos.x + 615) + (GetScreenWidth()/2 + (Object.Texture.width * Scale))) &&
       (WorldPos.y >= (CharacterWorldPos.y + 335) - (GetScreenHeight()/2 + (Object.Texture.height * Scale))) && (WorldPos.y <= (CharacterWorldPos.y + 335) + (GetScreenHeight()/2 + (Object.Texture.height * Scale))))
    {
        DrawTexturePro(Object.Texture, Object.GetSourceRec(), Object.GetPosRec(ScreenPos, Scale), Vector2{}, 0.f, WHITE);
    }
    
    if (Opening)
    {
        DrawTextureEx(Item, Vector2Add(ScreenPos, ItemPos), 0.f, ItemScale, WHITE);
        ItemPos = Vector2Add(ItemPos, Vector2{0,-0.1f});
    }

    // NPC Speech Boxes
    if (Talking)
    {
        DrawTextureEx(SpeechName, Vector2{376,438}, 0.f, 5.f, WHITE);
        DrawTextureEx(SpeechBox, Vector2{352,518}, 0.f, 12.f, WHITE);

        if (Type == PropType::NPC_A)
        {
            DrawText("Diana", 399, 490, 30, WHITE);
            DrawSpeech();
        }
        else if (Type == PropType::NPC_B)
        {
            DrawText("Jade", 399, 490, 30, WHITE);
            DrawSpeech();
        }
    }

    // CheckActivity(ScreenPos);
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
                ScreenPos.y + (Object.Texture.height * Scale) * .1f,
                Object.Texture.width * Scale,
                (Object.Texture.height * Scale) * .9f
            };
        }
        case PropType::GRASS:
        {
            return Rectangle
            {
                ScreenPos.x + (Object.Texture.width * Scale) * .05f,
                ScreenPos.y + (Object.Texture.height * Scale) * .4f,
                (Object.Texture.width - Object.Texture.width * .875f) * Scale,
                (Object.Texture.height - Object.Texture.height * .65f) * Scale
            };
        }
        case PropType::rHOUSELEFT:
        {
            return Rectangle
            {
                ScreenPos.x + (Object.Texture.width * Scale) * .08f,
                ScreenPos.y + (Object.Texture.height * Scale) * .6f,
                (Object.Texture.width * Scale) * .43f,
                (Object.Texture.height * Scale) * .3f
            };
        }
        case PropType::rHOUSERIGHT:
        {
            return Rectangle
            {
                ScreenPos.x + (Object.Texture.width * Scale) * .75f,
                ScreenPos.y + (Object.Texture.height * Scale) * .6f,
                (Object.Texture.width * Scale) * .19f,
                (Object.Texture.height * Scale) * .3f
            };
        }
        case PropType::bHOUSELEFT:
        {
            return Rectangle
            {
                ScreenPos.x + (Object.Texture.width * Scale) * .08f,
                ScreenPos.y + (Object.Texture.height * Scale) * .6f,
                (Object.Texture.width * Scale) * .10f,
                (Object.Texture.height * Scale) * .35f
            };
        }
        case PropType::bHOUSERIGHT:
        {
            return Rectangle
            {
                
                ScreenPos.x + (Object.Texture.width * Scale) * .40f,
                ScreenPos.y + (Object.Texture.height * Scale) * .6f,
                (Object.Texture.width * Scale) * .50f,
                (Object.Texture.height * Scale) * .35f
            };
        }
        case PropType::DOOR:
        {
            return Rectangle
            {
                ScreenPos.x + (Object.Texture.width * Scale) * .31f,
                ScreenPos.y + (Object.Texture.height * Scale) * .20f,
                (Object.Texture.width * Scale) - (Object.Texture.width * Scale) * .59f,
                (Object.Texture.height * Scale) * .35f
            };
        }
        case PropType::TOPWALL:
        {
            return Rectangle
            {
                ScreenPos.x,
                ScreenPos.y,
                Object.Texture.width * Scale,
                (Object.Texture.height * Scale) * .3f
            };
        }
        case PropType::LEFTSIDEWALL:
        {
            return Rectangle   
            {
                ScreenPos.x,
                ScreenPos.y,
                (Object.Texture.width * Scale) * .2f,
                Object.Texture.height * Scale
            };
        }
        case PropType::RIGHTSIDEWALL:
        {
            return Rectangle
            {
                ScreenPos.x + (Object.Texture.width * Scale) * .8f,
                ScreenPos.y,
                (Object.Texture.width * Scale) * .2f,
                Object.Texture.height * Scale
            };
        }
        case PropType::BOTTOMWALL:
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
        case PropType::HOLE:
        {
            return Rectangle
            {
                ScreenPos.x + (Object.Texture.width * Scale) * .1f,
                ScreenPos.y + (Object.Texture.height * Scale) * .2f,
                (Object.Texture.width * Scale) * .8f,
                (Object.Texture.height * Scale) * .7f
            };
        }
        case PropType::DUNGEON:
        {
            return Rectangle
            {
                ScreenPos.x + (Object.Texture.width * Scale) * .31f,
                ScreenPos.y + (Object.Texture.height * Scale) * .20f,
                (Object.Texture.width * Scale) - (Object.Texture.width * Scale) * .59f,
                (Object.Texture.height * Scale) * .35f
            };
        }
        case PropType::DUNGEONLEFT:
        {
            return Rectangle
            {
                ScreenPos.x,
                ScreenPos.y + (Object.Texture.height * Scale) * .20f,
                (Object.Texture.width * Scale) - (Object.Texture.width * Scale) * .70f,
                (Object.Texture.height * Scale) * .80f
            };
        }
        case PropType::DUNGEONRIGHT:
        {
            return Rectangle
            {
                ScreenPos.x + (Object.Texture.width * Scale) * .72f,
                ScreenPos.y + (Object.Texture.height * Scale) * .20f,
                (Object.Texture.width * Scale) - (Object.Texture.width * Scale) * .70f,
                (Object.Texture.height * Scale) * .80f
            };
        }
        case PropType::TREASURE:
        {
            return Rectangle
            {
                ScreenPos.x + (Object.Texture.width * Scale)/4.f * .10f,
                ScreenPos.y + (Object.Texture.height * Scale) * .10f,
                (Object.Texture.width * Scale)/4.f - (Object.Texture.width * Scale)/4.f * .10f,
                Object.Texture.height * Scale - (Object.Texture.height * Scale) * .10f
            };
        }
        case PropType::NPC_A:
        {
            return Rectangle
            {
                ScreenPos.x + (Object.Texture.width * Scale)/4.f * .10f,
                ScreenPos.y + (Object.Texture.height * Scale) * .10f,
                (Object.Texture.width * Scale)/4.f - (Object.Texture.width * Scale)/4.f * .10f,
                Object.Texture.height * Scale - (Object.Texture.height * Scale) * .10f
            };
        }
        case PropType::NPC_B:
        {
            return Rectangle
            {
                ScreenPos.x + (Object.Texture.width * Scale)/4.f * .10f,
                ScreenPos.y + (Object.Texture.height * Scale) * .10f,
                (Object.Texture.width * Scale)/4.f - (Object.Texture.width * Scale)/4.f * .10f,
                Object.Texture.height * Scale - (Object.Texture.height * Scale) * .10f
            };
        }
        default:
        {
            return Rectangle
            {
                ScreenPos.x,
                ScreenPos.y,
                Object.Texture.width * Scale,
                Object.Texture.height * Scale
            };
        }
    }
}

Rectangle Prop::GetInteractRec(Vector2 CharacterWorldPos)
{
    Vector2 ScreenPos {Vector2Subtract(WorldPos, CharacterWorldPos)};

    switch (Type)
    {
        case PropType::STUMP:
        {
            return Rectangle
            {
                ScreenPos.x - (Object.Texture.width * Scale) * .10f,
                ScreenPos.y - (Object.Texture.height * Scale) * .10f,
                Object.Texture.width * Scale + (Object.Texture.width * Scale) * .20f,
                Object.Texture.height * Scale + (Object.Texture.height * Scale) * .20f
            };
        }
        case PropType::TREASURE:
        {
            return Rectangle
            {
                ScreenPos.x - (Object.Texture.width * Scale)/4.f * .45f,
                ScreenPos.y - (Object.Texture.height * Scale) * .45f,
                (Object.Texture.width * Scale)/4.f + (Object.Texture.width * Scale)/4.f,
                (Object.Texture.height * Scale) + (Object.Texture.height * Scale)
            };
            break;
        }
        case PropType::NPC_A:
        {
            return Rectangle
            {
                ScreenPos.x - (Object.Texture.width * Scale)/4.f * .45f,
                ScreenPos.y - (Object.Texture.height * Scale) * .45f,
                (Object.Texture.width * Scale)/4.f + (Object.Texture.width * Scale)/4.f,
                (Object.Texture.height * Scale) + (Object.Texture.height * Scale)
            };
        }
        case PropType::NPC_B:
        {
            return Rectangle
            {
                ScreenPos.x - (Object.Texture.width * Scale)/4.f * .45f,
                ScreenPos.y - (Object.Texture.height * Scale) * .45f,
                (Object.Texture.width * Scale)/4.f + (Object.Texture.width * Scale)/4.f,
                (Object.Texture.height * Scale) + (Object.Texture.height * Scale)
            };
        }
        default:
        {
            return Rectangle
            {
                ScreenPos.x,
                ScreenPos.y,
                (Object.Texture.width * Scale),
                Object.Texture.height * Scale
            };
        }
    }
}

void Prop::SetWorldPos(Vector2 Direction)
{
    WorldPos = Vector2Add(WorldPos, Direction);
}

bool Prop::CheckMovement(Background& Map, Vector2 CharWorldPos, Vector2 Direction, float Speed, std::vector<std::vector<Prop>>* Props)
{
    bool Colliding{false};
    PrevWorldPos = WorldPos;

    if (Vector2Length(Direction) != 0.f)
    {
        // set MapPos -= Direction
        WorldPos = Vector2Add(WorldPos, Vector2Scale(Vector2Normalize(Direction), Speed));
    }

    if (WorldPos.x < 0.f ||
        WorldPos.y < 0.f ||
        WorldPos.x + (Object.Texture.width * Scale) > Map.GetMap().width * Map.GetScale() ||
        WorldPos.y + (Object.Texture.height * Scale) > Map.GetMap().height * Map.GetScale())
    {
        OutOfBounds = true;
        UndoMovement();
    }
    else
    {
        OutOfBounds = false;
    }

    for (auto& Proptype:*Props) {
        for (auto& Prop:Proptype) {
            if (Prop.HasCollision()) {
                if (CheckCollisionRecs(GetCollisionRec(CharWorldPos), Prop.GetCollisionRec(CharWorldPos))) {
                    if (!Prop.IsMoveable()) {
                        UndoMovement();
                        Colliding = true;
                    }
                }
            }
        }
    }
    return Colliding;
}

void Prop::CheckActivity(Vector2 ScreenPos)
{
    
}

void Prop::DrawSpeech()
{
    switch(Act)
    {
        case Progress::ACT_I:
        {   
            if (Type == PropType::NPC_A)
            {
                DrawText("Hello there, little Foxy! You look a little hungry.", 390, 550, 20, WHITE);
                DrawText("Here you go, have some food little one.", 390, 570, 20, WHITE);
            }
            else 
            {
                DrawText("Hi love! Have you seen my little one? Coulda sworn", 390, 550, 20, WHITE);
                DrawText("he was right here...", 390, 570, 20, WHITE);
            };

            if (IsKeyReleased(KEY_ENTER))
            {
                Opened = true;
                Talking = false;
            }
            break;
        }
        case Progress::ACT_II:
        {
            if (Type == PropType::NPC_A)
            {
                
            }
            else 
            {

            };

            if (IsKeyPressed(KEY_SPACE))
            {
                Opened = true;
            }
            break;
        }
        case Progress::ACT_III:
        {
            if (Type == PropType::NPC_A)
            {
                
            }
            else 
            {

            };

            if (IsKeyPressed(KEY_SPACE))
            {
                Opened = true;
            }
            break;
        }
        case Progress::ACT_IV:
        {
            if (Type == PropType::NPC_A)
            {
                
            }
            else 
            {

            };
        
            if (IsKeyPressed(KEY_SPACE))
            {
                Opened = true;
            }
            break;
        }
        default:
            // Opened = true;
            // Talking = false;
            break;
    }
}

// ---------------------------------------------------------------------

Props::Props(std::vector<std::vector<Prop>>* Under, std::vector<std::vector<Prop>>* Over)
    : Under{Under}, Over{Over}
{

}