#include "gameaudio.hpp"

GameAudio::~GameAudio()
{
    UnloadSound(Walking);
    UnloadMusicStream(ForestTheme);
    UnloadMusicStream(DungeonTheme);
}