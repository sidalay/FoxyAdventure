#include "gameaudio.hpp"

GameAudio::~GameAudio()
{
    UnloadSound(AltarInsert);
    UnloadSound(BossDeath);
    UnloadSound(FoxAttack);
    UnloadSound(FoxAttackTwo);
    UnloadSound(ImpactHeavy);
    UnloadSound(ImpactMedium);
    UnloadSound(MapChange);
    UnloadSound(MiniMapOpen);
    UnloadSound(MonsterAttack);
    UnloadSound(MonsterDeath);
    UnloadSound(MonsterRangedAttack);
    UnloadSound(MoveCursor);
    UnloadSound(NpcTalk);
    UnloadSound(Pushing);
    UnloadSound(Select);
    UnloadSound(Transition);
    UnloadSound(TreasureOpen);
    UnloadSound(Walking);
    UnloadMusicStream(DungeonTheme);
    UnloadMusicStream(ForestTheme);
    UnloadMusicStream(PauseMenuTheme);
}