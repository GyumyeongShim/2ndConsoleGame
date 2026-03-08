#pragma once
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <string>

#include "Core/Common.h"

namespace Wannabe
{
    struct WANNABE_API Color
	{
        unsigned char r, g, b;

        Color() : r(255), g(255), b(255) {}
        Color(unsigned char r, unsigned char g, unsigned char b);
         // 1. 색상 비교 (RenderSystem에서 최적화용으로 사용)
        bool operator==(const Color & other) const
        {
            return (r == other.r && g == other.g && b == other.b);
        }

        bool operator!=(const Color& other) const
        {
            return !(*this == other);
        }

        // 2. ANSI 전경색 문자열 생성 (FG: 38;2;R;G;B)
        std::wstring ToAnsiFG() const;
        static std::wstring Reset();

        // 3. 자주 사용하는 색상 프리셋
        static const Color Black;
        static const Color White;
        static const Color Red;
        static const Color Green;
        static const Color Blue;
        static const Color Cyan;
        static const Color Magenta;
        static const Color Yellow;
        static const Color Purple; // 우리가 원하던 RGB 보라색
    };
}