#include "gameaudio.hpp"

GameAudio::~GameAudio()
{
    UnloadSound(BossDeath);
    UnloadSound(FoxAttack);
    UnloadSound(ImpactHeavy);
    UnloadSound(ImpactMedium);
    UnloadSound(MonsterAttack);
    UnloadSound(MonsterDeath);
    UnloadSound(MonsterRangedAttack);
    UnloadSound(MapChange);
    UnloadSound(Transition);
    UnloadSound(TreasureOpen);
    UnloadSound(Walking);
    UnloadMusicStream(DungeonTheme);
    UnloadMusicStream(ForestTheme);
    UnloadMusicStream(PauseMenuTheme);
}