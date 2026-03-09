#pragma once
#include <vector>
#include "UI/UI.h"

class UI_TurnOrder;
class UI_MenuList;
class UI_ActorInfo;
class UI_Dialogue;
class BattleLogSystem;
class UI_BattleResult;

namespace Wannabe
{
    class TurnManager;
    class Actor;
}

class UI_BattleLayout : public Wannabe::UI
{
	RTTI_DECLARATIONS(UI_BattleLayout, UI)
public:
    UI_BattleLayout();
    virtual ~UI_BattleLayout();

    virtual void Tick(float fDeltaTime) override;
    virtual void Draw(Wannabe::RenderSystem& renderSys) override;
    virtual void RecalculateViewportPosition() override;
    virtual void SetRenderSystem(Wannabe::RenderSystem* renderSystem) { m_pRenderSystem = renderSystem; }

    void SetupBattleUI(Wannabe::TurnManager* pTurnMgr, BattleLogSystem* pLogSys,
        const std::vector<Wannabe::Actor*>& monsters, const std::vector<Wannabe::Actor*>& players);

    void ShowBattleResult(int iGold, int iExp, char rank);

    UI_MenuList* GetCommandMenu() { return m_pCommandMenu; }
    UI_TurnOrder* GetTurnOrder() { return m_pTurnOrderUI; }
    UI_BattleResult* GetResultUI() { return m_pResultUI; }

private:
    void Init();
    void ClearChildren();
    void UpdateLogs();

private:
    Wannabe::TurnManager* m_pTurnMgr = nullptr;
    UI_TurnOrder* m_pTurnOrderUI = nullptr;
    UI_MenuList* m_pCommandMenu = nullptr;
    BattleLogSystem* m_pLogSys = nullptr;
    UI_BattleResult* m_pResultUI = nullptr;

    std::vector<UI_ActorInfo*> m_vecMonsterInfos;
    std::vector<UI_ActorInfo*> m_vecPlayerInfos;

    std::vector<UI_Dialogue*> m_vecLogs; // 여러 줄의 로그를 위해 리스트화
    const int MAX_LOG_LINES = 4;
};