#ifndef GAME_HPP
#define GAME_HPP

#include <array>
#include "hud.hpp"

namespace Game 
{
    enum class State 
    {
        MAINMENU, FOREST, DUNGEON, PAUSED, GAMEOVER, TRANSITION, EXIT
    };

    struct Info
    {
        Background Map;
        int PauseFoxIndex{0};
        float Opacity{0.f};
        float EndGameTime{0.f};
        float TransitionInTime{0.f};
        float TransitionOutTime{0.f};
        bool MainMenuStart{true};
        bool GameOverStart{true};
        bool ExitGame{false};
        bool ExitIsYes{false};
        bool TeleportOn{false};
        Game::State State{Game::State::TRANSITION};
        Game::State PrevState{Game::State::FOREST};
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
        Props PropsContainer;
        std::vector<Enemy> Enemies;
        std::vector<Enemy> Crows;
        std::vector<Prop> Trees;
        std::array<Sprite, 5> PauseFox;
        std::array<const Texture2D, 9> Buttons;
    };

    void Run();
    void Initialize(const Window& Window, const std::string& Title);
    void CheckScreenSizing(Window& Window);
    void Tick(Window& Window, Game::Info& Info, Game::Objects& Objects, const GameTexture& Textures, const GameAudio& Audio);
    void ForestUpdate(Game::Info& Info, Game::Objects& Objects);
    void ForestDraw(Game::Info& Info, Game::Objects& Objects);
    void DungeonUpdate(Game::Info& Info, Game::Objects& Objects);
    void DungeonDraw(Game::Info& Info, Game::Objects& Objects);
    void PauseUpdate(Game::Info& Info, Game::Objects& Objects);
    void PauseDraw(const Game::Info& Info, Game::Objects& Objects, const GameTexture& Textures);
    void ExitUpdate(Game::Info& Info);
    void ExitDraw(const Game::Info& Info);
    void MainMenuUpdate(Game::Info& Info);
    void MainMenuDraw(const Game::Info& Info);
    void GameOverUpdate(Game::Info& Info);
    void GameOverDraw(const Game::Info& Info);
    void Transition(Game::Info& Info);

    HUD InitializeHud(const GameTexture& Textures);
    Character InitializeFox(const Window& Window, Game::Info& Info, const GameTexture& Textures, const GameAudio& Audio);
    std::array<Sprite,5> InitializePauseFox(const GameTexture& Textures);
    std::array<const Texture2D,9> InitializeButtons(const GameTexture& Textures);
    std::vector<std::vector<Prop>> InitializePropsUnder(const GameTexture& Textures);
    std::vector<std::vector<Prop>> InitializePropsOver(const GameTexture& Textures);
    std::vector<Prop> InitializeTrees(const GameTexture& Textures);
    std::vector<Enemy> InitializeEnemies(Background& MapBG, const Window& Window, const GameTexture& Textures, Randomizer& RandomEngine);
    std::vector<Enemy> InitializeCrows(Background& MapaBG, const Window& Window, const GameTexture& Textures, Randomizer& RandomEngine);

    // Debugging --------------------
    void DrawCollisionRecs(Prop& Prop, const Vector2 CharacterWorldPos, Color RecColor = {0, 121, 241, 150});
    template <typename Object>
    void DrawCollisionRecs(Object& Type, Color RecColor = {0, 238, 135, 150});
    template <typename Object>
    void DrawAttackRecs(Object& Type, Color RecColor = {240, 255, 20, 150});
}

#endif // GAME_HPP