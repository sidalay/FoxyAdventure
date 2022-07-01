#ifndef GAMEAUDIO_HPP
#define GAMEAUDIO_HPP

#include <raylib.h>

struct GameAudio 
{
    constexpr GameAudio() = default;
    ~GameAudio();
    GameAudio(const GameAudio&) = delete;
    GameAudio(GameAudio&&) = default;
    GameAudio& operator=(const GameAudio&) = delete;
    GameAudio& operator=(GameAudio&&) = default;
    
    const Sound Walking{LoadSound("../audio/Footsteps.wav")};
    Music ForestTheme{LoadMusicStream("../audio/SecretOfTheForest.mp3")};
    Music DungeonTheme{LoadMusicStream("../audio/ZealPalace.mp3")};
};


#endif // GAMEAUDIO_HPP