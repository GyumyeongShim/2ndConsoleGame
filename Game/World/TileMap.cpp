#include <fstream>
#include "TileMap.h"
#include "Render/RenderSystem.h"

void TileMap::Draw(Wannabe::RenderSystem& renderSys)
{
    Camera& cam = renderSys.GetCamera();
    auto& canvas = renderSys.GetWorldCanvas();

    int camX = (int)cam.GetPosition().x;
    int camY = (int)cam.GetPosition().y;

    for (int screenY = 0; screenY < cam.GetHeight(); ++screenY)
    {
        for (int screenX = 0; screenX < cam.GetWidth(); ++screenX)
        {
            int worldX = camX + screenX;
            int worldY = camY + screenY;

            if (!IsValidTile(worldX, worldY))
                continue;

            const Tile* tile = GetTile(worldX, worldY);
            if (!tile)
                continue;

            wchar_t ch = L'.';

            // 타일에 맞는 문자
            switch (tile->eType)
            {
            case TileType::Wall:
                ch = L'@';
                break;

            case TileType::Bush:
                ch = L'=';
                break;

            case TileType::Ground:
                ch = L'.';
                break;
            }
            renderSys.GetWorldCanvas().DrawTxt(screenX, screenY, std::wstring(1, ch), Color::White, 0);
        }
    }
}

void TileMap::Init(int iWidth, int iHeight)
{
    m_iWidth = iWidth;
    m_iHeight = iHeight;

    m_vecTiles.clear();
    m_vecTiles.resize(iWidth * iHeight);
}

bool TileMap::IsValidTile(int x, int y)
{
    return !(x < 0 || y < 0 ||
        x >= m_iWidth ||
        y >= m_iHeight);
}

bool TileMap::IsWalkable(int x, int y) const
{
    if (x < 0 || y < 0 || x >= m_iWidth || y >= m_iHeight)
        return false;

    return m_vecTiles[y * m_iWidth + x].bWalkable;
}

void TileMap::LoadFromFile(const std::string& path)
{
    std::string fullPath = "../" + path;

    std::ifstream file(fullPath);

    if (!file.is_open())
    {
        std::cout << "Map file open failed : " << fullPath << std::endl;
        return;
    }

    std::string line;
    int y = 0;

    while (std::getline(file, line))
    {
        if (!line.empty() && line.back() == '\r')
            line.pop_back();

        if (m_iWidth == 0)
        {
            m_iWidth = (int)line.size();
        }

        if (m_vecTiles.size() < (y + 1) * m_iWidth)
        {
            m_vecTiles.resize((y + 1) * m_iWidth);
        }

        for (int x = 0; x < line.size(); x++)
        {
            if (!IsValidTile(x, y))
                continue;

            Tile tile;

            switch (line[x])
            {
            case '@':
                tile.eType = TileType::Wall;
                tile.bWalkable = false;
                break;

            case '.':
                tile.eType = TileType::Ground;
                break;

            case '=':
                tile.eType = TileType::Bush;
                break;
            }

            m_vecTiles[y * m_iWidth + x] = tile;
        }

        y++;
    }

    m_iHeight = y;
}

const Tile* TileMap::GetTile(int x, int y)
{
    if (IsValidTile(x, y) == false)
        return nullptr;

    return &m_vecTiles[y * m_iWidth + x];
}

Vector2 TileMap::WorldToTile(const Vector2& worldPos) const
{
    return Vector2((int)worldPos.x,(int)worldPos.y);
}

Vector2 TileMap::TileToWorld(const Vector2& tilePos) const
{
    return tilePos;
}
