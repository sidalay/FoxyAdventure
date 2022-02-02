#ifndef GAME_HPP
#define GAME_HPP

#include <raylib.h>
#include <string>
#include <vector>
#include "headers/hud.hpp"
#include "headers/npc.hpp"
#include "headers/prop.hpp"
#include "headers/sprite.hpp"
#include "headers/window.hpp"
#include "headers/enemy.hpp"
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
    void Tick(Window& Window, Background& Map, Character& Character, Props& Props, HUD& Hud, std::vector<Enemy>& Enemies, std::vector<NPC>& NPCS);
    void CheckScreenSizing(Window& Window);
    void SetFullScreen(Window& Window);
    void Update(Background& Map, Character& Character, Props& Props, std::vector<Enemy>& Enemies, std::vector<NPC>& NPCS);
    void Draw(Background& Map, Character& Character, Props& Props, HUD& Hud, std::vector<Enemy>& Enemies, std::vector<NPC>& NPCS);

    std::vector<std::vector<Prop>> InitializePropsUnder();
    std::vector<std::vector<Prop>> InitializePropsOver();
}

#endif