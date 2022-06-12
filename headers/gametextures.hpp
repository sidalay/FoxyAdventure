#ifndef GAMETEXTURES_HPP
#define GAMETEXTURES_HPP

#include <raylib.h>

struct GameTexture {

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

    const Texture2D FoxPortraitAngry{LoadTexture("sprites/portraits/Fox_Angry.png")};
    const Texture2D FoxPortraitDead{LoadTexture("sprites/portraits/Fox_Dead.png")};
    const Texture2D FoxPortraitFrame{LoadTexture("sprites/portraits/Fox_Frame.png")};
    const Texture2D FoxPortraitHappy{LoadTexture("sprites/portraits/Fox_Happy.png")};
    const Texture2D FoxPortraitHurt{LoadTexture("sprites/portraits/Fox_Hurt.png")};
    const Texture2D FoxPortraitNervous{LoadTexture("sprites/portraits/Fox_Nervous.png")};
    const Texture2D FoxPortraitSad{LoadTexture("sprites/portraits/Fox_Sad.png")};
    const Texture2D FoxPortraitSleeping{LoadTexture("sprites/portraits/Fox_Sleeping.png")};

    const Texture2D HeartFull{LoadTexture("sprites/props/Heart.png")};
    const Texture2D HeartHalf{LoadTexture("sprites/props/HeartHalf.png")};
    const Texture2D HeartEmpty{LoadTexture("sprites/props/HeartEmpty.png")};

    const Texture2D ButtonW{LoadTexture("sprites/buttons/W.png")};
    const Texture2D ButtonA{LoadTexture("sprites/buttons/A.png")};
    const Texture2D ButtonS{LoadTexture("sprites/buttons/S.png")};
    const Texture2D ButtonD{LoadTexture("sprites/buttons/D.png")};
    const Texture2D ButtonL{LoadTexture("sprites/buttons/L.png")};
    const Texture2D ButtonM{LoadTexture("sprites/buttons/M.png")};
    const Texture2D Shift{LoadTexture("sprites/buttons/Shift.png")};
    const Texture2D Space{LoadTexture("sprites/buttons/Space.png")};
    const Texture2D Lmouse{LoadTexture("sprites/buttons/Lmouse.png")};

    const Texture2D Map{LoadTexture("sprites/maps/CodexMap.png")};
    const Texture2D MiniMap{LoadTexture("sprites/maps/MiniMap.png")};
    const Texture2D PauseBackground{LoadTexture("sprites/maps/PauseBackground.png")};
    
    const Texture2D BearBrownAttack{LoadTexture("sprites/enemies/bear/brown_attack.png")};
    const Texture2D BearBrownDeath{LoadTexture("sprites/enemies/bear/brown_death.png")};
    const Texture2D BearBrownHurt{LoadTexture("sprites/enemies/bear/brown_hurt.png")};
    const Texture2D BearBrownIdle{LoadTexture("sprites/enemies/bear/brown_idle.png")};
    const Texture2D BearBrownWalk{LoadTexture("sprites/enemies/bear/brown_walk.png")};
    const Texture2D BearGreyAttack{LoadTexture("sprites/enemies/bear/grey_attack.png")};
    const Texture2D BearGreyDeath{LoadTexture("sprites/enemies/bear/grey_death.png")};
    const Texture2D BearGreyHurt{LoadTexture("sprites/enemies/bear/grey_hurt.png")};
    const Texture2D BearGreyIdle{LoadTexture("sprites/enemies/bear/grey_idle.png")};
    const Texture2D BearGreyWalk{LoadTexture("sprites/enemies/bear/grey_walk.png")};
    const Texture2D BearLightBrownAttack{LoadTexture("sprites/enemies/bear/lightbrown_attack.png")};
    const Texture2D BearLightBrownDeath{LoadTexture("sprites/enemies/bear/lightbrown_death.png")};
    const Texture2D BearLightBrownHurt{LoadTexture("sprites/enemies/bear/lightbrown_hurt.png")};
    const Texture2D BearLightBrownIdle{LoadTexture("sprites/enemies/bear/lightbrown_idle.png")};
    const Texture2D BearLightBrownWalk{LoadTexture("sprites/enemies/bear/lightbrown_walk.png")};
    const Texture2D BearWhiteAttack{LoadTexture("sprites/enemies/bear/white_attack.png")};
    const Texture2D BearWhiteDeath{LoadTexture("sprites/enemies/bear/white_death.png")};
    const Texture2D BearWhiteHurt{LoadTexture("sprites/enemies/bear/white_hurt.png")};
    const Texture2D BearWhiteIdle{LoadTexture("sprites/enemies/bear/white_idle.png")};
    const Texture2D BearWhiteWalk{LoadTexture("sprites/enemies/bear/white_walk.png")};

