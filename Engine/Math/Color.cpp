#include "Color.h"

#include <string>

namespace Wannabe
{
    Color::Color(unsigned char r, unsigned char g, unsigned char b)
        :r(r), g(g), b(b)
    {

    }

    std::wstring Color::ToAnsiFG() const
    {
        return L"\x1b[38;2;" + std::to_wstring(r) + L";" +
            std::to_wstring(g) + L";" + std::to_wstring(b) + L"m";
    }

    std::wstring Color::Reset()
    {
        return L"\x1b[0m";
    }

    // 프리셋 실제 정의
    const Color Color::Black = { 0, 0, 0 };
    const Color Color::White = { 255, 255, 255 };
    const Color Color::Red = { 255, 80, 80 };
    const Color Color::Green = { 80, 255, 80 };
    const Color Color::Blue = { 80, 80, 255 };
    const Color Color::Cyan = { 0, 255, 255 };
    const Color Color::Magenta = { 255, 0, 255 };
    const Color Color::Yellow = { 255, 255, 0 };
    const Color Color::Purple = { 160, 32, 240 };
}