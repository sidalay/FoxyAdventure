#include "headers/hud.hpp"

HUD::HUD()
{

}

HUD::~HUD()
{

}

void HUD::Tick()
{

}

void HUD::Draw(int Health, Emotion State)
{
    switch (State)
    {
        case Emotion::ANGRY:
            Fox = FoxAngry;
            break;
        case Emotion::HAPPY:
            Fox = FoxHappy;
            break;
        case Emotion::NERVOUS:
            Fox = FoxNervous;
            break;
        case Emotion::SAD:
            Fox = FoxSad;
            break;
        case Emotion::SLEEPING:
            Fox = FoxSleeping;
            break;
        case Emotion::HURT:
            Fox = FoxHurt;
            break;
        default:
            Fox = FoxDefault;
            break;
    }

    Vector2 PosRowOne{100, 30};
    Vector2 PosRowTwo{-50, 60};
    Vector2 PosAdd{30,0};

    DrawTextureEx(RecContainer, Vector2{15, 15}, 0.f, 5.f, WHITE);
    DrawTextureEx(Fox, Vector2{20,20}, 0.f, Scale, WHITE);

    for (auto i = 1; i <= 10; ++i)
    {
        if (i <= 5)
        {
            if (i <= Health)
            {
                DrawTextureEx(HeartFull, PosRowOne, 0.f, 3.f, WHITE);
            }
            else 
            {
                DrawTextureEx(HeartEmpty, PosRowOne, 0.f, 3.f, WHITE);
            }
        }
        else
        {
            if (i <= Health)
            {
                DrawTextureEx(HeartFull, PosRowTwo, 0.f, 3.f, WHITE);
            }
            else 
            {
                DrawTextureEx(HeartEmpty, PosRowTwo, 0.f, 3.f, WHITE);
            }
        }

        PosRowOne = Vector2Add(PosRowOne, PosAdd);
        PosRowTwo = Vector2Add(PosRowTwo, PosAdd);
    }

    // Rectangle for debugging info (WorldPos, FPS, etc.)
    DrawTextureEx(RecContainer, Vector2{15, 135}, 0.f, 4.f, WHITE);
    DrawTextureEx(RecContainer, Vector2{15, 215}, 0.f, 2.f, WHITE);
    
}