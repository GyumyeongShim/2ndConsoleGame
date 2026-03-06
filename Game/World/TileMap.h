#pragma once
#include <vector>
#include "Render/RenderSystem.h"
#include "Math/Vector2.h"
#include "World/Tile.h"

class TileMap
{
public:
    void Draw(Wannabe::RenderSystem& renderSys);

    void Init(int iWidth, int iHeight);
    bool IsValidTile(int x, int y);
    bool IsWalkable(int x, int y) const;

    void LoadFromFile(const std::string& path);
    
    const Tile* GetTile(int x, int y);
    Vector2 WorldToTile(const Vector2& worldPos) const;
    Vector2 TileToWorld(const Vector2& tilePos) const;

private:
	int m_iWidth = 0;
	int m_iHeight = 0;

	std::vector<Tile> m_vecTiles;
};