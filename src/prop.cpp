#include "headers/prop.hpp"

// Constructor for inanimate props
Prop::Prop(const Texture2D& Texture, Vector2 Pos, PropType Type, GameTexture& GameTextures, float Scale, bool Moveable, bool Interactable)
    : Object{Texture}, Type{Type}, GameTextures{&GameTextures}, WorldPos{Pos}, Scale{Scale}, Interactable{Interactable}, 
      Moveable{Moveable}
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
        Type == PropType::NPC_B ||
        Type == PropType::NPC_C ||
        Type == PropType::ALTAR ||
        Type == PropType::BIGTREASURE)
    {
        Collidable = true;
    }
}

// Constructor for animated altar pieces
Prop::Prop(Sprite Object, Vector2 Pos, PropType Type, GameTexture& GameTextures, std::string ItemName, bool Spawned, bool Interactable)
    : Object{Object}, Type{Type}, GameTextures{&GameTextures}, WorldPos{Pos}, Interactable{Interactable}, Spawned{Spawned}, ItemName{ItemName} 
{
    Collidable = true;
}

// Constructor for animated props
Prop::Prop(Sprite Object, Vector2 Pos, PropType Type, GameTexture& GameTextures, const Texture2D& Item, float Scale, bool Moveable, bool Interactable, 
           Progress Act, PropType NPC, std::string ItemName, float ItemScale)
    : Object{Object}, Type{Type}, GameTextures{&GameTextures}, WorldPos{Pos}, Scale{Scale}, Interactable{Interactable}, Moveable{Moveable}, 
      TriggerAct{Act}, TriggerNPC{NPC}, Item{Item}, ItemName{ItemName}, ItemScale{ItemScale} 
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
        Type == PropType::NPC_B ||
        Type == PropType::NPC_C ||
        Type == PropType::ALTAR ||
        Type == PropType::BIGTREASURE)
    {
        Collidable = true;
    }

    if (Type == PropType::BIGTREASURE)
    {
        Spawned = false;
    }

    if (Type == PropType::TREASURE || Type == PropType::BIGTREASURE)
    {
        if (ItemScale == 1.f)
            ItemPos.x = 24;
        else if (ItemScale == 2.f)
            ItemPos.x = 16;
        else if (ItemScale == 3.f)
            ItemPos.x = 8;
        else if (ItemScale == 4.f)
            ItemPos.x = 0;
        else if (ItemScale == 5.f)
            ItemPos.x = -8;
        else if (ItemScale == 6.f)
            ItemPos.x = -16;
        else 
            ItemPos.x = 0;
    }
}

void Prop::Tick(float DeltaTime, Background& Map)
{
    if (Visible) {
        // Play NPC idle animation when NOT interacting
        if ((Type == PropType::NPC_A || Type == PropType::NPC_B || Type == PropType::NPC_C) && !Talking)
        {
            Object.Tick(DeltaTime);

            // Update any progression and triggers for NPCs
            if (CurrentAct != Progress::ACT_O)
            {
                if (Type == CurrentNPC)
                {
                    Act = CurrentAct;
                    CurrentAct = Progress::ACT_O;
                    CurrentNPC = PropType::NPC_O;
                }
            }
        }

        // Tick through Altar piece sprite animation once inserted
        if (Type == PropType::ANIMATEDALTAR)
        {
            for (auto& Piece:AltarPieces) 
            {
                if (ItemName == std::get<0>(Piece) && std::get<2>(Piece) == true)
                {
                    Object.Tick(DeltaTime);
                }
            }
        }

        // determine behavior of prop when interacting with it
        if (Active)
        {
            if (Type == PropType::GRASS)
            {
                Object.Tick(DeltaTime);
            }
            else if (Type == PropType::TREASURE || Type == PropType::BIGTREASURE)
            {
                if (TriggerAct != Progress::ACT_O) 
                {
                    CurrentAct = TriggerAct;
                    CurrentNPC = TriggerNPC;
                }

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

                for (auto& Piece:AltarPieces)
                {
                    if (std::get<0>(Piece) == ItemName)
                    {
                        std::get<1>(Piece) = true;
                    }
                }
            }
            else if (Type == PropType::DOOR)
            {
                Opened = true;
            }
            else if ((Type == PropType::NPC_A || Type == PropType::NPC_B || Type == PropType::NPC_C))
            {
                if (TriggerAct != Progress::ACT_O) 
                {
                    CurrentAct = TriggerAct;
                    CurrentNPC = TriggerNPC;
                }

                Talking = true;
            }
            else if (Type == PropType::ANIMATEDALTAR)
            {   
                for (auto& Piece:AltarPieces)
                {
                    if (std::get<1>(Piece) == true)
                    {
                        std::get<2>(Piece) = true;
                        InsertPiece = true;
                    }
                    if (std::get<2>(Piece) == true && std::get<3>(Piece) == false)
                    {
                        std::get<3>(Piece) = true;
                        PiecesAdded++;
                    } 
                }
                
                if (PiecesAdded >= 6)
                {
                    FinalChest = true;
                }

                Opened = true;   
            }
        }
        else {
            // Draw treasure item for (UpdateTime * X) seconds
            if (Opening)
            {
                ReceiveItem = true;
                
                RunningTime += DeltaTime;
                if (RunningTime >= Object.UpdateTime * 10)
                {
                    Opening = false;
                    RunningTime = 0.f; 
                }
            }

        }
    }
    else {
        // Turn visibility on for BigTreasure when all 6 pieces have been inserted
        if (Type == PropType::BIGTREASURE)
        {
            if (PiecesAdded >= 6)
            {
                Spawned = true;
            }
        }
    }
}

