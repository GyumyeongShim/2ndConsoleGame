#include "TypingAnimation.h"
#include "UI/UI_Dialogue.h"

TypingAnimation::TypingAnimation(UI_Dialogue* owner, const std::wstring& text, float charInterval)
    : m_pOwner(owner), m_wstrFullTxt(text), m_fInterval(charInterval)
{
    m_wstrCurTxt.clear();
}

void TypingAnimation::Tick(float fDeltaTime)
{
    if (m_bFinished)
        return;

    m_fTimer += fDeltaTime;

    if (m_fTimer < m_fInterval)
        return;

    m_fTimer = 0.f;

    if (m_iIdx >= (int)m_wstrFullTxt.size())
    {
        m_bFinished = true;
        return;
    }

    m_wstrCurTxt.push_back(m_wstrFullTxt[m_iIdx]);
    m_iIdx++;

    if (m_pOwner == nullptr)
        return;

    m_pOwner->ChangeTxt(m_wstrCurTxt.c_str());
}