    const Texture2D BeholderAquaAttack{LoadTexture("sprites/enemies/beholder/aqua_attack.png")};
    const Texture2D BeholderAquaDeath{LoadTexture("sprites/enemies/beholder/aqua_death.png")};
    const Texture2D BeholderAquaHurt{LoadTexture("sprites/enemies/beholder/aqua_hurt.png")};
    const Texture2D BeholderAquaIdle{LoadTexture("sprites/enemies/beholder/aqua_idle.png")};
    const Texture2D BeholderAquaProjectile{LoadTexture("sprites/enemies/beholder/aqua_projectile.png")};
    const Texture2D BeholderAquaWalk{LoadTexture("sprites/enemies/beholder/aqua_walk.png")};
    const Texture2D BeholderBlackAttack{LoadTexture("sprites/enemies/beholder/black_attack.png")};
    const Texture2D BeholderBlackDeath{LoadTexture("sprites/enemies/beholder/black_death.png")};
    const Texture2D BeholderBlackHurt{LoadTexture("sprites/enemies/beholder/black_hurt.png")};
    const Texture2D BeholderBlackIdle{LoadTexture("sprites/enemies/beholder/black_idle.png")};
    const Texture2D BeholderBlackProjectile{LoadTexture("sprites/enemies/beholder/black_projectile.png")};
    const Texture2D BeholderBlackWalk{LoadTexture("sprites/enemies/beholder/black_walk.png")};
    const Texture2D BeholderBlueAttack{LoadTexture("sprites/enemies/beholder/blue_attack.png")};
    const Texture2D BeholderBlueDeath{LoadTexture("sprites/enemies/beholder/blue_death.png")};
    const Texture2D BeholderBlueHurt{LoadTexture("sprites/enemies/beholder/blue_hurt.png")};
    const Texture2D BeholderBlueIdle{LoadTexture("sprites/enemies/beholder/blue_idle.png")};
    const Texture2D BeholderBlueProjectile{LoadTexture("sprites/enemies/beholder/blue_projectile.png")};
    const Texture2D BeholderBlueWalk{LoadTexture("sprites/enemies/beholder/blue_walk.png")};
    const Texture2D BeholderGreenAttack{LoadTexture("sprites/enemies/beholder/green_attack.png")};
    const Texture2D BeholderGreenDeath{LoadTexture("sprites/enemies/beholder/green_death.png")};
    const Texture2D BeholderGreenHurt{LoadTexture("sprites/enemies/beholder/green_hurt.png")};
    const Texture2D BeholderGreenIdle{LoadTexture("sprites/enemies/beholder/green_idle.png")};
    const Texture2D BeholderGreenProjectile{LoadTexture("sprites/enemies/beholder/green_projectile.png")};
    const Texture2D BeholderGreenWalk{LoadTexture("sprites/enemies/beholder/green_walk.png")};
    const Texture2D BeholderRedAttack{LoadTexture("sprites/enemies/beholder/red_attack.png")};
    const Texture2D BeholderRedDeath{LoadTexture("sprites/enemies/beholder/red_death.png")};
    const Texture2D BeholderRedHurt{LoadTexture("sprites/enemies/beholder/red_hurt.png")};
    const Texture2D BeholderRedIdle{LoadTexture("sprites/enemies/beholder/red_idle.png")};
    const Texture2D BeholderRedProjectile{LoadTexture("sprites/enemies/beholder/red_projectile.png")};
    const Texture2D BeholderRedWalk{LoadTexture("sprites/enemies/beholder/red_walk.png")};

