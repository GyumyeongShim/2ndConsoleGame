#pragma once
#include "Actor/Actor.h"

enum class TileType
{
    Ground,
    Wall,
    Water,
    Road,
    Bush,
    Portal,
    Event
};

struct Tile
{
    TileType eType = TileType::Ground;
    bool bWalkable = true;
    std::wstring strSymbol;
};