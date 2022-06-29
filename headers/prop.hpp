#ifndef PROP_HPP
#define PROP_HPP

#include <vector>
#include <string>
#include <tuple>
#include <unordered_map>
#include "sprite.hpp"
#include "background.hpp"

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
    NPC_DIANA,
    NPC_JADE,
    NPC_SON,
    NPC_RUMBY,
    PLACEHOLDER
};

enum class Progress
{
    ACT_O, ACT_I, ACT_II, ACT_III, ACT_IV, ACT_V, ACT_VI, ACT_VII
};

struct Props;

class Prop 
{
public:
    // Constructor for inanimate props
    Prop(const Texture2D& Texture, 
         const Vector2 Pos, 
         const PropType Type, 
         const GameTexture& GameTextures, 
         const float Scale = 4.f, 
         const bool Moveable = false, 
         const bool Interactable = false);

    // Constructor for animated altar pieces
    Prop(const Sprite& Object, 
         const Vector2 Pos, 
         const PropType Type, 
         const GameTexture& GameTextures, 
         const std::string& ItemName = "None", 
         const bool Spawned = false, 
         const bool Interactable = false);      

    // Constructor for animated props
    Prop(const Sprite& Object, 
         const Vector2 Pos, 
         const PropType Type, 
         const GameTexture& GameTextures, 
         const Texture2D& Item, 
         const float Scale = 4.f, 
         const bool Moveable = false, 
         const bool Interactable = false, 
         const Progress TriggerAct = Progress::ACT_O, 
         const PropType TriggerNPC = PropType::NPC_O, 
         const bool Spawn = true,
         const std::string& ItemName = "None", 
         const float ItemScale = 2.f);
    
    void Tick(const float DeltaTime);
    void Draw(const Vector2 CharacterWorldPos);

    void NpcTick(const float DeltaTime);
    void AltarTick(const float DeltaTime);
    void OpenChest(const float DeltaTime);
    void TreasureTick(const float DeltaTime);
    void CheckVisibility(const Vector2 CharacterWorldPos);
    void InsertAltarPiece();
    void TalkToNpc();
    void UpdateNpcPos();
    void UpdateNpcActs();
    void CheckFinalChest();

    constexpr bool HasCollision() const {return Collidable;}
    constexpr bool IsInteractable() const {return Interactable;}
    constexpr bool IsMoveable() const {return Moveable;}
    constexpr bool IsOutOfBounds() const {return OutOfBounds;}
    constexpr bool IsActive() const {return Active;}
    constexpr bool IsOpened() const {return Opened;}
    constexpr bool IsSpawned() const {return Spawned;}
    bool WithinScreen(const Vector2 CharacterWorldPos);
    
    constexpr Vector2 GetWorldPos() const {return WorldPos;}
    constexpr PropType GetType() const {return Type;}
    std::string GetItemName() const {return ItemName;}
    Rectangle GetCollisionRec(const Vector2 CharacterWorldPos);
    Rectangle GetInteractRec(const Vector2 CharacterWorldPos);
    
    void SetActive(const bool Input) {Active = Input;}
    void SetOpened(const bool Input) {Opened = Input;}
    void SetSpawned(const bool Input) {Spawned = Input;}
    void UndoMovement() {WorldPos = PrevWorldPos;}
    bool CheckMovement(Background& Map, const Vector2 CharWorldPos, const Vector2 Direction, const float Speed, std::vector<std::vector<Prop>>& Props);

    void DrawPropText();
    void DrawSpeech();
    
private:
    Sprite Object{};
    const PropType Type{};
    const GameTexture& GameTextures;
    Vector2 WorldPos{};
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
    bool Reading{false};
    bool ReadyToProgress{false};

    // NPC variables
    Progress Act{Progress::ACT_I};
    Progress TriggerAct{};
    PropType TriggerNPC{};

    static inline std::unordered_map<PropType, std::pair<Progress, PropType>> QuestlineProgress
    {
        {PropType::NPC_DIANA, std::make_pair(Progress::ACT_O, PropType::NPC_O)},
        {PropType::NPC_JADE, std::make_pair(Progress::ACT_O, PropType::NPC_O)},
        {PropType::NPC_SON, std::make_pair(Progress::ACT_O, PropType::NPC_O)},
        {PropType::NPC_RUMBY, std::make_pair(Progress::ACT_O, PropType::NPC_O)},
        {PropType::NPC_O, std::make_pair(Progress::ACT_O, PropType::NPC_O)},
        {PropType::TREASURE, std::make_pair(Progress::ACT_O, PropType::NPC_O)},
    };

    // Treasure variables
    bool ReceiveItem{false};
    bool InsertPiece{false};
    const Texture2D Item{};
    const std::string ItemName{};
    float ItemScale{};
    Vector2 ItemPos{};
    float RunningTime{};
    static inline int PiecesReceived{0};
    static inline int PiecesAdded{0};
    static inline bool CryptexReceived{false};
    static inline bool FinalChestKey{false};
    static inline bool FinalChestSpawned{false};
    static inline bool FirstPieceInserted{false};

    static inline std::vector<std::tuple<std::string, bool, bool, bool>> AltarPieces {
            // {"ItemName", ItemUnlocked, ItemVisible, ItemInserted}
            {"Top Left Altar Piece", false, false, false}, 
            {"Top Altar Piece", false, false, false}, 
            {"Top Right Altar Piece", false, false, false}, 
            {"Bottom Left Altar Piece", false, false, false},
            {"Bottom Altar Piece", false, false, false}, 
            {"Bottom Right Altar Piece", false, false, false}
    };
};

struct Props
{
    Props(const std::vector<std::vector<Prop>>& Under, const std::vector<std::vector<Prop>>& Over);

    std::vector<std::vector<Prop>> Under{};
    std::vector<std::vector<Prop>> Over{};
};

#endif // PROP_HPP