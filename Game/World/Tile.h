#pragma once
#include "Actor/Actor.h"

enum class TileType
{
    Ground,
    Wall,
    Bush,
    Portal,
    Event
};

struct Tile
{
    TileType eType = TileType::Ground;
    bool bWalkable = true;

    Actor* pOccupiedActor = nullptr;
};