    const Texture2D CreatureAquaAttack{LoadTexture("sprites/enemies/creature/aqua_attack.png")};
    const Texture2D CreatureAquaDeath{LoadTexture("sprites/enemies/creature/aqua_death.png")};
    const Texture2D CreatureAquaHurt{LoadTexture("sprites/enemies/creature/aqua_hurt.png")};
    const Texture2D CreatureAquaIdle{LoadTexture("sprites/enemies/creature/aqua_idle.png")};
    const Texture2D CreatureAquaWalk{LoadTexture("sprites/enemies/creature/aqua_walk.png")};
    const Texture2D CreatureGreyAttack{LoadTexture("sprites/enemies/creature/grey_attack.png")};
    const Texture2D CreatureGreyDeath{LoadTexture("sprites/enemies/creature/grey_death.png")};
    const Texture2D CreatureGreyHurt{LoadTexture("sprites/enemies/creature/grey_hurt.png")};
    const Texture2D CreatureGreyIdle{LoadTexture("sprites/enemies/creature/grey_idle.png")};
    const Texture2D CreatureGreyWalk{LoadTexture("sprites/enemies/creature/grey_walk.png")};
    const Texture2D CreatureOrangeAttack{LoadTexture("sprites/enemies/creature/orange_attack.png")};
    const Texture2D CreatureOrangeDeath{LoadTexture("sprites/enemies/creature/orange_death.png")};
    const Texture2D CreatureOrangeHurt{LoadTexture("sprites/enemies/creature/orange_hurt.png")};
    const Texture2D CreatureOrangeIdle{LoadTexture("sprites/enemies/creature/orange_idle.png")};
    const Texture2D CreatureOrangeWalk{LoadTexture("sprites/enemies/creature/orange_walk.png")};
    const Texture2D CreaturePurpleAttack{LoadTexture("sprites/enemies/creature/purple_attack.png")};
    const Texture2D CreaturePurpleDeath{LoadTexture("sprites/enemies/creature/purple_death.png")};
    const Texture2D CreaturePurpleHurt{LoadTexture("sprites/enemies/creature/purple_hurt.png")};
    const Texture2D CreaturePurpleIdle{LoadTexture("sprites/enemies/creature/purple_idle.png")};
    const Texture2D CreaturePurpleWalk{LoadTexture("sprites/enemies/creature/purple_walk.png")};
    const Texture2D CreatureRedAttack{LoadTexture("sprites/enemies/creature/red_attack.png")};
    const Texture2D CreatureRedDeath{LoadTexture("sprites/enemies/creature/red_death.png")};
    const Texture2D CreatureRedHurt{LoadTexture("sprites/enemies/creature/red_hurt.png")};
    const Texture2D CreatureRedIdle{LoadTexture("sprites/enemies/creature/red_idle.png")};
    const Texture2D CreatureRedWalk{LoadTexture("sprites/enemies/creature/red_walk.png")};

    const Texture2D GhostBlackAttack{LoadTexture("sprites/enemies/ghost/black_attack.png")};
    const Texture2D GhostBlackDeath{LoadTexture("sprites/enemies/ghost/black_death.png")};
    const Texture2D GhostBlackHurt{LoadTexture("sprites/enemies/ghost/black_hurt.png")};
    const Texture2D GhostBlackIdle{LoadTexture("sprites/enemies/ghost/black_idle.png")};
    const Texture2D GhostBlackWalk{LoadTexture("sprites/enemies/ghost/black_walk.png")};
    const Texture2D GhostBlueAttack{LoadTexture("sprites/enemies/ghost/blue_attack.png")};
    const Texture2D GhostBlueDeath{LoadTexture("sprites/enemies/ghost/blue_death.png")};
    const Texture2D GhostBlueHurt{LoadTexture("sprites/enemies/ghost/blue_hurt.png")};
    const Texture2D GhostBlueIdle{LoadTexture("sprites/enemies/ghost/blue_idle.png")};
    const Texture2D GhostBlueWalk{LoadTexture("sprites/enemies/ghost/blue_walk.png")};
    const Texture2D GhostGreenAttack{LoadTexture("sprites/enemies/ghost/green_attack.png")};
    const Texture2D GhostGreenDeath{LoadTexture("sprites/enemies/ghost/green_death.png")};
    const Texture2D GhostGreenHurt{LoadTexture("sprites/enemies/ghost/green_hurt.png")};
    const Texture2D GhostGreenIdle{LoadTexture("sprites/enemies/ghost/green_idle.png")};
    const Texture2D GhostGreenWalk{LoadTexture("sprites/enemies/ghost/green_walk.png")};
    const Texture2D GhostRedAttack{LoadTexture("sprites/enemies/ghost/red_attack.png")};
    const Texture2D GhostRedDeath{LoadTexture("sprites/enemies/ghost/red_death.png")};
    const Texture2D GhostRedHurt{LoadTexture("sprites/enemies/ghost/red_hurt.png")};
    const Texture2D GhostRedIdle{LoadTexture("sprites/enemies/ghost/red_idle.png")};
    const Texture2D GhostRedWalk{LoadTexture("sprites/enemies/ghost/red_walk.png")};
    const Texture2D GhostWhiteAttack{LoadTexture("sprites/enemies/ghost/white_attack.png")};
    const Texture2D GhostWhiteDeath{LoadTexture("sprites/enemies/ghost/white_death.png")};
    const Texture2D GhostWhiteHurt{LoadTexture("sprites/enemies/ghost/white_hurt.png")};
    const Texture2D GhostWhiteIdle{LoadTexture("sprites/enemies/ghost/white_idle.png")};
    const Texture2D GhostWhiteWalk{LoadTexture("sprites/enemies/ghost/white_walk.png")};
    const Texture2D GhostYellowAttack{LoadTexture("sprites/enemies/ghost/yellow_attack.png")};
    const Texture2D GhostYellowDeath{LoadTexture("sprites/enemies/ghost/yellow_death.png")};
    const Texture2D GhostYellowHurt{LoadTexture("sprites/enemies/ghost/yellow_hurt.png")};
    const Texture2D GhostYellowIdle{LoadTexture("sprites/enemies/ghost/yellow_idle.png")};
    const Texture2D GhostYellowWalk{LoadTexture("sprites/enemies/ghost/yellow_walk.png")};

