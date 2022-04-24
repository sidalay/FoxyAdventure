#ifndef HUD_HPP
#define HUD_HPP

#include <raylib.h>
#include "headers/character.hpp"

class HUD 
{
private:
    Texture2D HeartFull{LoadTexture("sprites/props/Heart.png")};
    Texture2D HeartHalf{LoadTexture("sprites/props/Heart_Half.png")};
    Texture2D HeartEmpty{LoadTexture("sprites/props/Heart_Empty.png")};

    Texture2D RecContainer{LoadTexture("sprites/props/TransparentContainer.png")};
    Texture2D SquareContainer{LoadTexture("sprites/props/TransparentSquare.png")};
    
    Texture2D Fox{};
    Texture2D FoxDefault{LoadTexture("sprites/portraits/Fox_Frame.png")};
    Texture2D FoxAngry{LoadTexture("sprites/portraits/Fox_Angry.png")};
    Texture2D FoxHappy{LoadTexture("sprites/portraits/Fox_Happy.png")};
    Texture2D FoxNervous{LoadTexture("sprites/portraits/Fox_Nervous.png")};
    Texture2D FoxSad{LoadTexture("sprites/portraits/Fox_Sad.png")};
    Texture2D FoxSleeping{LoadTexture("sprites/portraits/Fox_Sleeping.png")};
    Texture2D FoxHurt{LoadTexture("sprites/portraits/Fox_Hurt.png")};
    Texture2D FoxDead{LoadTexture("sprites/portraits/Fox_Dead.png")};

    float Scale{2.f};
public:
    HUD();
    ~HUD();

    void Tick();
    void Draw(float Health, Emotion State);
};

#endif // HUD_HPP