#include "headers/hud.hpp"

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

    Vector2 HeartRowOne{100, 30}; 
    Vector2 HeartRowTwo{-50, 60}; 
    Vector2 HeartSpacing{30,0}; 
    float MaxHP{10.f};

    DrawTextureEx(RecContainer, Vector2{15, 15}, 0.f, 5.f, WHITE);  // Draw Container holding the hearts
    DrawTextureEx(Fox, Vector2{20,20}, 0.f, Scale, WHITE);          // Draw fox portrait

    for (float i = 1; i <= MaxHP; ++i) {
        // Draw hearts 1-5 on first row
        if (i <= MaxHP/2.f) {
            if (i < Health) {
                if ((i + .5f) == Health) {
                    DrawTextureEx(HeartHalf, HeartRowOne, 0.f, 3.f, WHITE);
                }
                else {
                    DrawTextureEx(HeartFull, HeartRowOne, 0.f, 3.f, WHITE);
                }
            }
            else {
                DrawTextureEx(HeartEmpty, HeartRowOne, 0.f, 3.f, WHITE);
            }
        }
        // Draw hearts 6-10 on second row
        else {
            if (i < Health) {
                if ((i + .5f) == Health) {
                    DrawTextureEx(HeartHalf, HeartRowTwo, 0.f, 3.f, WHITE);
                }
                else {
                    DrawTextureEx(HeartFull, HeartRowTwo, 0.f, 3.f, WHITE);
                }
            }
            else {
                DrawTextureEx(HeartEmpty, HeartRowTwo, 0.f, 3.f, WHITE);
            }
        }

        // add spacing between each heart
        HeartRowOne = Vector2Add(HeartRowOne, HeartSpacing);
        HeartRowTwo = Vector2Add(HeartRowTwo, HeartSpacing);
    }

    // Rectangle for debugging info (WorldPos, FPS, etc.)
    DrawTextureEx(RecContainer, Vector2{15, 135}, 0.f, 4.f, WHITE);
    DrawTextureEx(RecContainer, Vector2{15, 215}, 0.f, 2.f, WHITE);
    
}