    const Texture2D ImpBlueAttack{LoadTexture("sprites/enemies/imp/blue_attack.png")};
    const Texture2D ImpBlueDeath{LoadTexture("sprites/enemies/imp/blue_death.png")};
    const Texture2D ImpBlueHurt{LoadTexture("sprites/enemies/imp/blue_hurt.png")};
    const Texture2D ImpBlueIdle{LoadTexture("sprites/enemies/imp/blue_idle.png")};
    const Texture2D ImpBlueProjectile{LoadTexture("sprites/enemies/imp/blue_projectile.png")};
    const Texture2D ImpBlueWalk{LoadTexture("sprites/enemies/imp/blue_walk.png")};
    const Texture2D ImpGreenAttack{LoadTexture("sprites/enemies/imp/green_attack.png")};
    const Texture2D ImpGreenDeath{LoadTexture("sprites/enemies/imp/green_death.png")};
    const Texture2D ImpGreenHurt{LoadTexture("sprites/enemies/imp/green_hurt.png")};
    const Texture2D ImpGreenIdle{LoadTexture("sprites/enemies/imp/green_idle.png")};
    const Texture2D ImpGreenProjectile{LoadTexture("sprites/enemies/imp/green_projectile.png")};
    const Texture2D ImpGreenWalk{LoadTexture("sprites/enemies/imp/green_walk.png")};
    const Texture2D ImpGreyAttack{LoadTexture("sprites/enemies/imp/grey_attack.png")};
    const Texture2D ImpGreyDeath{LoadTexture("sprites/enemies/imp/grey_death.png")};
    const Texture2D ImpGreyHurt{LoadTexture("sprites/enemies/imp/grey_hurt.png")};
    const Texture2D ImpGreyIdle{LoadTexture("sprites/enemies/imp/grey_idle.png")};
    const Texture2D ImpGreyProjectile{LoadTexture("sprites/enemies/imp/grey_projectile.png")};
    const Texture2D ImpGreyWalk{LoadTexture("sprites/enemies/imp/grey_walk.png")};
    const Texture2D ImpRedAttack{LoadTexture("sprites/enemies/imp/red_attack.png")};
    const Texture2D ImpRedDeath{LoadTexture("sprites/enemies/imp/red_death.png")};
    const Texture2D ImpRedHurt{LoadTexture("sprites/enemies/imp/red_hurt.png")};
    const Texture2D ImpRedIdle{LoadTexture("sprites/enemies/imp/red_idle.png")};
    const Texture2D ImpRedProjectile{LoadTexture("sprites/enemies/imp/red_projectile.png")};
    const Texture2D ImpRedWalk{LoadTexture("sprites/enemies/imp/red_walk.png")};
    const Texture2D ImpYellowAttack{LoadTexture("sprites/enemies/imp/yellow_attack.png")};
    const Texture2D ImpYellowDeath{LoadTexture("sprites/enemies/imp/yellow_death.png")};
    const Texture2D ImpYellowHurt{LoadTexture("sprites/enemies/imp/yellow_hurt.png")};
    const Texture2D ImpYellowIdle{LoadTexture("sprites/enemies/imp/yellow_idle.png")};
    const Texture2D ImpYellowProjectile{LoadTexture("sprites/enemies/imp/yellow_projectile.png")};
    const Texture2D ImpYellowWalk{LoadTexture("sprites/enemies/imp/yellow_walk.png")};

