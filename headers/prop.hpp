#ifndef PROP_HPP
#define PROP_HPP

#include <raylib.h>
#include <raymath.h>
#include <vector>
#include <string>
#include <tuple>
#include "sprite.hpp"
#include "background.hpp"
#include "gametextures.hpp"

enum class PropType
{
    TREE, 
    ROCK, 
    BOULDER, 
    GRASS, 
    FLOWER,
    rHOUSELEFT, rHOUSERIGHT, DOOR,
    bHOUSELEFT, bHOUSERIGHT,
    FENCE, 
    TOPWALL, LEFTSIDEWALL, RIGHTSIDEWALL, BOTTOMWALL, 
    BUSH, 
    STUMP, 
    HOLE,
    TREASURE,
    BIGTREASURE, 
    DUNGEON, DUNGEONLEFT, DUNGEONRIGHT,
    ALTAR,
    ANIMATEDALTAR,
    NPC_O,
    NPC_A,
    NPC_B,
    NPC_C,
    PLACEHOLDER
};

enum class Progress
{
    ACT_O, ACT_I, ACT_II, ACT_III, ACT_IV, ACT_V
};

struct Props;

class Prop 
{
public:
    Prop(const Texture2D& Texture, Vector2 Pos, PropType Type, GameTexture& GameTextures, float Scale = 4.f, bool Moveable = false, bool Interactable = false);
    Prop(const Sprite& Object, Vector2 Pos, PropType Type, GameTexture& GameTextures, const std::string& ItemName = "None", bool Spawned = false, bool Interactable = false);           
    Prop(const Sprite& Object, Vector2 Pos, PropType Type, GameTexture& GameTextures, const Texture2D& Item, float Scale = 4.f, bool Moveable = false, bool Interactable = false, 
         Progress Act = Progress::ACT_O, PropType NPC = PropType::NPC_O, 
         const std::string& ItemName = "None", float ItemScale = 2.f);
    
    void Tick(float DeltaTime);
    void Draw(Vector2 CharacterWorldPos);

    bool HasCollision() {return Collidable;}
    bool IsInteractable() {return Interactable;}
    bool IsMoveable() {return Moveable;}
    bool IsOutOfBounds() {return OutOfBounds;}
    bool IsActive() {return Active;}
    bool IsOpened() {return Opened;}
    bool IsSpawned() {return Spawned;}
    bool WithinScreen(Vector2 CharacterWorldPos);
    
    Vector2 GetWorldPos() {return WorldPos;}
    PropType GetType() {return Type;}
    Rectangle GetCollisionRec(Vector2 CharacterWorldPos);
    Rectangle GetInteractRec(Vector2 CharacterWorldPos);
    
    void SetActive(bool Input) {Active = Input;}
    void SetOpened(bool Input) {Opened = Input;}
    void UndoMovement() {WorldPos = PrevWorldPos;}
    bool CheckMovement(Background& Map, Vector2 CharWorldPos, Vector2 Direction, float Speed, std::vector<std::vector<Prop>>& Props);

    void DrawSpeech();
    
private:
    Sprite Object{};
    PropType Type{};
    GameTexture* GameTextures{};
    Vector2 WorldPos{};     // Where the prop is located in the world
    Vector2 PrevWorldPos{};
    float Scale {4.f};
    bool Opened{false};
    bool Opening{false};
    bool Active{false};
    bool Collidable{false};
    bool Interactable{false};
    bool Moveable{false};
    bool OutOfBounds{false};
    bool Talking{false};
    bool Visible{false};
    bool Spawned{true};

    // NPC variables
    Progress Act{Progress::ACT_I};

    // Progress Trigger Variables
    Progress TriggerAct{};
    PropType TriggerNPC{};
    static inline Progress CurrentAct{Progress::ACT_O};
    static inline PropType CurrentNPC{PropType::NPC_O};

    // Treasure member variables
    bool ReceiveItem{false};
    const Texture2D Item{};
    std::string ItemName{};
    float ItemScale{};
    Vector2 ItemPos{};
    float RunningTime{};

    // {"ItemName", ItemUnlocked, ItemVisible, ItemInserted}
    static inline std::vector<std::tuple<std::string, bool, bool, bool>> AltarPieces {
            {"Top Left Altar Piece", false, false, false}, 
            {"Top Altar Piece", false, false, false}, 
            {"Top Right Altar Piece", false, false, false}, 
            {"Bottom Left Altar Piece", false, false, false},
            {"Bottom Altar Piece", false, false, false}, 
            {"Bottom Right Altar Piece", false, false, false}
    };
    static inline int PiecesAdded{0};
    static inline bool FinalChest{false};                                                               
    bool InsertPiece{false};   
};

struct Props
{
    Props(const std::vector<std::vector<Prop>>& Under, const std::vector<std::vector<Prop>>& Over);

    std::vector<std::vector<Prop>> Under{};
    std::vector<std::vector<Prop>> Over{};
};

#endif // PROP_HPP