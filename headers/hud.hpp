#ifndef HUD_HPP
#define HUD_HPP

#include "character.hpp"

class HUD 
{
public:
    explicit HUD(GameTexture& GameTextures);
    
    void Tick();
    void Draw(float Health, Emotion State);
    
private:
    GameTexture* GameTextures{};
    Texture2D Fox{};

    float Scale{2.f};
};

#endif // HUD_HPP