    const Texture2D MushroomBlueAttack{LoadTexture("sprites/enemies/mushroom/blue_attack.png")};
    const Texture2D MushroomBlueDeath{LoadTexture("sprites/enemies/mushroom/blue_death.png")};
    const Texture2D MushroomBlueHurt{LoadTexture("sprites/enemies/mushroom/blue_hurt.png")};
    const Texture2D MushroomBlueIdle{LoadTexture("sprites/enemies/mushroom/blue_idle.png")};
    const Texture2D MushroomBlueWalk{LoadTexture("sprites/enemies/mushroom/blue_walk.png")};
    const Texture2D MushroomBrownAttack{LoadTexture("sprites/enemies/mushroom/brown_attack.png")};
    const Texture2D MushroomBrownDeath{LoadTexture("sprites/enemies/mushroom/brown_death.png")};
    const Texture2D MushroomBrownHurt{LoadTexture("sprites/enemies/mushroom/brown_hurt.png")};
    const Texture2D MushroomBrownIdle{LoadTexture("sprites/enemies/mushroom/brown_idle.png")};
    const Texture2D MushroomBrownWalk{LoadTexture("sprites/enemies/mushroom/brown_walk.png")};
    const Texture2D MushroomGreenAttack{LoadTexture("sprites/enemies/mushroom/green_attack.png")};
    const Texture2D MushroomGreenDeath{LoadTexture("sprites/enemies/mushroom/green_death.png")};
    const Texture2D MushroomGreenHurt{LoadTexture("sprites/enemies/mushroom/green_hurt.png")};
    const Texture2D MushroomGreenIdle{LoadTexture("sprites/enemies/mushroom/green_idle.png")};
    const Texture2D MushroomGreenWalk{LoadTexture("sprites/enemies/mushroom/green_walk.png")};
    const Texture2D MushroomPinkAttack{LoadTexture("sprites/enemies/mushroom/pink_attack.png")};
    const Texture2D MushroomPinkDeath{LoadTexture("sprites/enemies/mushroom/pink_death.png")};
    const Texture2D MushroomPinkHurt{LoadTexture("sprites/enemies/mushroom/pink_hurt.png")};
    const Texture2D MushroomPinkIdle{LoadTexture("sprites/enemies/mushroom/pink_idle.png")};
    const Texture2D MushroomPinkWalk{LoadTexture("sprites/enemies/mushroom/pink_walk.png")};
    const Texture2D MushroomRedAttack{LoadTexture("sprites/enemies/mushroom/red_attack.png")};
    const Texture2D MushroomRedDeath{LoadTexture("sprites/enemies/mushroom/red_death.png")};
    const Texture2D MushroomRedHurt{LoadTexture("sprites/enemies/mushroom/red_hurt.png")};
    const Texture2D MushroomRedIdle{LoadTexture("sprites/enemies/mushroom/red_idle.png")};
    const Texture2D MushroomRedWalk{LoadTexture("sprites/enemies/mushroom/red_walk.png")};
    const Texture2D MushroomTealAttack{LoadTexture("sprites/enemies/mushroom/teal_attack.png")};
    const Texture2D MushroomTealDeath{LoadTexture("sprites/enemies/mushroom/teal_death.png")};
    const Texture2D MushroomTealHurt{LoadTexture("sprites/enemies/mushroom/teal_hurt.png")};
    const Texture2D MushroomTealIdle{LoadTexture("sprites/enemies/mushroom/teal_idle.png")};
    const Texture2D MushroomTealWalk{LoadTexture("sprites/enemies/mushroom/teal_walk.png")};

