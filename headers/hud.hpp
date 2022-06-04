#ifndef HUD_HPP
#define HUD_HPP

#include <raylib.h>
#include "headers/character.hpp"

class HUD 
{
public:
    HUD(GameTexture& GameTextures);
    ~HUD() = default;
    HUD(const HUD&) = delete;
    HUD(HUD&&) = default;
    HUD& operator=(const HUD&) = delete;
    HUD& operator=(HUD&&) = default;
    
    void Tick();
    void Draw(float Health, Emotion State);
    
private:
    Texture2D HeartFull{};
    Texture2D HeartHalf{};
    Texture2D HeartEmpty{};

    Texture2D RecContainer{};
    Texture2D SquareContainer{};
    
    Texture2D Fox{};
    Texture2D FoxFrame{};
    Texture2D FoxAngry{};
    Texture2D FoxHappy{};
    Texture2D FoxNervous{};
    Texture2D FoxSad{};
    Texture2D FoxSleeping{};
    Texture2D FoxHurt{};
    Texture2D FoxDead{};

    float Scale{2.f};
};

#endif // HUD_HPP