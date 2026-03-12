#pragma once
#include "UI/UI.h"

class UI_BattleResult : public Wannabe::UI
{
public:
    enum class DisplayState 
    { 
        Background, 
        Stats, 
        Rank, 
        Finished 
    };

    UI_BattleResult(int iGold, int iExp, char rank = 'A');

    virtual void Tick(float fDeltaTime) override;
    virtual void Draw(Wannabe::RenderSystem& renderSys) override;
    virtual void RecalculateViewportPosition() override;

    DisplayState GetDisplayState() { return m_eDisplayState; }
    void SetExit(bool bIsExit) { m_bExit = bIsExit; }
    bool Exit() const { return m_bExit; }

private:
    DisplayState m_eDisplayState = DisplayState::Background; // 연출용 변수
    float m_fTimer = 0.0f;
    bool m_bExit = false;
    
    int m_iCurGold = 0;
    int m_iTargetGold = 0;
    
    int m_iCurExp = 0;
    int m_iTargetExp = 0;

    char m_rank = 'b';
};