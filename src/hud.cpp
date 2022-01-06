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
        default:
            Fox = FoxDefault;
            break;
    }

    Vector2 PosRowOne{100, 20};
    Vector2 PosRowTwo{-50, 50};
    Vector2 PosAdd{30,0};

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
}