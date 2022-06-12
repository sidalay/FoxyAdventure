#include "hud.hpp"

HUD::HUD(GameTexture& GameTextures)
    : GameTextures{&GameTextures}
{

}

void HUD::Draw(float Health, Emotion State)
{
    // manage which fox portraits to draw
    switch (State)
    {
        case Emotion::ANGRY:
            Fox = GameTextures->FoxPortraitAngry;
            break;
        case Emotion::HAPPY:
            Fox = GameTextures->FoxPortraitHappy;
            break;
        case Emotion::NERVOUS:
            Fox = GameTextures->FoxPortraitNervous;
            break;
        case Emotion::SAD:
            Fox = GameTextures->FoxPortraitSad;
            break;
        case Emotion::SLEEPING:
            Fox = GameTextures->FoxPortraitSleeping;
            break;
        case Emotion::HURT:
            Fox = GameTextures->FoxPortraitHurt;
            break;
        case Emotion::DEAD:
            Fox = GameTextures->FoxPortraitDead;
            break;
        default:
            Fox = GameTextures->FoxPortraitFrame;
            break;
    }

    Vector2 HeartRowOne{100, 30}; 
    Vector2 HeartRowTwo{-50, 60}; 
    Vector2 HeartSpacing{30,0}; 
    float MaxHP{10.f};

    DrawTextureEx(GameTextures->TransparentContainer, Vector2{15, 15}, 0.f, 5.f, WHITE);  // Draw Container holding the hearts
    DrawTextureEx(Fox, Vector2{20,20}, 0.f, Scale, WHITE);                                // Draw fox portrait

    for (float i = 1; i <= MaxHP; ++i) {
        // Draw hearts 1-5 on first row
        if (i <= MaxHP/2.f) {
            if (i < Health) {
                if ((i + .5f) == Health) {
                    DrawTextureEx(GameTextures->HeartHalf, HeartRowOne, 0.f, 3.f, WHITE);
                }
                else {
                    DrawTextureEx(GameTextures->HeartFull, HeartRowOne, 0.f, 3.f, WHITE);
                }
            }
            else {
                DrawTextureEx(GameTextures->HeartEmpty, HeartRowOne, 0.f, 3.f, WHITE);
            }
        }
        // Draw hearts 6-10 on second row
        else {
            if (i < Health) {
                if ((i + .5f) == Health) {
                    DrawTextureEx(GameTextures->HeartHalf, HeartRowTwo, 0.f, 3.f, WHITE);
                }
                else {
                    DrawTextureEx(GameTextures->HeartFull, HeartRowTwo, 0.f, 3.f, WHITE);
                }
            }
            else {
                DrawTextureEx(GameTextures->HeartEmpty, HeartRowTwo, 0.f, 3.f, WHITE);
            }
        }

        // add spacing between each heart
        HeartRowOne = Vector2Add(HeartRowOne, HeartSpacing);
        HeartRowTwo = Vector2Add(HeartRowTwo, HeartSpacing);
    }
}