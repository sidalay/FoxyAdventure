#ifndef HUD_HPP
#define HUD_HPP

#include <raylib.h>
#include "headers/character.hpp"

class HUD 
{
public:
    HUD(GameTexture& GameTextures);
    ~HUD();
    HUD(const HUD&) = delete;
    HUD(HUD&&) = default;
    HUD& operator=(const HUD&) = delete;
    HUD& operator=(HUD&&) = default;
    
    void Tick();
    void Draw(float Health, Emotion State);
    
private:
    GameTexture* GameTextures{};
    Texture2D Fox{};

    float Scale{2.f};
};

#endif // HUD_HPP