#include "UI_ActorInfo.h"

#include "Enum/CombatType.h"
#include "Render/RenderSystem.h"
#include "Battle/TurnManager.h"
#include "Actor/BattleActor.h"
#include "Component/StatComponent.h"
#include "Component/DisplayComponent.h"
#include "Component/StatusComponent.h"

UI_ActorInfo::UI_ActorInfo(Wannabe::TurnManager* pTurnManager, Wannabe::Actor* pTarget, bool bIsPlayer)
	:super(L"",Wannabe::Color::White), m_pTurnManager(pTurnManager), m_pTargetActor(pTarget), m_bIsPlayer(bIsPlayer)
{
}

UI_ActorInfo::~UI_ActorInfo()
{
}

void UI_ActorInfo::Tick(float fDeltaTime)
{
	if (m_pTargetActor == nullptr || m_pTargetActor->IsDestroyRequested())
		return;

    auto* pStat = m_pTargetActor->GetComponent<StatComponent>();
    auto* pDisplay = m_pTargetActor->GetComponent<DisplayComponent>();
    auto* pStatus = m_pTargetActor->GetComponent<Wannabe::StatusComponent>();
    if (!pStat || !pDisplay)
        return;

    bool bMyTurn = IsMyTurn();
    std::wstring prefix = bMyTurn ? L"▶ " : L"  ";

    m_infoStr = prefix + pDisplay->GetOriginName();
    if (m_bIsPlayer)
    {
        m_infoStr += L" LV" + std::to_wstring(pStat->GetLevel());
    }

    if (pStatus && pStatus->HasStatus(StatusType::Stun))
    {
        m_Color = Wannabe::Color::Gray; // 기절 시 회색
    }
    else
    {
        m_Color = bMyTurn ? Wannabe::Color::Yellow : Wannabe::Color::White;
    }

    RecalculateViewportPosition();
}

void UI_ActorInfo::Draw(Wannabe::RenderSystem& renderSys)
{
    if (m_infoStr.empty() || m_pTargetActor == nullptr || m_pTargetActor->IsDestroyRequested())
        return;

    Wannabe::Vector2 actorScreenPos = renderSys.GetCamera().WorldToScreen(m_pTargetActor->GetPosition());

    auto* pStat = m_pTargetActor->GetComponent<StatComponent>();
    auto* pStatus = m_pTargetActor->GetComponent<Wannabe::StatusComponent>();

    if (m_bIsPlayer)
    {
        // 플레이어: 아스키 아트 하단(보통 Y+3)부터 정보 출력
        Wannabe::Vector2 drawPos = { actorScreenPos.x, actorScreenPos.y + 2 };

        renderSys.DrawUI(m_infoStr, drawPos, m_Color, m_SortingOrder);

        Wannabe::Color hpColor;
        std::wstring hpBar = L"HP:" + MakeGaugeBar(pStat->GetHp(), pStat->GetMaxHp(), 5, hpColor);
        renderSys.DrawUI(hpBar, { drawPos.x, drawPos.y + 1 }, hpColor, m_SortingOrder);

        //Wannabe::Color mpColor = Wannabe::Color::Cyan;
        //std::wstring mpBar = L"MP:" + MakeGaugeBar(pStat->GetHp(), pStat->GetMaxHp(), 5, mpColor);
        //renderSys.DrawUI(mpBar, { drawPos.x, drawPos.y + 2 }, mpColor, m_SortingOrder);
        if (pStatus)
        {
            const auto& states = pStatus->GetCurStatusState();
            for (size_t i = 0; i < states.size(); ++i)
            {
                Wannabe::Color statusColor = GetStatusColor(states[i].eStatusType);
                renderSys.DrawUI(L"■", { drawPos.x + (static_cast<int>(i) * 2), drawPos.y + 3 }, statusColor, m_SortingOrder);
            }
        }
    }
    else
    {
        // 몬스터: 아스키 아트 상단(Y-2)부터 위로 쌓듯이 출력
        Wannabe::Vector2 drawPos = { actorScreenPos.x, actorScreenPos.y - 2 };

        renderSys.DrawUI(m_infoStr, drawPos, m_Color, m_SortingOrder);

        Wannabe::Color hpColor;
        std::wstring hpBar = L"HP:" + MakeGaugeBar(pStat->GetHp(), pStat->GetMaxHp(), 5, hpColor);
        renderSys.DrawUI(hpBar, { drawPos.x, drawPos.y + 1 }, hpColor, m_SortingOrder);
        
        if (pStatus)
        {
            const auto& states = pStatus->GetCurStatusState();
            for (size_t i = 0; i < states.size(); ++i)
            {
                Wannabe::Color statusColor = GetStatusColor(states[i].eStatusType);
                renderSys.DrawUI(L"■", { drawPos.x + (static_cast<int>(i) * 2), drawPos.y + 3 }, statusColor, m_SortingOrder);
            }
        }
    }
}

void UI_ActorInfo::RecalculateViewportPosition()
{
    m_cachedViewportPos = GetViewportPosition();
}

void UI_ActorInfo::Init()
{
    m_SortingOrder = 100;
}

bool UI_ActorInfo::IsMyTurn()
{
    bool bIsMyTurn = (m_pTurnManager->GetCurBattleActor() == m_pTargetActor);
    return bIsMyTurn;
}

std::wstring UI_ActorInfo::MakeGaugeBar(int iCur, int iMax, int iWidth, Wannabe::Color& outColor)
{
    float percent = (iMax > 0) ? static_cast<float>(iCur) / iMax : 0.0f;
    int fillCount = static_cast<int>(percent * iWidth);

    // 색상 변화
    if (percent > 0.6f)
        outColor = Wannabe::Color::Green;
    else if (percent > 0.3f)
        outColor = Wannabe::Color::BrightOrange;
    else
        outColor = Wannabe::Color::BrightRed;

    // [||| ], [||   ]
    std::wstring bar = L"[";
    for (int i = 0; i < iWidth; ++i)
    {
        bar += (i < fillCount) ? L"|" : L" ";
    }
    bar += L"] " + std::to_wstring(iCur) + L"/" + std::to_wstring(iMax);

    return bar;
}