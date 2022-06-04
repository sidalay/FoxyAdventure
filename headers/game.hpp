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
    int PauseFoxIndex{};
    float Opacity{0.f};
    float TransitionInTime{0.f};
    float TransitionOutTime{0.f};
    bool ExitGame{false};
    bool IsYes{false};
};

namespace Game 
{
    void Run();
    void Initialize(Window& Window, int FPS, std::string Title);
    void Tick(Window& Window, Background& Map, GameState& State, GameState& PrevState, 
              Character& Character, Props& Props, HUD& Hud, std::vector<Enemy>& Enemies, 
              std::vector<Enemy>& Crows, std::array<Sprite, 5>& PauseFox, std::array<Texture2D, 9>& Buttons, std::vector<Prop>& Trees, GameInfo& GameInfo);
    void CheckScreenSizing(Window& Window);
    void SetFullScreen(Window& Window);
    void Update(Background& Map, GameState& State, GameState& NextState, Character& Character, Props& Props, 
                std::vector<Enemy>& Enemies, std::vector<Enemy>& Crows, std::vector<Prop>& Trees, GameInfo& GameInfo);
    void Draw(Background& Map, Character& Character, Props& Props, HUD& Hud, std::vector<Enemy>& Enemies, std::vector<Enemy>& Crows, std::vector<Prop>& Trees);
    void PauseUpdate(GameState& State, GameState& NextState, std::array<Sprite, 5>& PauseFox, std::array<Texture2D, 9>& Buttons, GameInfo& GameInfo);
    void PauseDraw(std::array<Sprite, 5>& PauseFox, std::array<Texture2D, 9>& Buttons, GameState& State, const GameInfo& GameInfo);
    void ExitUpdate(GameState& State, GameState& NextState, GameInfo& GameInfo);
    void ExitDraw(GameState& State, const GameInfo& GameInfo);
    void Transition(GameState& State, GameState& NextState, GameInfo& GameInfo);

    std::vector<std::vector<Prop>> InitializePropsUnder(GameTexture& GameTextures);
    std::vector<std::vector<Prop>> InitializePropsOver(GameTexture& GameTextures);
    std::vector<Prop> InitializeTrees(GameTexture& GameTextures);
    std::vector<Enemy> InitializeEnemies(Background& MapBG, Window& Window, GameTexture& GameTextures);
    std::vector<Enemy> InitializeCrows(Background& MapBG, Window& Window, GameTexture& GameTextures);
}

#endif // GAME_HPP