#include <fstream>
#include "TileMap.h"

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

            renderSys.GetWorldCanvas().DrawTxt(screenX, screenY, tile->strSymbol, Color::White, 0);
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

    int index = (y * m_iWidth) + x;

    if (index < 0 || index >= (int)m_vecTiles.size())
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

    m_vecTiles.clear();
    m_iWidth = 0;
    m_iHeight = 0;

    std::string line;
    std::vector<std::string> lines;
    while (std::getline(file, line))
    {
        if (!line.empty() && line.back() == '\r')
            line.pop_back();

        // 빈 줄 무시 방지 (공백 줄도 맵의 일부일 수 있음)
        lines.push_back(line);
        if ((int)line.size() > m_iWidth)
            m_iWidth = (int)line.size();
    }
    m_iHeight = (int)lines.size();

    m_vecTiles.resize(m_iWidth * m_iHeight);

    for (int y = 0; y < m_iHeight; ++y)
    {
        for (int x = 0; x < m_iWidth; ++x)
        {
            Tile tile;
            tile.bWalkable = true;

            char symbol = (x < (int)lines[y].size()) ? lines[y][x] : ' ';

            switch (symbol) 
            {
            case '#': // 외곽 벽
                tile.eType = TileType::Wall;
                tile.bWalkable = false;
                tile.strSymbol = L"\u2593"; // wstring 출력용
                break;
            case 'A': case 'B': case 'C': // 건물 벽
                tile.eType = TileType::Wall;
                tile.bWalkable = false;
                tile.strSymbol = L"\u2588\u2588";
                break;
            case '.': // 길
                tile.eType = TileType::Road;
                tile.strSymbol = L"\u2591\u2591";
                break;
            case 'T': // 나무
                tile.eType = TileType::Bush;
                tile.bWalkable = false;
                tile.strSymbol = L"\u2663";
                break;
            case '~': case 'W': // 물
                tile.eType = TileType::Water;
                tile.bWalkable = false;
                tile.strSymbol = L"\u2248\u2248";
                break;
            case '*': // 광장 중앙/이벤트
                tile.eType = TileType::Event;
                tile.strSymbol = L"\u2605";
                break;
            default: // 빈 공간
                tile.eType = TileType::Ground;
                tile.strSymbol = L"  ";
                break;

            }

            m_vecTiles[y * m_iWidth + x] = tile;
        }
    }
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