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

void HUD::Draw(float Health, Emotion State)
{
    // manage which fox portraits to draw
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
        case Emotion::DEAD:
            Fox = FoxDead;
            break;
        default:
            Fox = FoxDefault;
            break;
    }

    Vector2 PosRowOne{100, 30}; // Heart row 1
    Vector2 PosRowTwo{-50, 60}; // Heart row 2
    Vector2 PosAdd{30,0};       // Spacing between each heart
    float MaxHP{10.f};

    DrawTextureEx(RecContainer, Vector2{15, 15}, 0.f, 5.f, WHITE);  // Draw Container holding the hearts
    DrawTextureEx(Fox, Vector2{20,20}, 0.f, Scale, WHITE);          // Draw fox portrait

    for (float i = 1; i <= MaxHP; ++i) {
        // Draw hearts 1-5 on first row
        if (i <= MaxHP/2.f) {
            if (i < Health) {
                if ((i + .5f) == Health) {
                    DrawTextureEx(HeartHalf, PosRowOne, 0.f, 3.f, WHITE);
                }
                else {
                    DrawTextureEx(HeartFull, PosRowOne, 0.f, 3.f, WHITE);
                }
            }
            else {
                DrawTextureEx(HeartEmpty, PosRowOne, 0.f, 3.f, WHITE);
            }
        }
        // Draw hearts 6-10 on second row
        else {
            if (i < Health) {
                if ((i + .5f) == Health) {
                    DrawTextureEx(HeartHalf, PosRowTwo, 0.f, 3.f, WHITE);
                }
                else {
                    DrawTextureEx(HeartFull, PosRowTwo, 0.f, 3.f, WHITE);
                }
            }
            else {
                DrawTextureEx(HeartEmpty, PosRowTwo, 0.f, 3.f, WHITE);
            }
        }

        // add spacing between each heart
        PosRowOne = Vector2Add(PosRowOne, PosAdd);
        PosRowTwo = Vector2Add(PosRowTwo, PosAdd);
    }

    // Rectangle for debugging info (WorldPos, FPS, etc.)
    DrawTextureEx(RecContainer, Vector2{15, 135}, 0.f, 4.f, WHITE);
    DrawTextureEx(RecContainer, Vector2{15, 215}, 0.f, 2.f, WHITE);
    
}