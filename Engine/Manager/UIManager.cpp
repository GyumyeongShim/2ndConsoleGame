#include "UIManager.h"
#include "UI/UI.h"

namespace Wannabe
{
    void UIManager::Register(UI* ui)
    {
        if (ui == nullptr)
            return;

        // 중복 등록 방지
        auto it = std::find(m_vecUI.begin(), m_vecUI.end(), ui);
        if (it == m_vecUI.end())
        {
            m_vecUI.emplace_back(ui);
        }
    }

    void UIManager::Unregister(UI* ui)
    {
        if (ui == nullptr)
            return;

        auto it = std::find(m_vecUI.begin(), m_vecUI.end(), ui);
        if (it != m_vecUI.end())
            m_vecUI.erase(it);
    }

    void UIManager::OnViewportChanged()
    {
        for (UI* ui : m_vecUI)
        {
            if (ui != nullptr && ui->IsDestroyRequested() == false)
            {
                ui->RecalculateViewportPosition();
            }
        }
    }
}