void Prop::Draw(Vector2 CharacterWorldPos)
{
    Vector2 ScreenPos {Vector2Subtract(WorldPos, CharacterWorldPos)}; // Where the prop is drawn on the screen
    // Vector2 MaxItemDistance {0,-20};

    if (Spawned)
    {
        // Draw only if Prop is viewable in the screen frame
        if (WithinScreen(CharacterWorldPos))
        {
            Visible = true;

            // Only draw final chest if conditions are met
            if (Type == PropType::BIGTREASURE)
            {
                if (FinalChest) 
                {
                    DrawTexturePro(Object.Texture, Object.GetSourceRec(), Object.GetPosRec(ScreenPos, Scale), Vector2{}, 0.f, WHITE);
                }
            }
            // Draw the object
            else 
            {
                DrawTexturePro(Object.Texture, Object.GetSourceRec(), Object.GetPosRec(ScreenPos, Scale), Vector2{}, 0.f, WHITE);
            }

            // Draw the animated altar piece
            if (Type == PropType::ANIMATEDALTAR)
            {
                for (auto& Piece:AltarPieces)
                {
                    if (std::get<2>(Piece) == true)
                    {
                        if (std::get<0>(Piece) == ItemName)
                        {
                            DrawTextureEx(Object.Texture, WorldPos, 0.f, Scale, WHITE);
                        }
                    }
                }
            }
        }
        else
        {   
            Visible = false;

            // Once NPC_C has been interacted with and is offscreen, set to not draw anymore
            if (Type == PropType::NPC_C && Act == Progress::ACT_O)
            {
                // Spawned = false;
                WorldPos.x = 1283;
                WorldPos.y = 2859;
                Act = Progress::ACT_II;
            }
        }
    }
    
    // Draw Treasure Box Item
    if (Opening)
    {
        DrawTextureEx(Item, Vector2Add(ScreenPos, ItemPos), 0.f, ItemScale, WHITE);
        ItemPos = Vector2Add(ItemPos, Vector2{0,-0.1f});
    }

    // Treasure Speech Box
    if (ReceiveItem)
    {
        // DrawTextureEx(SpeechBox, Vector2{352,518}, 0.f, 12.f, WHITE);
        DrawTextureEx(GameTextures->SpeechBox, Vector2{472,574}, 0.f, 8.f, WHITE);
        DrawSpeech();
    }

    // NPC Speech Box
    if (Talking)
    {
        DrawTextureEx(GameTextures->SpeechName, Vector2{376,438}, 0.f, 5.f, WHITE);
        DrawTextureEx(GameTextures->SpeechBox, Vector2{352,518}, 0.f, 12.f, WHITE);

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
        else if (Type == PropType::NPC_C)
        {
            DrawText("Louie", 399, 490, 30, WHITE);
            DrawSpeech();
        }
    }

    // Altar Pieces Inserted
    if (InsertPiece)
    {
        DrawTextureEx(GameTextures->SpeechBox, Vector2{472,574}, 0.f, 8.f, WHITE);
        DrawSpeech();
    }

    // CheckActivity(ScreenPos);
}

bool Prop::WithinScreen(Vector2 CharacterWorldPos)
{
    if (
        (WorldPos.x >= (CharacterWorldPos.x + 615) - (GetScreenWidth()/2 + (Object.Texture.width * Scale))) && 
        (WorldPos.x <= (CharacterWorldPos.x + 615) + (GetScreenWidth()/2 + (Object.Texture.width * Scale))) &&
        (WorldPos.y >= (CharacterWorldPos.y + 335) - (GetScreenHeight()/2 + (Object.Texture.height * Scale))) && 
        (WorldPos.y <= (CharacterWorldPos.y + 335) + (GetScreenHeight()/2 + (Object.Texture.height * Scale)))
       ) {
            return true;
         }
    else {
        return false;
    }
}

