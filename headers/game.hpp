#ifndef GAME_HPP
#define GAME_HPP

#include <raylib.h>
#include <string>
#include "headers/sprite.hpp"
#include "headers/window.hpp"
#include "headers/character.hpp"
#include "headers/background.hpp"

enum class GameState 
{
    MAINMENU, RUNNING, PAUSED, GAMEOVER
};

namespace Game 
{
    void Run();
    void Initialize(Window& Window, int FPS, std::string Title);
    void Tick(Window& Window, Background& Map, Character& Character);
    void CheckScreenSizing(Window& Window);
    void SetFullScreen(Window& Window);
    void Update(Background& Map, Character& Character);
    void Draw(Background& Map, Character& Character);
}

#endif