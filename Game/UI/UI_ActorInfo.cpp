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
	if (m_pTargetActor == nullptr || !m_pTargetActor->IsDestroyRequested())
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
}

void UI_ActorInfo::Draw(Wannabe::RenderSystem& renderSys)
{
    if (m_infoStr.empty() || m_pTargetActor == nullptr)
        return;

    Wannabe::Vector2 pos = m_cachedViewportPos;

    // 이름
    renderSys.DrawUI(m_infoStr, pos, m_Color, m_SortingOrder);

    auto* pStat = m_pTargetActor->GetComponent<StatComponent>();
    auto* pStatus = m_pTargetActor->GetComponent<Wannabe::StatusComponent>();

    // Hp
    Wannabe::Color hpBarColor;
    std::wstring hpGauge = L"HP:" + MakeGaugeBar(pStat->GetHp(), pStat->GetMaxHp(), 5, hpBarColor);
    renderSys.DrawUI(hpGauge, { pos.x, pos.y + 1 }, Wannabe::Color::White, m_SortingOrder);
    if (m_bIsPlayer)
    {
        Wannabe::Color mpBarColor = Wannabe::Color::Cyan;
        std::wstring mpGauge = L" MP:" + MakeGaugeBar(pStat->GetHp(), pStat->GetMaxHp(), 5, mpBarColor);
        renderSys.DrawUI(mpGauge, { pos.x, pos.y + 2 }, Wannabe::Color::White, m_SortingOrder);
    }

    // 상태 이상 슬롯
    if (pStatus)
    {
        const auto& states = pStatus->GetCurStatusState();
        for (size_t i = 0; i < states.size(); ++i)
        {
            Wannabe::Color statusColor = GetStatusColor(states[i].eStatusType);
            renderSys.DrawUI(L"■", { pos.x + (static_cast<int>(i) * 2), pos.y + 3 }, statusColor, m_SortingOrder);
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
    std::wstring bar = L"[";
    float percent = (iMax > 0) ? static_cast<float>(iCur) / iMax : 0.0f;
    int fillCount = static_cast<int>(percent * iWidth);

    // ■■■■ ], ■■   ]
    bar += L"[";
    for (int i = 0; i < iWidth; ++i)
    {
        bar += (i < fillCount) ? L"■" : L" ";
    }
    bar += L"]";

    return bar;

    // 색상 변화
    if (percent > 0.6f) 
        outColor = Wannabe::Color::Red;
    else if (percent > 0.3f) 
        outColor = Wannabe::Color::Orange;
    else 
        outColor = Wannabe::Color::Pink;

    return bar;
}