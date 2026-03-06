#pragma once
#include <vector>

#include "Common/Common.h"

namespace Wannabe
{
    class UI;

    class WANNABE_API UIManager
    {
    public:
        // Meyers Singleton
        static UIManager& Get()
        {
            static UIManager instance;
            return instance;
        }

        // 복사 및 이동 금지
        UIManager(const UIManager&) = delete;
        UIManager& operator=(const UIManager&) = delete;

        void Register(UI* ui);
        void Unregister(UI* ui);

        void OnViewportChanged();

    private:
        UIManager() = default;
        ~UIManager() = default;

    private:
        std::vector<UI*> m_vecUI;
    };
}