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

    const Color Color::BrightRed = { 255, 120, 120 };
    const Color Color::BrightGreen = { 120, 255, 120 };
    const Color Color::BrightBlue = { 120, 120, 255 };

    const Color Color::BrightCyan = { 120, 255, 255 };
    const Color Color::BrightMagenta = { 255, 120, 255 };
    const Color Color::BrightYellow = { 255, 255, 120 };
    const Color Color::BrightPurple = { 200, 120, 255 };

    const Color Color::DarkGray = { 64, 64, 64 };
    const Color Color::Gray = { 128, 128, 128 };
    const Color Color::LightGray = { 192, 192, 192 };
    const Color Color::BrightGray = { 230, 230, 230 };

    const Color Color::Orange = { 255, 165, 0 };
    const Color Color::BrightOrange = { 255, 200, 80 };
    const Color Color::Pink = { 255, 105, 180 };
    const Color Color::BrightPink = { 255, 170, 210 };
    const Color Color::SkyBlue = { 135, 206, 235 };
    const Color Color::BrightSkyBlue = { 180, 230, 255 };
    const Color Color::Lime = { 140, 255, 0 };
    const Color Color::BrightLime = { 190, 255, 80 };
    const Color Color::Gold = { 255, 200, 40 };
    const Color Color::BrightGold = { 255, 230, 120 };
    const Color Color::Teal = { 0, 160, 160 };
    const Color Color::BrightTeal = { 80, 220, 220 };
}