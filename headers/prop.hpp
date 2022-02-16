#ifndef PROP_HPP
#define PROP_HPP

#include <raylib.h>
#include <raymath.h>
#include <vector>
#include <string>
#include "headers/sprite.hpp"
#include "headers/background.hpp"

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
    DUNGEON, DUNGEONLEFT, DUNGEONRIGHT,
    ALTAR,
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
private:
    Sprite Object{};
    PropType Type{};
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

    // NPC variables
    Progress Act{Progress::ACT_I};
    bool Visible{true};
    Texture2D SpeechBox{LoadTexture("sprites/npc/SpeechBox.png")};
    Texture2D SpeechName{LoadTexture("sprites/npc/SpeechName.png")};

    // Progress Trigger Variables
    Progress TriggerAct{};
    PropType TriggerNPC{};
    static inline Progress CurrentAct{Progress::ACT_O};
    static inline PropType CurrentNPC{PropType::NPC_O};

    // Treasure member variables
    bool ReceiveItem{false};
    Texture2D Item{};
    std::string ItemName{};
    float ItemScale{};
    Vector2 ItemPos{};
    float RunningTime{};

public:
    Prop(const char* TexturePath, Vector2 Pos, PropType Type, float Scale = 4.f, bool Moveable = false, bool Interactable = false);
    Prop(Sprite Object, Vector2 Pos, PropType Type, float Scale = 4.f, bool Moveable = false, bool Interactable = false, 
         Progress Act = Progress::ACT_O, PropType NPC = PropType::NPC_O, Texture2D Item = LoadTexture("sprites/props/Heart.png"), 
         std::string ItemName = "None", float ItemScale = 2.f);           
    
    void Tick(float DeltaTime, Background& Map);
    void Draw(Vector2 CharacterWorldPos);

    bool HasCollision() {return Collidable;}
    bool IsInteractable() {return Interactable;}
    bool IsMoveable() {return Moveable;}
    bool IsOutOfBounds() {return OutOfBounds;}
    bool IsActive() {return Active;}
    bool IsOpened() {return Opened;}
    bool IsVisible() {return Visible;}
    
    Vector2 GetWorldPos() {return WorldPos;}
    PropType GetType() {return Type;}
    Rectangle GetCollisionRec(Vector2 CharacterWorldPos);
    Rectangle GetInteractRec(Vector2 CharacterWorldPos);
    
    void SetWorldPos(Vector2 Direction);
    void SetActive(bool Input) {Active = Input;}
    void SetOpened(bool Input) {Opened = Input;}
    void UndoMovement() {WorldPos = PrevWorldPos;}
    bool CheckMovement(Background& Map, Vector2 CharWorldPos, Vector2 Direction, float Speed, std::vector<std::vector<Prop>>* Props);
    void CheckActivity(Vector2 ScreenPos);

    void DrawSpeech();
};

struct Props
{
    std::vector<std::vector<Prop>>* Under{};
    std::vector<std::vector<Prop>>* Over{};

    Props(std::vector<std::vector<Prop>>* Under, std::vector<std::vector<Prop>>* Over);
};

#endif