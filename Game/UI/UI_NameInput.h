#pragma once
#include <vector>
#include <string>
#include <functional>

#include "UI/UI.h"

class UI_NameInput : public Wannabe::UI
{
    RTTI_DECLARATIONS(UI_NameInput, UI)

public:
    UI_NameInput();
    virtual ~UI_NameInput() = default;

public:
    virtual void Tick(float fDeltaTime) override;
    virtual void Draw(Wannabe::RenderSystem& renderSys) override;
    virtual void RecalculateViewportPosition() override;

    void Init();

    // 입력 관련 제어
    void MoveCursor(int iX, int iY);
    void OnConfirmAction();

    // 결과값 제어
    const std::wstring& GetInputName() const { return m_wstrCurrentName; }
    void ClearName() { m_wstrCurrentName.clear(); }
    void SetActive(bool bActive) { m_bIsActive = bActive; }

    // 콜백 설정
    void SetOnConfirm(std::function<void(std::wstring)> func) { m_OnConfirm = func; }

private:
    std::vector<std::wstring> m_vecChars; // 선택 가능한 문자 리스트
    std::wstring m_wstrCurrentName;      // 현재 입력된 이름

    bool m_bIsActive = false;
    int m_iCursorIdx = 0;

    int m_iColumnCount = 7;     // 가로로 보여줄 글자 수
    int m_iMaxNameLength = 10;  // 최대 이름 길이
    int m_iCellWidth = 6;       // 한 칸의 가로 간격

    std::function<void(std::wstring)> m_OnConfirm;
};

