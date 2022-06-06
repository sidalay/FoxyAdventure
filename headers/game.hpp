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

namespace Game 
{
    enum class State 
    {
        MAINMENU, RUNNING, PAUSED, GAMEOVER, TRANSITION, EXIT
    };

    struct Info
    {
        Background Map;
        int PauseFoxIndex{0};
        float Opacity{0.f};
        float TransitionInTime{0.f};
        float TransitionOutTime{0.f};
        bool MainMenuStart{true};
        bool ExitGame{false};
        bool ExitIsYes{false};
        Game::State State{Game::State::TRANSITION};
        Game::State NextState{Game::State::MAINMENU};

        // Debugging --------------------
        bool ShowPos{false};
        bool ShowFPS{false};
        bool NoClipOn{false};
        bool ToggleAll{false};
        bool DevToolsOn{false};
        bool ShowDevTools{true};
        bool DrawRectanglesOn{false};
    };

    struct Objects
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

    void Run();
    void Initialize(Window& Window, std::string Title);
    void CheckScreenSizing(Window& Window);
    void Tick(Window& Window, Game::Info& Info, GameTexture& Textures, Game::Objects& Objects);
    void Update(Game::Info& Info, Game::Objects& Objects);
    void Draw(Game::Info& Info, Game::Objects& Objects);
    void PauseUpdate(Game::Info& Info, Game::Objects& Objects);
    void PauseDraw(const Game::Info& Info, GameTexture& Textures, Game::Objects& Objects);
    void ExitUpdate(Game::Info& Info);
    void ExitDraw(const Game::Info& Info);
    void MainMenuUpdate(Game::Info& Info);
    void MainMenuDraw(const Game::Info& Info);
    void GameOverUpdate(Game::Info& Info);
    void GameOverDraw(const Game::Info& Info);
    void Transition(Game::Info& Info);

    HUD InitializeHud(GameTexture& Textures);
    Character InitializeFox(Window& Window, Game::Info& Info, GameTexture& Textures);
    std::array<Sprite,5> InitializePauseFox(GameTexture& Textures);
    std::array<const Texture2D,9> InitializeButtons(GameTexture& Textures);
    std::vector<std::vector<Prop>> InitializePropsUnder(GameTexture& Textures);
    std::vector<std::vector<Prop>> InitializePropsOver(GameTexture& Textures);
    std::vector<Prop> InitializeTrees(GameTexture& Textures);
    std::vector<Enemy> InitializeEnemies(Background& MapBG, Window& Window, GameTexture& Textures);
    std::vector<Enemy> InitializeCrows(Background& MapBG, Window& Window, GameTexture& Textures);

    // Debugging --------------------
    void DrawCollisionRecs(Prop& Prop, Vector2 CharacterWorldPos, Color RecColor = CLITERAL(Color){ 0, 121, 241, 150 });
    template <typename Object>
    void DrawCollisionRecs(Object& Type, Color Color = CLITERAL(Color){ 0, 238, 135, 150 });
    template <typename Object>
    void DrawAttackRecs(Object& Type, Color Color = CLITERAL(Color){ 240, 255, 20, 150 });
}

#endif // GAME_HPP