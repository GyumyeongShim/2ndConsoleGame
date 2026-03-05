#pragma once
#include <string>
#include "Interface/IUIAnimation.h"

class UI_Dialogue;

class TypingAnimation : public Wannabe::IUIAnimation
{
public:
    TypingAnimation(UI_Dialogue* owner, const std::wstring& fullText, float charInterval = 0.05f);

    void Tick(float fDeltaTime) override;
    bool IsFinished() const override { return m_bFinished; }

private:
    UI_Dialogue* m_pOwner = nullptr;

    std::wstring m_wstrFullTxt;
    std::wstring m_wstrCurTxt;

    float m_fInterval = 0.f;
    float m_fTimer = 0.f;

    int m_iIdx = 0;
    bool m_bFinished = false;
};