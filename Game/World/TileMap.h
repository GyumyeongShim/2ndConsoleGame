#pragma once
#include <vector>
#include <string>

#include "Math/Vector2.h"
#include "World/Tile.h"

class Wannabe::RenderSystem;

class TileMap
{
public:
    void Draw(Wannabe::RenderSystem& renderSys);

    void Init(int iWidth, int iHeight);
    bool IsValidTile(int x, int y);
    bool IsWalkable(int x, int y) const;

    void LoadFromFile(const std::string& path);
    
    bool IsInMap(int x, int y) const;

    const Tile* GetTile(int x, int y);
    Wannabe::Vector2 WorldToTile(const Wannabe::Vector2& worldPos) const;
    Wannabe::Vector2 TileToWorld(const Wannabe::Vector2& tilePos) const;
    std::vector<Wannabe::Vector2> FindPath(const Wannabe::Vector2& start, const Wannabe::Vector2& end);
private:
	int m_iWidth = 0;
	int m_iHeight = 0;

	std::vector<Tile> m_vecTiles;
};