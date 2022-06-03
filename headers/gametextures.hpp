#ifndef GAMETEXTURES_HPP
#define GAMETEXTURES_HPP

#include <raylib.h>
#include "headers/prop.hpp"

struct GameTexture {
    // Texture2D GetTexture(PropType Type);
public:
    GameTexture() = default;
    ~GameTexture();
    GameTexture(const GameTexture&) = delete;
    GameTexture(GameTexture&&) = default;
    GameTexture& operator=(const GameTexture&) = delete;
    GameTexture& operator=(GameTexture&&) = default;

    const Texture2D AltarBot{LoadTexture("sprites/props/AltarBot.png")};
    const Texture2D AltarBotAnimated{LoadTexture("sprites/props/AltarBotAnimated.png")};
    const Texture2D AltarBotLeft{LoadTexture("sprites/props/AltarBotLeft.png")};
    const Texture2D AltarBotLeftAnimated{LoadTexture("sprites/props/AltarBotLeftAnimated.png")};
    const Texture2D AltarBotRight{LoadTexture("sprites/props/AltarBotRight.png")};
    const Texture2D AltarBotRightAnimated{LoadTexture("sprites/props/AltarBotRightAnimated.png")};
    const Texture2D AltarDormant{LoadTexture("sprites/props/AltarDormant.png")};
    const Texture2D AltarDormantRust{LoadTexture("sprites/props/AltarDormantRust.png")};
    const Texture2D AltarTop{LoadTexture("sprites/props/AltarTop.png")};
    const Texture2D AltarTopAnimated{LoadTexture("sprites/props/AltarTopAnimated.png")};
    const Texture2D AltarTopLeft{LoadTexture("sprites/props/AltarTopLeft.png")};
    const Texture2D AltarTopLeftAnimated{LoadTexture("sprites/props/AltarTopLeftAnimated.png")};
    const Texture2D AltarTopRight{LoadTexture("sprites/props/AltarTopRight.png")};
    const Texture2D AltarTopRightAnimated{LoadTexture("sprites/props/AltarTopRightAnimated.png")};

    const Texture2D AxeStump{LoadTexture("sprites/props/AxeStump.png")};
    const Texture2D BigGrass{LoadTexture("sprites/props/BigGrass.png")};
    const Texture2D Boulder{LoadTexture("sprites/props/Boulder.png")};

    const Texture2D BridgeHorizontal{LoadTexture("sprites/props/BridgeHorizontal.png")};
    const Texture2D BridgeRopeBottom{LoadTexture("sprites/props/BridgeRopeBottom.png")};
    const Texture2D BridgeRopeLeft{LoadTexture("sprites/props/BridgeRopeLeft.png")};
    const Texture2D BridgeRopeRight{LoadTexture("sprites/props/BridgeRopeRight.png")};
    const Texture2D BridgeRopeTop{LoadTexture("sprites/props/BridgeRopeTop.png")};
    const Texture2D BridgeVertical{LoadTexture("sprites/props/BridgeVertical.png")};

    const Texture2D Bush{LoadTexture("sprites/props/Bush.png")};
    const Texture2D ClayPot{LoadTexture("sprites/props/ClayPot.png")};

    const Texture2D Door{LoadTexture("sprites/props/Door.png")};
    const Texture2D DoorBlue{LoadTexture("sprites/props/DoorBlue.png")};
    const Texture2D DoorRed{LoadTexture("sprites/props/DoorRed.png")};

    const Texture2D DungeonEntrance{LoadTexture("sprites/props/DungeonEntrance.png")};

    const Texture2D FenceDown{LoadTexture("sprites/props/FenceDown.png")};
    const Texture2D FenceLeft{LoadTexture("sprites/props/FenceLeft.png")};
    const Texture2D FenceRight{LoadTexture("sprites/props/FenceRight.png")};
    const Texture2D FenceUp{LoadTexture("sprites/props/FenceUp.png")};

    const Texture2D FlowerBush{LoadTexture("sprites/props/FlowerBush.png")};
    const Texture2D Flowers{LoadTexture("sprites/props/Flowers.png")};

    const Texture2D GrassAnimation{LoadTexture("sprites/props/GrassAnimation.png")};

    const Texture2D GrassWallBotLeft{LoadTexture("sprites/props/GrassWallBotLeft.png")};
    const Texture2D GrassWallBotRight{LoadTexture("sprites/props/GrassWallBotRight.png")};
    const Texture2D GrassWallBottom{LoadTexture("sprites/props/GrassWallBottom.png")};
    const Texture2D GrassWallInnerLeft{LoadTexture("sprites/props/GrassWallInnerLeft.png")};
    const Texture2D GrassWallInnerRight{LoadTexture("sprites/props/GrassWallInnerRight.png")};
    const Texture2D GrassWallLeft{LoadTexture("sprites/props/GrassWallLeft.png")};
    const Texture2D GrassWallRight{LoadTexture("sprites/props/GrassWallRight.png")};
    const Texture2D GrassWallTop{LoadTexture("sprites/props/GrassWallTop.png")};
    const Texture2D GrassWallTopLeft{LoadTexture("sprites/props/GrassWallTopLeft.png")};
    const Texture2D GrassWallTopRight{LoadTexture("sprites/props/GrassWallTopRight.png")};

    const Texture2D Hole{LoadTexture("sprites/props/Hole.png")};

