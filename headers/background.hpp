#ifndef BACKGROUND_HPP
#define BACKGROUND_HPP

#include "gametextures.hpp"
#include "gameaudio.hpp"
#include <raymath.h>

enum class Direction
{
    UP, DOWN, LEFT, RIGHT
};

enum class Area
{
    FOREST, DUNGEON
};

class Background
{
public:
    explicit Background(const GameTexture& GameTextures, const GameAudio& Audio);
    ~Background() = default;
    constexpr Background(const Background&) = delete;
    constexpr Background(Background&&) = default;
    constexpr Background& operator=(const Background&) = delete;
    constexpr Background& operator=(Background&&) = default;

    void Tick(const Vector2 WorldPos);
    void DrawForest();
    void DrawDungeon();
    void DrawMiniMap(const Vector2 CharWorldPos);
    void SetArea(const Area& NextMap) {Map = NextMap;}
    Vector2 GetForestMapSize();
    Vector2 GetDungeonMapSize();
    constexpr Vector2 GetMapPos() const {return MapPos;}
    constexpr bool IsMiniMapOn() const {return MiniMapOpen;}
    constexpr Area GetArea() const {return Map;}
    constexpr float GetScale() const {return Scale;}
    
private:
    const GameTexture& GameTextures;
    const GameAudio& Audio;
    Vector2 MapPos{0.f, 0.f};
    float Scale{4.f};
    Area Map{Area::FOREST};
    bool MiniMapOpen{false};
};

#endif // BACKGROUND_HPP