    const Texture2D NecromancerBlueAttack{LoadTexture("sprites/enemies/necromancer/blue_attack.png")};
    const Texture2D NecromancerBlueDeath{LoadTexture("sprites/enemies/necromancer/blue_death.png")};
    const Texture2D NecromancerBlueHurt{LoadTexture("sprites/enemies/necromancer/blue_hurt.png")};
    const Texture2D NecromancerBlueIdle{LoadTexture("sprites/enemies/necromancer/blue_idle.png")};
    const Texture2D NecromancerBlueProjectile{LoadTexture("sprites/enemies/necromancer/blue_projectile.png")};
    const Texture2D NecromancerBlueWalk{LoadTexture("sprites/enemies/necromancer/blue_walk.png")};
    const Texture2D NecromancerBrownAttack{LoadTexture("sprites/enemies/necromancer/brown_attack.png")};
    const Texture2D NecromancerBrownDeath{LoadTexture("sprites/enemies/necromancer/brown_death.png")};
    const Texture2D NecromancerBrownHurt{LoadTexture("sprites/enemies/necromancer/brown_hurt.png")};
    const Texture2D NecromancerBrownIdle{LoadTexture("sprites/enemies/necromancer/brown_idle.png")};
    const Texture2D NecromancerBrownProjectile{LoadTexture("sprites/enemies/necromancer/brown_projectile.png")};
    const Texture2D NecromancerBrownWalk{LoadTexture("sprites/enemies/necromancer/brown_walk.png")};
    const Texture2D NecromancerPurpleAttack{LoadTexture("sprites/enemies/necromancer/purple_attack.png")};
    const Texture2D NecromancerPurpleDeath{LoadTexture("sprites/enemies/necromancer/purple_death.png")};
    const Texture2D NecromancerPurpleHurt{LoadTexture("sprites/enemies/necromancer/purple_hurt.png")};
    const Texture2D NecromancerPurpleIdle{LoadTexture("sprites/enemies/necromancer/purple_idle.png")};
    const Texture2D NecromancerPurpleProjectile{LoadTexture("sprites/enemies/necromancer/purple_projectile.png")};
    const Texture2D NecromancerPurpleWalk{LoadTexture("sprites/enemies/necromancer/purple_walk.png")};
    const Texture2D NecromancerRedAttack{LoadTexture("sprites/enemies/necromancer/red_attack.png")};
    const Texture2D NecromancerRedDeath{LoadTexture("sprites/enemies/necromancer/red_death.png")};
    const Texture2D NecromancerRedHurt{LoadTexture("sprites/enemies/necromancer/red_hurt.png")};
    const Texture2D NecromancerRedIdle{LoadTexture("sprites/enemies/necromancer/red_idle.png")};
    const Texture2D NecromancerRedProjectile{LoadTexture("sprites/enemies/necromancer/red_projectile.png")};
    const Texture2D NecromancerRedWalk{LoadTexture("sprites/enemies/necromancer/red_walk.png")};

    const Texture2D ShadowBlueAttack{LoadTexture("sprites/enemies/shadow/blue_attack.png")};
    const Texture2D ShadowBlueDeath{LoadTexture("sprites/enemies/shadow/blue_death.png")};
    const Texture2D ShadowBlueHurt{LoadTexture("sprites/enemies/shadow/blue_hurt.png")};
    const Texture2D ShadowBlueIdle{LoadTexture("sprites/enemies/shadow/blue_idle.png")};
    const Texture2D ShadowBlueWalk{LoadTexture("sprites/enemies/shadow/blue_walk.png")};
    const Texture2D ShadowRedAttack{LoadTexture("sprites/enemies/shadow/red_attack.png")};
    const Texture2D ShadowRedDeath{LoadTexture("sprites/enemies/shadow/red_death.png")};
    const Texture2D ShadowRedHurt{LoadTexture("sprites/enemies/shadow/red_hurt.png")};
    const Texture2D ShadowRedIdle{LoadTexture("sprites/enemies/shadow/red_idle.png")};
    const Texture2D ShadowRedWalk{LoadTexture("sprites/enemies/shadow/red_walk.png")};
    const Texture2D ShadowTealAttack{LoadTexture("sprites/enemies/shadow/teal_attack.png")};
    const Texture2D ShadowTealDeath{LoadTexture("sprites/enemies/shadow/teal_death.png")};
    const Texture2D ShadowTealHurt{LoadTexture("sprites/enemies/shadow/teal_hurt.png")};
    const Texture2D ShadowTealIdle{LoadTexture("sprites/enemies/shadow/teal_idle.png")};
    const Texture2D ShadowTealWalk{LoadTexture("sprites/enemies/shadow/teal_walk.png")};
    const Texture2D ShadowYellowAttack{LoadTexture("sprites/enemies/shadow/yellow_attack.png")};
    const Texture2D ShadowYellowDeath{LoadTexture("sprites/enemies/shadow/yellow_death.png")};
    const Texture2D ShadowYellowHurt{LoadTexture("sprites/enemies/shadow/yellow_hurt.png")};
    const Texture2D ShadowYellowIdle{LoadTexture("sprites/enemies/shadow/yellow_idle.png")};
    const Texture2D ShadowYellowWalk{LoadTexture("sprites/enemies/shadow/yellow_walk.png")};