Rectangle Prop::GetCollisionRec(Vector2 CharacterWorldPos)
{
    Vector2 ScreenPos {Vector2Subtract(WorldPos, CharacterWorldPos)};

    switch (Type)
    {
        case PropType::ALTAR:
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
        case PropType::ANIMATEDALTAR:
        {
            return Rectangle
            {
                ScreenPos.x,
                ScreenPos.y,
                (Object.Texture.width * Scale) / Object.MaxFramesX,
                (Object.Texture.height * Scale) / Object.MaxFramesY
            };
            break;
        }
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
        case PropType::BIGTREASURE:
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
        case PropType::NPC_C:
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
        case PropType::BIGTREASURE:
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
        case PropType::NPC_C:
        {
            return Rectangle
            {
                ScreenPos.x - (Object.Texture.width * Scale)/4.f * .45f,
                ScreenPos.y - (Object.Texture.height * Scale) * .45f,
                (Object.Texture.width * Scale)/4.f + (Object.Texture.width * Scale)/4.f,
                (Object.Texture.height * Scale) + (Object.Texture.height * Scale)
            };
        }
        case PropType::ALTAR:
        {
            return Rectangle
            {
                ScreenPos.x - (Object.Texture.width * Scale) * .10f,
                ScreenPos.y - (Object.Texture.height * Scale) * .10f,
                (Object.Texture.width * Scale) + (Object.Texture.width * Scale) * .20f,
                (Object.Texture.height * Scale) + (Object.Texture.height * Scale) * .20f
            };
            break;
        }
        case PropType::ANIMATEDALTAR:
        {
            return Rectangle
            {
                ScreenPos.x - ((Object.Texture.width * Scale) / Object.MaxFramesX) * .10f,
                ScreenPos.y - (Object.Texture.height * Scale) * .10f,
                ((Object.Texture.width * Scale) / Object.MaxFramesX) + ((Object.Texture.width * Scale) / Object.MaxFramesX) * .20f,
                ((Object.Texture.height * Scale) / Object.MaxFramesY) + ((Object.Texture.height * Scale) / Object.MaxFramesY) * .20f
            };
            break;
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

bool Prop::CheckMovement(Background& Map, Vector2 CharWorldPos, Vector2 Direction, float Speed, std::vector<std::vector<Prop>>& Props)
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
        WorldPos.x + (Object.Texture.width * Scale) > Map.GetMapSize().x * Map.GetScale() ||
        WorldPos.y + (Object.Texture.height * Scale) > Map.GetMapSize().y * Map.GetScale())
    {
        OutOfBounds = true;
        UndoMovement();
    }
    else
    {
        OutOfBounds = false;
    }

    for (auto& Proptype:Props) {
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
    if (Type == PropType::TREASURE)
    {
        DrawText("", 510, 550, 20, WHITE);
        DrawText("", 510, 575, 20, WHITE);
        DrawText(TextFormat("Received: %s!", ItemName.c_str()), 490, 625, 20, WHITE);
        DrawText("", 510, 625, 20, WHITE);
        DrawText("", 510, 650, 20, WHITE);
        DrawText("                                               (ENTER to Continue)", 390, 675, 16, WHITE);

        if (IsKeyPressed(KEY_ENTER))
        {
            ReceiveItem = false;
        }
    }
    else if (Type == PropType::BIGTREASURE)
    {
        DrawText(TextFormat("Received: %s!", ItemName.c_str()), 510, 600, 20, WHITE);
        DrawText("Diana is written on the bottom...", 510, 625, 20, WHITE);
        DrawText("Bring it back to her!", 510, 650, 20, WHITE);
        DrawText("                                               (ENTER to Continue)", 390, 675, 16, WHITE);

        if (IsKeyPressed(KEY_ENTER))
        {
            ReceiveItem = false;
        }
    }
    else if (Type == PropType::ANIMATEDALTAR)
    {
        if (PiecesAdded <= 0) // This statement is not possible. DrawSpeech() isn't called until an Altar piece is inserted
        {
            DrawText("A mysterious altar...", 490, 600, 20, WHITE);
            DrawText("You feel a strange power resonating", 490, 625, 20, WHITE);
            DrawText("from the empty engravings...", 490, 650, 20, WHITE);
            DrawText("                                               (ENTER to Continue)", 390, 675, 16, WHITE);
        }
        else if (PiecesAdded > 0 && PiecesAdded < 6)
        {
            DrawText("", 510, 550, 20, WHITE);
            DrawText("", 510, 575, 20, WHITE);
            DrawText("Altar piece inserted!", 510, 625, 20, WHITE);
            DrawText("", 510, 625, 20, WHITE);
            DrawText("", 510, 650, 20, WHITE);
            DrawText("                                               (ENTER to Continue)", 390, 675, 16, WHITE);
        }
        else
        {
            DrawText("All pieces have been collected!", 490, 600, 20, WHITE);
            DrawText("You hear sounds coming from", 490, 625, 20, WHITE);
            DrawText("the middle of the forest...", 490, 650, 20, WHITE);
            DrawText("", 510, 675, 20, WHITE);
            DrawText("                                               (ENTER to Continue)", 390, 675, 16, WHITE);
        }

        if (IsKeyPressed(KEY_ENTER))
        {
            InsertPiece = false;
        }
    }
    // Manage NPC speech
    else
    {
        switch(Act)
        {
            case Progress::ACT_O:
            {
                if (Type == PropType::NPC_C)
                {
                    DrawText("...Mom told you to find me?...", 390, 550, 20, WHITE);
                    DrawText("She said that I keep wandering off???", 390, 575, 20, WHITE);
                    DrawText("SHES the one who left ME here!!", 390, 600, 20, WHITE);
                    DrawText("...Well, thank you for finding me...", 390, 625, 20, WHITE);
                    DrawText("", 390, 650, 20, WHITE);
                    DrawText("                                                         (ENTER to Continue)", 390, 675, 16, WHITE);
                }

                if (IsKeyReleased(KEY_ENTER))
                {
                    Opened = true;
                    Talking = false;
                }
                break;
            }
            case Progress::ACT_I:
            {   
                if (Type == PropType::NPC_A)
                {
                    DrawText("Hello there, little Foxy! You look a little lost.", 390, 550, 20, WHITE);
                    DrawText("Have you ran into my neighbor, Jade? I know she", 390, 575, 20, WHITE);
                    DrawText("can be noisy, but she means no harm...", 390, 600, 20, WHITE);
                    DrawText("", 390, 625, 20, WHITE);
                    DrawText("", 390, 650, 20, WHITE);
                    DrawText("                                                         (ENTER to Continue)", 390, 675, 16, WHITE);
                }
                else if (Type == PropType::NPC_B)
                {
                    DrawText("Why HELLO, Love! Have you seen my little one?", 390, 550, 20, WHITE);
                    DrawText("Could have sworn he was right here...", 390, 575, 20, WHITE);
                    DrawText("I really hope he didn't wander into the forest", 390, 600, 20, WHITE);
                    DrawText("AGAIN!!", 390, 625, 20, WHITE);
                    DrawText("                                                         (ENTER to Continue)", 390, 675, 16, WHITE);
                }
                else
                {
                    DrawText("...Mom told you to find me?...", 390, 550, 20, WHITE);
                    DrawText("She said that I keep wandering off???", 390, 575, 20, WHITE);
                    DrawText("SHES the one who left ME here!!", 390, 600, 20, WHITE);
                    DrawText("...Well, thank you for finding me...", 390, 625, 20, WHITE);
                    DrawText("", 390, 650, 20, WHITE);
                    DrawText("                                                         (ENTER to Continue)", 390, 675, 16, WHITE);
                };

                if (IsKeyReleased(KEY_ENTER))
                {
                    if (Type != PropType::NPC_A)
                    {
                        Opened = true;
                        Talking = false;
                    }

                    // Update NPC_C's act to ACT_O as it doesn't need to be seen anymore once it leaves screen
                    if (Type == PropType::NPC_A)
                    {
                        Act = Progress::ACT_II;
                    }
                    else if (Type == PropType::NPC_C)
                    {
                        Act = Progress::ACT_O;
                    }
                }
                break;
            }
            case Progress::ACT_II:
            {
                if (Type == PropType::NPC_A)
                {
                    DrawText("By the way, do you live in the flower forest", 390, 550, 20, WHITE);
                    DrawText("WEST of here? There's a strange stone", 390, 575, 20, WHITE);
                    DrawText("monument NORTH of that location...", 390, 600, 20, WHITE);
                    DrawText("People have been wondering what it is", 390, 625, 20, WHITE);
                    DrawText("but no one really knows...", 390, 650, 20, WHITE);
                    DrawText("                                                         (ENTER to Continue)", 390, 675, 16, WHITE);   
                }
                else if (Type == PropType::NPC_B)
                {
                    DrawText("You found my boy! He's alawys wandering", 390, 550, 20, WHITE);
                    DrawText("off and getting in trouble... This time", 390, 575, 20, WHITE);
                    DrawText("the FOREST!! Thank you for finding him.", 390, 600, 20, WHITE);
                    DrawText("There are rumors of a hidden treasure out", 390, 625, 20, WHITE);
                    DrawText("there... maybe he was looking for it?", 390, 650, 20, WHITE);
                    DrawText("                                                         (ENTER to Continue)", 390, 675, 16, WHITE);
                }
                else
                {
                    DrawText("Hi, Foxy! Thanks again for your help! ", 390, 550, 20, WHITE);
                    DrawText("Mom is still blaming me about getting lost..", 390, 575, 20, WHITE);
                    DrawText("While I was lost, I think I saw a treasure", 390, 600, 20, WHITE);
                    DrawText("in the NORTH WEST region of the forest...", 390, 625, 20, WHITE);
                    DrawText("Might be worth checking out!", 390, 650, 20, WHITE);
                    DrawText("                                                         (ENTER to Continue)", 390, 675, 16, WHITE);
                };


                if (IsKeyPressed(KEY_ENTER))
                {
                    Opened = true;
                    Talking = false;

                    if (Type == PropType::NPC_A)
                    {
                        Act = Progress::ACT_I;
                    }
                }
                break;
            }
            case Progress::ACT_III:
            {
                if (Type == PropType::NPC_A)
                {
                    DrawText("", 390, 550, 20, WHITE);
                    DrawText("", 390, 575, 20, WHITE);
                    DrawText("", 390, 600, 20, WHITE);
                    DrawText("", 390, 625, 20, WHITE);
                    DrawText("", 390, 650, 20, WHITE);
                    DrawText("                                                         (ENTER to Continue)", 390, 675, 16, WHITE);
                }
                else if (Type == PropType::NPC_B) 
                {
                    DrawText("", 390, 550, 20, WHITE);
                    DrawText("", 390, 575, 20, WHITE);
                    DrawText("", 390, 600, 20, WHITE);
                    DrawText("", 390, 625, 20, WHITE);
                    DrawText("", 390, 650, 20, WHITE);
                    DrawText("                                                         (ENTER to Continue)", 390, 675, 16, WHITE);
                }
                else
                {
                    DrawText("", 390, 550, 20, WHITE);
                    DrawText("", 390, 575, 20, WHITE);
                    DrawText("", 390, 600, 20, WHITE);
                    DrawText("", 390, 625, 20, WHITE);
                    DrawText("", 390, 650, 20, WHITE);
                    DrawText("                                                         (ENTER to Continue)", 390, 675, 16, WHITE);
                };

                if (IsKeyPressed(KEY_ENTER))
                {
                    Opened = true;
                    Talking = false;
                }
                break;
            }
            case Progress::ACT_IV:
            {
                if (Type == PropType::NPC_A)
                {
                    DrawText("", 390, 550, 20, WHITE);
                    DrawText("", 390, 575, 20, WHITE);
                    DrawText("", 390, 600, 20, WHITE);
                    DrawText("", 390, 625, 20, WHITE);
                    DrawText("", 390, 650, 20, WHITE);
                    DrawText("                                                         (ENTER to Continue)", 390, 675, 16, WHITE); 
                }
                else if (Type == PropType::NPC_B)
                {
                    DrawText("", 390, 550, 20, WHITE);
                    DrawText("", 390, 575, 20, WHITE);
                    DrawText("", 390, 600, 20, WHITE);
                    DrawText("", 390, 625, 20, WHITE);
                    DrawText("", 390, 650, 20, WHITE);
                    DrawText("                                                         (ENTER to Continue)", 390, 675, 16, WHITE);
                }
                else
                {
                    DrawText("", 390, 550, 20, WHITE);
                    DrawText("", 390, 575, 20, WHITE);
                    DrawText("", 390, 600, 20, WHITE);
                    DrawText("", 390, 625, 20, WHITE);
                    DrawText("", 390, 650, 20, WHITE);
                    DrawText("                                                         (ENTER to Continue)", 390, 675, 16, WHITE);
                };
            
                if (IsKeyPressed(KEY_ENTER))
                {
                    Opened = true;
                    Talking = false;
                }
                break;
            }
            default:
                Opened = true;
                Talking = false;
                break;
        }
    }
}

// ---------------------------------------------------------------------

Props::Props(std::vector<std::vector<Prop>> Under, std::vector<std::vector<Prop>> Over)
    : Under{Under}, Over{Over}
{

}