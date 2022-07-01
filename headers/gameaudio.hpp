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
    
    const Sound AltarInsert{LoadSound("audio/AltarInsert.wav")};
    const Sound BossDeath{LoadSound("audio/BossDeath.wav")};
    const Sound FoxAttack{LoadSound("audio/Slash.wav")};
    const Sound FoxAttackTwo{LoadSound("audio/DoubleSlash.wav")};
    const Sound ImpactHeavy{LoadSound("audio/ImpactHeavy.wav")};
    const Sound ImpactMedium{LoadSound("audio/ImpactMedium.wav")};
    const Sound MapChange{LoadSound("audio/MapChange.wav")};
    const Sound MiniMapOpen{LoadSound("audio/MiniMap.wav")};
    const Sound MonsterAttack{LoadSound("audio/MonsterAttack.wav")};
    const Sound MonsterDeath{LoadSound("audio/MonsterDeath.wav")};
    const Sound MonsterRangedAttack{LoadSound("audio/MonsterRangedAttack.wav")};
    const Sound MoveCursor{LoadSound("audio/MoveCursor.wav")};
    const Sound NpcTalk{LoadSound("audio/NpcTalk.wav")};
    const Sound Pushing{LoadSound("audio/Push.wav")};
    const Sound Select{LoadSound("audio/Select.wav")};
    const Sound Transition{LoadSound("audio/Transition.wav")};
    const Sound TreasureOpen{LoadSound("audio/Treasure.wav")};
    const Sound Walking{LoadSound("audio/Footsteps.wav")};

    Music DungeonTheme{LoadMusicStream("audio/ZealPalace.mp3")};
    Music ForestTheme{LoadMusicStream("audio/SecretOfTheForest.mp3")};
    Music PauseMenuTheme{LoadMusicStream("audio/PauseMenuTheme.ogg")};
};


#endif // GAMEAUDIO_HPP