    const Texture2D SpiderBlackAttack{LoadTexture("sprites/enemies/spider/black_attack.png")};
    const Texture2D SpiderBlackDeath{LoadTexture("sprites/enemies/spider/black_death.png")};
    const Texture2D SpiderBlackHurt{LoadTexture("sprites/enemies/spider/black_hurt.png")};
    const Texture2D SpiderBlackIdle{LoadTexture("sprites/enemies/spider/black_idle.png")};
    const Texture2D SpiderBlackWalk{LoadTexture("sprites/enemies/spider/black_walk.png")};
    const Texture2D SpiderBlueAttack{LoadTexture("sprites/enemies/spider/blue_attack.png")};
    const Texture2D SpiderBlueDeath{LoadTexture("sprites/enemies/spider/blue_death.png")};
    const Texture2D SpiderBlueHurt{LoadTexture("sprites/enemies/spider/blue_hurt.png")};
    const Texture2D SpiderBlueIdle{LoadTexture("sprites/enemies/spider/blue_idle.png")};
    const Texture2D SpiderBlueWalk{LoadTexture("sprites/enemies/spider/blue_walk.png")};
    const Texture2D SpiderBrownAttack{LoadTexture("sprites/enemies/spider/brown_attack.png")};
    const Texture2D SpiderBrownDeath{LoadTexture("sprites/enemies/spider/brown_death.png")};
    const Texture2D SpiderBrownHurt{LoadTexture("sprites/enemies/spider/brown_hurt.png")};
    const Texture2D SpiderBrownIdle{LoadTexture("sprites/enemies/spider/brown_idle.png")};
    const Texture2D SpiderBrownWalk{LoadTexture("sprites/enemies/spider/brown_walk.png")};
    const Texture2D SpiderGreenAttack{LoadTexture("sprites/enemies/spider/green_attack.png")};
    const Texture2D SpiderGreenDeath{LoadTexture("sprites/enemies/spider/green_death.png")};
    const Texture2D SpiderGreenHurt{LoadTexture("sprites/enemies/spider/green_hurt.png")};
    const Texture2D SpiderGreenIdle{LoadTexture("sprites/enemies/spider/green_idle.png")};
    const Texture2D SpiderGreenWalk{LoadTexture("sprites/enemies/spider/green_walk.png")};
    const Texture2D SpiderRedAttack{LoadTexture("sprites/enemies/spider/red_attack.png")};
    const Texture2D SpiderRedDeath{LoadTexture("sprites/enemies/spider/red_death.png")};
    const Texture2D SpiderRedHurt{LoadTexture("sprites/enemies/spider/red_hurt.png")};
    const Texture2D SpiderRedIdle{LoadTexture("sprites/enemies/spider/red_idle.png")};
    const Texture2D SpiderRedWalk{LoadTexture("sprites/enemies/spider/red_walk.png")};

