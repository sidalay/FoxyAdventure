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
    
    const Sound BossDeath{LoadSound("audio/BossDeath.wav")};
    const Sound FoxAttack{LoadSound("audio/Slash.wav")};
    const Sound ImpactHeavy{LoadSound("audio/ImpactHeavy.wav")};
    const Sound ImpactMedium{LoadSound("audio/ImpactMedium.wav")};
    const Sound MonsterAttack{LoadSound("audio/MonsterAttack.wav")};
    const Sound MonsterDeath{LoadSound("audio/MonsterDeath.wav")};
    const Sound MonsterRangedAttack{LoadSound("audio/MonsterRangedAttack.wav")};
    const Sound MapChange{LoadSound("audio/MapChange.wav")};
    const Sound Transition{LoadSound("audio/Transition.wav")};
    const Sound TreasureOpen{LoadSound("audio/Treasure.wav")};
    const Sound Walking{LoadSound("audio/Footsteps.wav")};

    Music DungeonTheme{LoadMusicStream("audio/ZealPalace.mp3")};
    Music ForestTheme{LoadMusicStream("audio/SecretOfTheForest.mp3")};
    Music PauseMenuTheme{LoadMusicStream("audio/PauseMenuTheme.ogg")};
};


#endif // GAMEAUDIO_HPP