    const Texture2D HouseBlue{LoadTexture("sprites/props/HouseBlue.png")};
    const Texture2D HouseRed{LoadTexture("sprites/props/HouseRed.png")};

    const Texture2D Interact{LoadTexture("sprites/props/Interact.png")};

    const Texture2D LifebarLeftEmpty{LoadTexture("sprites/enemies/lifebar/round_left_empty.png")};
    const Texture2D LifebarLeftFilled{LoadTexture("sprites/enemies/lifebar/round_left_filled.png")};
    const Texture2D LifebarMiddleEmpty{LoadTexture("sprites/enemies/lifebar/round_middle_empty.png")};
    const Texture2D LifebarMiddleFilled{LoadTexture("sprites/enemies/lifebar/round_middle_filled.png")};
    const Texture2D LifebarRightEmpty{LoadTexture("sprites/enemies/lifebar/round_right_empty.png")};
    const Texture2D LifebarRightFilled{LoadTexture("sprites/enemies/lifebar/round_right_filled.png")};

    const Texture2D LargeRocks{LoadTexture("sprites/props/LargeRocks.png")};
    const Texture2D LittleGrass{LoadTexture("sprites/props/LittleGrass.png")};
    const Texture2D LittleRocks{LoadTexture("sprites/props/LittleRocks.png")};
    const Texture2D MediumRocks{LoadTexture("sprites/props/MediumRocks.png")};
    const Texture2D Placeholder{LoadTexture("sprites/placeholder/placeholder.png")};
    const Texture2D RockStump{LoadTexture("sprites/props/RockStump.png")};

    const Texture2D Sapling{LoadTexture("sprites/props/Sapling.png")};
    const Texture2D Sign{LoadTexture("sprites/props/Sign.png")};
    const Texture2D SpeechBox{LoadTexture("sprites/npc/SpeechBox.png")};
    const Texture2D SpeechName{LoadTexture("sprites/npc/SpeechName.png")};
    const Texture2D SquareContainer{LoadTexture("sprites/props/SquareContainer.png")};
    const Texture2D Stones{LoadTexture("sprites/props/Stones.png")};

    const Texture2D TransparentContainer{LoadTexture("sprites/props/TransparentContainer.png")};
    const Texture2D TransparentSquare{LoadTexture("sprites/props/TransparentSquare.png")};

    const Texture2D TreasureChest{LoadTexture("sprites/props/TreasureChest.png")};
    const Texture2D TreasureChestBig{LoadTexture("sprites/props/TreasureChestBig.png")};
    const Texture2D TreasureHeart{LoadTexture("sprites/props/TreasureHeart.png")};

    const Texture2D TreeBlue{LoadTexture("sprites/props/TreeBlue.png")};
    const Texture2D TreeClear{LoadTexture("sprites/props/TreeClear.png")};
    const Texture2D TreeFall{LoadTexture("sprites/props/TreeFall.png")};
    const Texture2D TreeGreen{LoadTexture("sprites/props/TreeGreen.png")};
    const Texture2D TreePink{LoadTexture("sprites/props/TreePink.png")};
    const Texture2D TreeStump{LoadTexture("sprites/props/TreeStump.png")};

    const Texture2D UnderFlowersOne{LoadTexture("sprites/props/UnderFlowersOne.png")};
    const Texture2D UnderFlowersTwo{LoadTexture("sprites/props/UnderFlowersTwo.png")};
    const Texture2D UnderFlowersThree{LoadTexture("sprites/props/UnderFlowersThree.png")};

    const Texture2D WallBotLeft{LoadTexture("sprites/props/WallBotLeft.png")};
    const Texture2D WallBotRight{LoadTexture("sprites/props/WallBotRight.png")};
    const Texture2D WallBottom{LoadTexture("sprites/props/WallBottom.png")};
    const Texture2D WallLeft{LoadTexture("sprites/props/WallLeft.png")};
    const Texture2D WallRight{LoadTexture("sprites/props/WallRight.png")};
    const Texture2D WallTopLeft{LoadTexture("sprites/props/WallTopLeft.png")};
    const Texture2D WallTopRight{LoadTexture("sprites/props/WallTopRight.png")};

    const Texture2D Didi{LoadTexture("sprites/npc/Didi.png")};
    const Texture2D Jade{LoadTexture("sprites/npc/Jade.png")};
    const Texture2D Son{LoadTexture("sprites/npc/Son.png")};

    const Texture2D FoxIdle{LoadTexture("sprites/characters/fox/Fox_idle.png")};
    const Texture2D FoxWalk{LoadTexture("sprites/characters/fox/Fox_walk.png")};
    const Texture2D FoxRun{LoadTexture("sprites/characters/fox/Fox_run.png")};
    const Texture2D FoxMelee{LoadTexture("sprites/characters/fox/Fox_melee.png")};
    const Texture2D FoxHit{LoadTexture("sprites/characters/fox/Fox_hit.png")};
    const Texture2D FoxDie{LoadTexture("sprites/characters/fox/Fox_die.png")};
    const Texture2D FoxPush{LoadTexture("sprites/characters/fox/Fox_push.png")};
    const Texture2D FoxSleeping{LoadTexture("sprites/characters/fox/Fox_sleeping.png")};
    const Texture2D FoxItemGot{LoadTexture("sprites/characters/fox/Fox_itemGot.png")};
    
};

#endif // gametextures_hpp