    const Texture2D ToadBlueAttack{LoadTexture("sprites/enemies/toad/blue_attack.png")};
    const Texture2D ToadBlueDeath{LoadTexture("sprites/enemies/toad/blue_death.png")};
    const Texture2D ToadBlueHurt{LoadTexture("sprites/enemies/toad/blue_hurt.png")};
    const Texture2D ToadBlueIdle{LoadTexture("sprites/enemies/toad/blue_idle.png")};
    const Texture2D ToadBlueWalk{LoadTexture("sprites/enemies/toad/blue_walk.png")};
    const Texture2D ToadGreenAttack{LoadTexture("sprites/enemies/toad/green_attack.png")};
    const Texture2D ToadGreenDeath{LoadTexture("sprites/enemies/toad/green_death.png")};
    const Texture2D ToadGreenHurt{LoadTexture("sprites/enemies/toad/green_hurt.png")};
    const Texture2D ToadGreenIdle{LoadTexture("sprites/enemies/toad/green_idle.png")};
    const Texture2D ToadGreenWalk{LoadTexture("sprites/enemies/toad/green_walk.png")};
    const Texture2D ToadPinkAttack{LoadTexture("sprites/enemies/toad/pink_attack.png")};
    const Texture2D ToadPinkDeath{LoadTexture("sprites/enemies/toad/pink_death.png")};
    const Texture2D ToadPinkHurt{LoadTexture("sprites/enemies/toad/pink_hurt.png")};
    const Texture2D ToadPinkIdle{LoadTexture("sprites/enemies/toad/pink_idle.png")};
    const Texture2D ToadPinkWalk{LoadTexture("sprites/enemies/toad/pink_walk.png")};
    const Texture2D ToadRedAttack{LoadTexture("sprites/enemies/toad/red_attack.png")};
    const Texture2D ToadRedDeath{LoadTexture("sprites/enemies/toad/red_death.png")};
    const Texture2D ToadRedHurt{LoadTexture("sprites/enemies/toad/red_hurt.png")};
    const Texture2D ToadRedIdle{LoadTexture("sprites/enemies/toad/red_idle.png")};
    const Texture2D ToadRedWalk{LoadTexture("sprites/enemies/toad/red_walk.png")};
    const Texture2D ToadWhiteAttack{LoadTexture("sprites/enemies/toad/white_attack.png")};
    const Texture2D ToadWhiteDeath{LoadTexture("sprites/enemies/toad/white_death.png")};
    const Texture2D ToadWhiteHurt{LoadTexture("sprites/enemies/toad/white_hurt.png")};
    const Texture2D ToadWhiteIdle{LoadTexture("sprites/enemies/toad/white_idle.png")};
    const Texture2D ToadWhiteWalk{LoadTexture("sprites/enemies/toad/white_walk.png")};

    const Texture2D CrowFlySleep{LoadTexture("sprites/npc/wildlife/crow/crow_fly_sleep.png")};
    const Texture2D CrowFlying{LoadTexture("sprites/npc/wildlife/crow/crow_flying.png")};
    const Texture2D CrowGroundSleep{LoadTexture("sprites/npc/wildlife/crow/crow_ground_sleep.png")};
    const Texture2D CrowIdle{LoadTexture("sprites/npc/wildlife/crow/crow_idle.png")};
    const Texture2D CrowIdleTwo{LoadTexture("sprites/npc/wildlife/crow/crow_idle2.png")};
    const Texture2D CrowWalk{LoadTexture("sprites/npc/wildlife/crow/crow_walk.png")};

    const Texture2D FoxFamilyAngry{LoadTexture("sprites/npc/wildlife/fox/fox_family_angry.png")};
    const Texture2D FoxFamilyIdle{LoadTexture("sprites/npc/wildlife/fox/fox_family_idle.png")};
    const Texture2D FoxFamilyIdleTwo{LoadTexture("sprites/npc/wildlife/fox/fox_family_idle2.png")};
    const Texture2D FoxFamilyLazy{LoadTexture("sprites/npc/wildlife/fox/fox_family_lazy.png")};
    const Texture2D FoxFamilyRun{LoadTexture("sprites/npc/wildlife/fox/fox_family_run.png")};
    const Texture2D FoxFamilySleep{LoadTexture("sprites/npc/wildlife/fox/fox_family_sleep.png")};
    const Texture2D FoxFamilyWalk{LoadTexture("sprites/npc/wildlife/fox/fox_family_walk.png")};

    const Texture2D SquirrelEat{LoadTexture("sprites/npc/wildlife/squirrel/squirrel_eat.png")};
    const Texture2D SquirrelIdle{LoadTexture("sprites/npc/wildlife/squirrel/squirrel_idle.png")};
    const Texture2D SquirrelIdleTwo{LoadTexture("sprites/npc/wildlife/squirrel/squirrel_idle2.png")};
    const Texture2D SquirrelJump{LoadTexture("sprites/npc/wildlife/squirrel/squirrel_jump.png")};
    const Texture2D SquirrelRun{LoadTexture("sprites/npc/wildlife/squirrel/squirrel_run.png")};
    const Texture2D SquirrelSleep{LoadTexture("sprites/npc/wildlife/squirrel/squirrel_sleep.png")};
    const Texture2D SquirrelWalk{LoadTexture("sprites/npc/wildlife/squirrel/squirrel_walk.png")};
};

#endif // gametextures_hpp