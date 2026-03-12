#include "UI_BattleLayout.h"

#include "Render/RenderSystem.h"
#include "Battle/TurnManager.h"
#include "Battle/BattleLogSystem.h"
#include "Util/Utill.h"

#include "Actor/Actor.h"
#include "UI/UI_TurnOrder.h"
#include "UI/UI_MenuList.h"
#include "UI/UI_Dialogue.h"
#include "UI/UI_ActorInfo.h"
#include "UI/UI_BattleResult.h"

UI_BattleLayout::UI_BattleLayout()
	: super(L"", Wannabe::Color::White)
{
}

UI_BattleLayout::~UI_BattleLayout()
{
	ClearChildren();
}

void UI_BattleLayout::Tick(float fDeltaTime)
{
	super::Tick(fDeltaTime);

	UpdateLogs();

	if (m_pTurnOrderUI) 
		m_pTurnOrderUI->Tick(fDeltaTime);

	if (m_pCommandMenu) 
		m_pCommandMenu->Tick(fDeltaTime);

	for (auto* info : m_vecMonsterInfos)
	{
		if (info == nullptr)
			continue;

		info->Tick(fDeltaTime);
	}

	for (auto* info : m_vecPlayerInfos)
	{
		if (info == nullptr)
			continue;

		info->Tick(fDeltaTime);
	}

	for (auto* log : m_vecLogs)
	{
		if (log == nullptr)
			continue;

		log->Tick(fDeltaTime);
	}

	if (m_pResultUI)
		m_pResultUI->Tick(fDeltaTime);
}

void UI_BattleLayout::Draw(Wannabe::RenderSystem& renderSys)
{
	Wannabe::Vector2 screen = renderSys.GetScreenSize();
	Wannabe::Vector2 myPos = GetPosition();

	if (m_pTurnOrderUI) 
		m_pTurnOrderUI->Draw(renderSys);

	// 상단 영역
	renderSys.DrawUI(std::wstring(screen.x, L'-'), { 0, 3 }, Wannabe::Color::White, 50);

	// 정보창
	for (auto* info : m_vecMonsterInfos)
	{
		if (info == nullptr)
			continue;

		info->Draw(renderSys);
	}

	for (auto* info : m_vecPlayerInfos)
	{
		if (info == nullptr)
			continue;

		info->Draw(renderSys);
	}

	// log 영역
	renderSys.DrawUI(std::wstring(screen.x, L'-'), { 0, 28 }, Wannabe::Color::White, 100);

	if (m_pCommandMenu) 
		m_pCommandMenu->Draw(renderSys);

	for (auto* log : m_vecLogs)
	{
		if (log == nullptr)
			continue;

		log->Draw(renderSys);
	}

	if (m_pResultUI)
		m_pResultUI->Draw(renderSys);
}

void UI_BattleLayout::RecalculateViewportPosition()
{
}

void UI_BattleLayout::SetupBattleUI(Wannabe::TurnManager* pTurnMgr, BattleLogSystem* pLogSys,
	const std::vector<Wannabe::Actor*>& monsters, const std::vector<Wannabe::Actor*>& players)
{
	ClearChildren();
	
	// 턴 매니저
	m_pTurnMgr = pTurnMgr;

	// 로그 시스템
	m_pLogSys = pLogSys;

	// 턴 순서
	m_pTurnOrderUI = new UI_TurnOrder(pTurnMgr, { 2, 1 });
	m_pTurnOrderUI->SetRenderSystem(m_pRenderSystem);
	m_pTurnOrderUI->Init();
	m_pTurnOrderUI->SetOffset({ 2, 1 });

	int startX = 10;
	int gap = 32;

	// 몬스터 정보창
	for (size_t i = 0; i < monsters.size(); ++i)
	{
		UI_ActorInfo* pInfo = new UI_ActorInfo(m_pTurnMgr, monsters[i], false);
		pInfo->SetRenderSystem(m_pRenderSystem);
		pInfo->Init();
		pInfo->SetOffset({ startX + (static_cast<int>(i) * gap), 4 });
		m_vecMonsterInfos.emplace_back(pInfo);
	}

	// 플레이어 정보창
	for (size_t i = 0; i < players.size(); ++i)
	{
		UI_ActorInfo* pInfo = new UI_ActorInfo(m_pTurnMgr, players[i], true);
		pInfo->SetRenderSystem(m_pRenderSystem);
		pInfo->Init();
		pInfo->SetOffset({ startX + (static_cast<int>(i) * gap), 18 });
		m_vecPlayerInfos.emplace_back(pInfo);
	}

	// 로그 창
	for (int i = 0; i < MAX_LOG_LINES; ++i)
	{
		UI_Dialogue* pLogUI = new UI_Dialogue(L"", i, Wannabe::Color::White);
		pLogUI->SetRenderSystem(m_pRenderSystem);
		pLogUI->SetOffset({ 3, 29 + i }); // 로그 영역 시작 위치
		m_vecLogs.emplace_back(pLogUI);
	}

	// 커맨드 메뉴
	m_pCommandMenu = new UI_MenuList();
	m_pCommandMenu->SetRenderSystem(m_pRenderSystem);
	m_pCommandMenu->SetAnchor(Wannabe::UI::UIAnchor::BottomRight);
	m_pCommandMenu->SetOffset({ -35, -11 });
	m_pCommandMenu->DefaultMenuItems();
}

void UI_BattleLayout::ShowBattleResult(int iGold, int iExp, char rank)
{
	if (m_pResultUI != nullptr) 
		return;

	// 객체 생성 및 초기화
	m_pResultUI = new UI_BattleResult(iGold, iExp, rank);
	m_pResultUI->SetRenderSystem(m_pRenderSystem);
}

void UI_BattleLayout::Init()
{
	SetAnchor(Wannabe::UI::UIAnchor::TopLeft);
	SetOffset({ 0, 0 });
}

void UI_BattleLayout::ClearChildren()
{
	SafeDelete(m_pTurnOrderUI);
	SafeDelete(m_pCommandMenu);
	SafeDelete(m_pResultUI);

	for (auto* info : m_vecMonsterInfos)
		SafeDelete(info);

	m_vecMonsterInfos.clear();

	for (auto* info : m_vecPlayerInfos)
		SafeDelete(info);

	m_vecPlayerInfos.clear();

	for (auto* log : m_vecLogs) 
		SafeDelete(log);

	m_vecLogs.clear();
}

void UI_BattleLayout::UpdateLogs()
{
	if (m_pLogSys == nullptr || m_vecLogs.empty())
		return;

	const auto& dqLogs = m_pLogSys->GetLog();

	// 로그 시스템의 데이터를 UI_Dialogue 리스트에 순서대로 매칭
	// 로그 시스템의 m_maxLog와 레이아웃의 MAX_LOG_LINES 크기에 맞춰 출력
	for (int i = 0; i < MAX_LOG_LINES; ++i)
	{
		if (i < (int)dqLogs.size())
			m_vecLogs[i]->ChangeTxt(dqLogs[i].c_str());
		else
			m_vecLogs[i]->ChangeTxt(L""); // 빈 로그 처리
	}
}
