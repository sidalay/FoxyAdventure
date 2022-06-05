#ifndef GAME_HPP
#define GAME_HPP

#include <raylib.h>
#include <string>
#include <vector>
#include "headers/hud.hpp"
#include "headers/prop.hpp"
#include "headers/sprite.hpp"
#include "headers/window.hpp"
#include "headers/enemy.hpp"
#include "headers/character.hpp"
#include "headers/background.hpp"
#include "headers/gametextures.hpp"

enum class GameState 
{
    MAINMENU, RUNNING, PAUSED, GAMEOVER, TRANSITION, EXIT
};

struct GameInfo
{
    Background Map;
    int PauseFoxIndex{0};
    float Opacity{0.f};
    float TransitionInTime{0.f};
    float TransitionOutTime{0.f};
    bool MainMenuStart{true};
    bool ExitGame{false};
    bool IsYes{false};
    GameState State{GameState::TRANSITION};
    GameState NextState{GameState::MAINMENU};
};

struct GameObjects
{
    HUD Hud;
    Character Fox;
    Props Props;
    std::vector<Enemy> Enemies;
    std::vector<Enemy> Crows;
    std::vector<Prop> Trees;
    std::array<Sprite, 5> PauseFox;
    std::array<const Texture2D, 9> Buttons;
};

namespace Game 
{
    void Run();
    void Initialize(Window& Window, std::string Title);
    void CheckScreenSizing(Window& Window);
    void Tick(Window& Window, GameInfo& GameInfo, GameTexture& GameTextures, GameObjects& GameObjects);
    void Update(GameInfo& GameInfo, GameObjects& GameObjects);
    void Draw(GameInfo& GameInfo, GameObjects& GameObjects);
    void PauseUpdate(GameInfo& GameInfo, GameObjects& GameObjects);
    void PauseDraw(const GameInfo& GameInfo, GameTexture& GameTextures, GameObjects& GameObjects);
    void ExitUpdate(GameInfo& GameInfo);
    void ExitDraw(const GameInfo& GameInfo);
    void MainMenuUpdate(GameInfo& GameInfo);
    void MainMenuDraw(const GameInfo& GameInfo);
    void GameOverUpdate(GameInfo& GameInfo);
    void GameOverDraw(const GameInfo& GameInfo);
    void Transition(GameInfo& GameInfo);

    Character InitializeFox(Window& Window, GameInfo& GameInfo, GameTexture& GameTextures);
    std::array<Sprite,5> InitializePauseFox(GameTexture& GameTextures);
    std::array<const Texture2D,9> InitializeButtons(GameTexture& GameTextures);
    std::vector<std::vector<Prop>> InitializePropsUnder(GameTexture& GameTextures);
    std::vector<std::vector<Prop>> InitializePropsOver(GameTexture& GameTextures);
    std::vector<Prop> InitializeTrees(GameTexture& GameTextures);
    std::vector<Enemy> InitializeEnemies(Background& MapBG, Window& Window, GameTexture& GameTextures);
    std::vector<Enemy> InitializeCrows(Background& MapBG, Window& Window, GameTexture& GameTextures);
}

#endif // GAME_HPP