#include "Canvas.h"

using namespace Wannabe;

static bool IsWideChar(wchar_t ch) // 전각(2칸) 문자 판별
{
    // 한글 자모
    if (ch >= 0x1100 && ch <= 0x11FF) return true;

    // 한글 완성형
    if (ch >= 0xAC00 && ch <= 0xD7A3) return true;

    // CJK 호환 한자
    if (ch >= 0x2E80 && ch <= 0x9FFF) return true;

    return false;
}

Canvas::Canvas()
    :m_iHeight(0), m_iWidth(0)
{
}

Canvas::Canvas(int iWidth, int iHeight)
    :m_iWidth(iWidth), m_iHeight(iHeight)
{
    m_vecBuffer.resize(iWidth * iHeight);
}

Canvas::~Canvas()
{
}

void Canvas::DrawAscii(int iX, int iY, wchar_t wch, Color color, int iSortingOrder)
{
    if (iX < 0 || iY < 0 || iX >= m_iWidth || iY >= m_iHeight) //좌표 체크
        return;

    int idx = Idx(iX, iY);
    CanvasCell& cell = m_vecBuffer[idx];
    if (cell.sortingOrder <= iSortingOrder) //layer 정리
    {
        cell.wch = wch;
        cell.color = color;
        cell.sortingOrder = iSortingOrder;
        cell.bDirty = true;
    }

    if (IsWideChar(wch) && iX + 1 < m_iWidth) // 전각이면 다음 칸도 점유
    {
        int idx2 = Idx(iX + 1, iY);
        auto& cell2 = m_vecBuffer[idx2];

        if (cell2.sortingOrder <= iSortingOrder)
        {
            cell2.wch = L' ';
            cell2.color = color;
            cell2.sortingOrder = iSortingOrder;
            cell2.bDirty = true;
        }
    }
}

void Canvas::DrawTxt(int iX, int iY, const std::wstring& txt, Color color, int iSortingOrder)
{
    int drawX = iX;
    for (wchar_t wch : txt)
    {
        DrawAscii(drawX, iY, wch, color, iSortingOrder);
        if (IsWideChar(wch))
        {
            drawX += 1;
            DrawAscii(drawX, iY, L' ', color, iSortingOrder);
        }

        drawX += 1;
    }
}

void Canvas::Resize(int iX, int iY)
{
    m_iWidth = iX;
    m_iHeight = iY;

    m_vecBuffer.clear();
    m_vecBuffer.resize(iX * iY);

    for (auto& cell : m_vecBuffer)
    {
        cell.wch = L' ';
        cell.color = Color::White;
        cell.sortingOrder = -1;
        cell.bDirty = true;
    }
}

void Canvas::Clear()
{
    for (auto& cell : m_vecBuffer)
    {
        //상태가 변한 경우에만 dirty 발생
        if (cell.wch != L' ' ||
            cell.color != Color::White ||
            cell.sortingOrder != -1)
        {
            cell.wch = L' ';
            cell.color = Color::White;
            cell.sortingOrder = -1;
            cell.bDirty = true;
        }
    }
}

void Canvas::SetCell(int iX, int iY, const CanvasCell& newCell)
{
    if (iX < 0 || iY < 0 || iX >= m_iWidth || iY >= m_iHeight)
        return;

    int idx = iY * m_iWidth + iX;
    //기존과 다르다면 cell 교체
    if (m_vecBuffer[idx].wch != newCell.wch ||
        m_vecBuffer[idx].color != newCell.color)
    {
        m_vecBuffer[idx] = newCell;
        m_vecBuffer[idx].bDirty = true;
    }
}

const CanvasCell& Canvas::GetCell(int iX, int iY) const
{
    return m_vecBuffer[Idx(iX, iY)];
}

CanvasCell& Wannabe::Canvas::GetMutableCell(int iX, int iy)
{
    return m_vecBuffer[Idx(iX, iy)];
}

int Canvas::Idx(int x, int y) const
{
    return y * m_iWidth + x;
}