#include <string>
#include <algorithm>

#include "BattleLevel.h"
#include "Game/Game.h"
#include "Engine/Engine.h"
#include "Util/Utill.h"
#include "Core/Input.h"
#include "Render/RenderSystem.h"
#include "Manager/DataManager.h"

#include "Actor/Actor.h"
#include "Actor/Player.h"
#include "Actor/BattleActor.h"

#include "Item/Item.h"
#include "Item/ItemInstance.h"

#include "Enum/CombatType.h"

#include "Battle/BattleContext.h"
#include "Battle/TurnManager.h"

#include "Battle/Command/AtkCommand.h"
#include "Battle/Command/SkillCommand.h"
#include "Battle/Command/ItemCommand.h"
#include "Battle/Command/RunCommand.h"

#include "Battle/Cutscene/BattlePhaseChangeEvent.h"
#include "Battle/Cutscene/TurnStartEvent.h"
#include "Battle/Cutscene/TurnEndEvent.h"

#include "Battle/AI/EnemyAI.h"

#include "UI/UI_BattleLayout.h"
#include "UI/UI_Dialogue.h"
#include "UI/UI_HPBar.h"
#include "UI/UI_MenuList.h"
#include "UI/UI_TargetCursor.h"
#include "UI/UI_TurnOrder.h"
#include "UI/UI_Inventory.h"
#include "UI/UI_BattleResult.h"

#include "Component/StatComponent.h"
#include "Component/DisplayComponent.h"
#include "Component/StatusComponent.h"
#include "Component/InventoryComponent.h"
#include "Component/SkillComponent.h"

#include "Data/ActionData.h"

using namespace Wannabe;

BattleLevel::BattleLevel()
    :m_eBattleState(BattleState::Start), m_EventFactory(nullptr),
    m_BattleContext(BattleContext(&m_logSystem,&m_cutScenePlayer,
        [this](Wannabe::Actor* actor){RemoveActor(actor); },
        this, nullptr,nullptr))
{
    Init();
}

BattleLevel::~BattleLevel()
{
    //커맨드 정리
    while (!m_queBattleCmd.empty())
        m_queBattleCmd.pop();

    // 연출 정리
    m_cutScenePlayer.ClearCutscenePlayer();

    //턴 매니저 정리
    SafeDelete(m_pTurnManager);
    SafeDelete(m_pCamera);

    m_pTurnManager = nullptr;

    m_BattleContext.GetEventProcessor().SetRemoveCallback(nullptr);
    m_BattleContext.GetEventProcessor().SetEventFactory(nullptr);

    m_pMenu = nullptr;
    m_pTargetCursor = nullptr;
    m_pTurnOrder = nullptr;

    //actor 제거
    m_vecPlayerParty.clear();
    m_vecEnemyParty.clear();
    m_vecTargets.clear();
    m_vecPendingDestroy.clear();

    m_pMenu = nullptr;
    m_pTargetCursor = nullptr;
    m_pTurnOrder = nullptr;
    m_pInvenMenu = nullptr;
    m_pBattleResult = nullptr;
}

void BattleLevel::SetupBattle(std::vector<Wannabe::Actor*> vecPlayer, std::vector<Wannabe::Actor*> vecEnemy)
{
    RenderSystem& renderSys = Engine::Get().GetRenderSystem();
    renderSys.GetCamera().SetFollowMode(false);
    renderSys.GetCamera().SetPosition({ 0,0 });
    
    ConfigureRenderer(renderSys);

    for (int i = 0; i < vecPlayer.size(); ++i)
    {
        BattleActor* clone = new BattleActor();
        clone->CloneStat(vecPlayer[i], i, Team::Player);
        AddNewActor(clone);
        m_vecPlayerParty.emplace_back(clone);
    }

    for (int i = 0; i < vecEnemy.size(); ++i)
    {
        BattleActor* clone = new BattleActor();
        clone->CloneStat(vecEnemy[i], i, Team::Enemy);
        AddNewActor(clone);
        m_vecEnemyParty.emplace_back(clone);
    }

    m_logSystem.AddLog(L"적과의 조우!");
    m_logSystem.AddLog(std::to_wstring(m_vecEnemyParty.size()) + L"명의 적 발견!");

    m_pBattleLayout->SetupBattleUI(m_pTurnManager, &m_logSystem, m_vecEnemyParty, m_vecPlayerParty);
    m_pMenu = m_pBattleLayout->GetCommandMenu();
    m_pTurnOrder = m_pBattleLayout->GetTurnOrder();

    RequestBattleStateChange(BattleState::Init);
}

void BattleLevel::FinishBattle()
{
    //커맨드 정리
    while (!m_queBattleCmd.empty())
    {
        m_queBattleCmd.pop();
    }

    // 연출 정리
    m_cutScenePlayer.ClearCutscenePlayer();

    //턴 매니저 정리
    SafeDelete(m_pTurnManager);
    m_pTurnManager = nullptr;

    m_pMenu = nullptr;
    m_pTargetCursor = nullptr;
    m_pTurnOrder = nullptr;

    //actor 제거
    for (auto* actor : m_vecPlayerParty)
    {
        if (actor)
            m_vecPendingDestroy.emplace_back(actor);
    }

    for (auto* actor : m_vecEnemyParty)
    {
        if (actor)
            m_vecPendingDestroy.emplace_back(actor);
    }

    m_vecPlayerParty.clear();
    m_vecEnemyParty.clear();

    for (auto* actor : m_vecPendingDestroy)
    {
        if(actor)
            actor->Destroy();
    }

    m_vecPendingDestroy.clear();
}

void BattleLevel::SetEventFactory(IBattleEventFactory* factory)
{
    m_EventFactory = factory;

    m_BattleContext.SetEventFactory(factory);
}

void BattleLevel::RemoveActorCallback(Actor* actor)
{
    if (actor == nullptr)
        return;

    BattleLevel* level = Game::Get().GetBattleLevel();

    if (level)
        level->RemoveActor(actor);
}

void BattleLevel::AddActor(std::unique_ptr<Wannabe::Actor> actor)
{
    m_vecActors.emplace_back(std::move(actor));
}

void BattleLevel::OnRemoveActor(Actor* actor)
{
    RemoveActor(actor);
}

void BattleLevel::PushCommand(std::unique_ptr<Wannabe::IBattleCommand> cmd)
{
    m_queBattleCmd.push(std::move(cmd));
}

void BattleLevel::BeginPlay()
{
    super::BeginPlay();
    ConfigureRenderer(Engine::Get().GetRenderSystem());
}

void BattleLevel::Tick(float fDeltaTime)
{
    super::Tick(fDeltaTime);

    CleanupDeacActor();

    switch (m_eBattleState)
    {
    case BattleState::Init:
        Phase_Init();
        break;

    case BattleState::Start:
        Phase_Start();
        break;

    case BattleState::TurnCheck:
        Phase_TurnCheck();
        break;

    case BattleState::CommandSelect:
        Phase_CommandSelect();
        break;
    case BattleState::InventorySelect:
        Phase_InvenSelect();
        break;
    case BattleState::SkillSelect:
        Phase_SkillSelect();
        break;
    case BattleState::TargetSelect:
        Phase_TargetSelect();
        break;

    case BattleState::EnemyAI:
        Phase_EnemyAI();
        break;

    case BattleState::Animation:
        Phase_Animation(fDeltaTime);
        break;

    case BattleState::Log:
        Phase_Log();
        break;

    case BattleState::Result:
        Phase_Result();
        break;

    default:
        break;
    }

    for (auto& actor : m_vecActors)
        actor->Tick(fDeltaTime);

    ProcessStateRequest();
}

void BattleLevel::Draw(Wannabe::RenderSystem& renderSys)
{
    super::Draw(renderSys);

    for (auto& actor : m_vecActors)
        actor->Draw(renderSys);
}

void BattleLevel::ConfigureRenderer(Wannabe::RenderSystem& renderer) const
{
    renderer.SetRenderMode(Wannabe::RenderSystem::RenderMode::Battle);
    renderer.GetCamera().SetFollowMode(false);
}

void BattleLevel::Init()
{
    Wannabe::RenderSystem& renderSys = Engine::Get().GetRenderSystem();
    // 카메라
    m_pCamera = new Camera(0,0);

    // 턴 매니저
    m_pTurnManager = new TurnManager();

    // 통합 UI 배경, 틀 역할을 함
    m_pBattleLayout = new UI_BattleLayout();
    m_pBattleLayout->SetRenderSystem(&renderSys);

    AddNewActor(m_pBattleLayout);

    // 타겟 커서
    m_pTargetCursor = new UI_TargetCursor(this);
    m_pTargetCursor->SetRenderSystem(&renderSys);
    m_pTargetCursor->Init();
    m_pTargetCursor->SetActive(false);

    AddNewActor(m_pTargetCursor);

    // Inventory UI 모달 윈도우
    m_pInvenMenu = new UI_Inventory(nullptr);
    m_pInvenMenu->SetRenderSystem(&renderSys);
    m_pInvenMenu->Init();
    m_pInvenMenu->SetActive(false);

    AddNewActor(m_pInvenMenu);

    m_BattleContext.GetEventProcessor().SetRemoveCallback(this);
}

void BattleLevel::Phase_Init()
{
    if (Input::Get().GetKeyDown(VK_RETURN))
    {
        //start Phase, enter시 로그 지우고 다음 phase로
        while (!m_logSystem.IsEmptyLog())
            m_logSystem.PopFrontLog();

        RequestBattleStateChange(BattleState::Start);
    }
}

void BattleLevel::Phase_Start()
{
    m_CurActor = m_pTurnManager->GetNextActor(m_vecPlayerParty, m_vecEnemyParty);
    if (m_CurActor == nullptr)
    {
        m_pTurnManager->ProgressTurns();
        // 다시 Start로 와서 다음 액터가 있는지 재확인
        RequestBattleStateChange(BattleState::Start);
    }
    else
    {
        // 행동할 액터가 있다면 턴 체크 단계로 진입
        RequestBattleStateChange(BattleState::TurnCheck);
    }
}

void BattleLevel::Phase_CommandSelect()
{
    if (m_pMenu == nullptr)
        return;

    if (m_eMenuTxt != MenuTxt::Command)
    {
        m_eMenuTxt = MenuTxt::Command;
        m_pMenu->SetActive(true);
        m_pMenu->DefaultMenuItems();
        m_pMenu->SetOffset(Vector2(-25, -8)); //우측 하단
    }

    Input_MenuSelect();

    if (m_queBattleCmd.empty() == false)
        RequestBattleStateChange(BattleState::Animation);
}

void BattleLevel::Input_MenuSelect()
{
    if (m_pMenu == nullptr)
        return;

    if (Input::Get().GetKeyDown(VK_UP))
        m_pMenu->MoveCursor(-1);

    if (Input::Get().GetKeyDown(VK_DOWN))
        m_pMenu->MoveCursor(1);

    if (Input::Get().GetKeyDown(VK_ESCAPE))
    {
        RequestBattleStateChange(BattleState::CommandSelect);
        return;
    }

    if (Input::Get().GetKeyDown(VK_RETURN))
    {
        int idx = m_pMenu->GetCursorIdx();
        switch (m_eMenuTxt)
        {
        case MenuTxt::Command:

            if (idx == 0)
            {
                m_iActionTID = 1;
                m_selectedCmd = CommandType::Atk;
                EnterTargetSelect(m_iActionTID);
            }
            else if (idx == 1)
            {
                m_selectedCmd = CommandType::Skill;
                RequestBattleStateChange(BattleState::SkillSelect);
            }
            else if (idx == 2)
            {
                m_selectedCmd = CommandType::Item;

                BuildItemMenu();
                m_pMenu->SetItems(m_vecItemMenu);
                m_eMenuTxt = MenuTxt::Item;
                RequestBattleStateChange(BattleState::InventorySelect);
            }
            else if (idx == 3)
            {
                // Run
            }
            break;

        case MenuTxt::Skill:
        case MenuTxt::Item:

            m_iActionTID = idx; // 중요
            m_pMenu->SetActive(false);

            EnterTargetSelect(m_iActionTID);
            break;
        }
    }
}

void BattleLevel::Phase_TargetSelect()
{
    // 타겟 커서 활성화
    m_pTargetCursor->SetActive(true);
    // 적 리스트 기준 위치
    m_pTargetCursor->SetOffset(Vector2(10, 2));

    Input_TargetSelect();
}

void BattleLevel::Input_TargetSelect()
{
    if (m_CurActor == nullptr)
        return;

    if (m_vecTargets.empty())
        return;

    const int targetCount = static_cast<int>(m_vecEnemyParty.size());
    if (targetCount == 0)
        return;

    if (Input::Get().GetKeyDown(VK_LEFT))
        m_pTargetCursor->SetCursorIdx(-1);

    if (Input::Get().GetKeyDown(VK_RIGHT))
        m_pTargetCursor->SetCursorIdx(+1);

    //타켓 설정 취소
    if (Input::Get().GetKeyDown(VK_ESCAPE))
    {
        //목표 제거, 타겟 지정 취소
        m_pTargetCursor->SetActive(false);
        m_pTargetCursor->SetTargets({}); // 빈 벡터를 넘겨 내부 리스트 초기화
        m_vecTargets.clear();

        RequestBattleStateChange(BattleState::CommandSelect);
        m_pMenu->SetActive(true);
        m_pTargetCursor->SetActive(false);
        return;
    }

    if (Input::Get().GetKeyDown(VK_RETURN))
    {
        int idx = m_pTargetCursor->GetCursorIdx();
        if (idx >= m_vecTargets.size())
            return;

        Actor* target = m_vecTargets[idx]; //대상 없음
        if (target == nullptr)
            return;

        std::unique_ptr<IBattleCommand> cmd;
        switch (m_selectedCmd)
        {
        case CommandType::Atk:
            cmd = std::make_unique<AtkCommand>(m_CurActor, target);
            break;

        case CommandType::Skill:
            cmd = std::make_unique<SkillCommand>(m_CurActor, target, m_iActionTID);
            break;

        case CommandType::Item:
            cmd = std::make_unique<ItemCommand>(m_CurActor, target, m_iActionTID);
            break;
        }

        if (cmd)
            m_queBattleCmd.push(std::move(cmd));

        //초기화
        m_pTargetCursor->SetActive(false);
        m_pTargetCursor->SetTargets({}); // 빈 벡터를 넘겨 내부 리스트 초기화
        m_vecTargets.clear();
        m_iActionTID = 0;

        m_pMenu->SetActive(false);
        m_pTargetCursor->SetActive(false);
        RequestBattleStateChange(BattleState::Animation);
    }
}

void BattleLevel::Phase_InvenSelect()
{
    if (m_pMenu == nullptr)
        return;

    if (m_eMenuTxt != MenuTxt::Item)
    {
        BuildItemMenu();
        m_pMenu->SetItems(m_vecItemMenu);
        m_eMenuTxt = MenuTxt::Item;

        m_pInvenMenu->SetActive(true);
        m_pInvenMenu->SetOffset(Vector2(-25, -8));
    }

    Input_InvenSelect();
}

void BattleLevel::Input_InvenSelect()
{
    if (m_CurActor == nullptr)
        return;

    if (m_CurActor->GetTeam() != Team::Player)
        return;

    auto inven = m_CurActor->GetComponent<InventoryComponent>();
    if (inven == nullptr)
        return;

    const auto& items = inven->GetItems();
    if (items.empty())
        return;

    if (Input::Get().GetKeyDown(VK_UP))
        m_pMenu->MoveCursor(-1);

    if (Input::Get().GetKeyDown(VK_DOWN))
        m_pMenu->MoveCursor(1);

    // Cancle
    if (Input::Get().GetKeyDown(VK_ESCAPE))
    {
        RequestBattleStateChange(BattleState::CommandSelect);

        m_pInvenMenu->SetActive(false);
        m_pMenu->SetActive(true);
        return;
    }

    // Confirm
    if (Input::Get().GetKeyDown(VK_RETURN))
    {
        int idx = m_pMenu->GetCursorIdx();
        if (idx >= items.size())
            return;

        Wannabe::Item* item = items[idx]->GetItem();
        if (item == nullptr)
            return;

        m_iActionTID = item->GetItemTID(); //선택된 item
        m_selectedCmd = CommandType::Item;

        m_pInvenMenu->SetActive(false);

        EnterTargetSelect(m_iActionTID);
    }
}

void BattleLevel::Phase_SkillSelect()
{
    if (m_pMenu == nullptr)
        return;

    if (m_eMenuTxt != MenuTxt::Skill)
    {
        m_eMenuTxt = MenuTxt::Skill;
        m_pMenu->SetActive(true);

        BuildSkillMenu();
        m_pMenu->SetItems(m_vecSkillMenu);
        m_pMenu->SetOffset(Vector2(-25, -8));
    }

    Input_SkillSelect();
}

void BattleLevel::Input_SkillSelect()
{
    if (!m_CurActor || m_CurActor->GetTeam() != Team::Player)
        return;

    auto skillComp = m_CurActor->GetComponent<SkillComponent>();
    if (skillComp == nullptr)
        return;

    const auto& skills = skillComp->GetSkillList();
    if (skills.empty())
        return;

    if (Input::Get().GetKeyDown(VK_UP))
        m_pMenu->MoveCursor(-1);

    if (Input::Get().GetKeyDown(VK_DOWN))
        m_pMenu->MoveCursor(1);

    if (Input::Get().GetKeyDown(VK_ESCAPE))
    {
        RequestBattleStateChange(BattleState::CommandSelect);
        m_pMenu->SetActive(false);
        return;
    }

    if (Input::Get().GetKeyDown(VK_RETURN))
    {
        int idx = m_pMenu->GetCursorIdx();
        if (idx >= skills.size())
            return;

        m_iActionTID = skills[idx];
        m_pMenu->SetActive(false);

        EnterTargetSelect(m_iActionTID);
    }
}

void BattleLevel::Phase_TurnCheck()
{
    //dead 액터 제거
    CleanupDeacActor();
    
    // 전투 종료 체크
    if (IsFinishBattle())
    {
        RequestBattleStateChange(BattleState::Result);
        return;
    }

    // 강조 해제
    if (auto* bActor = dynamic_cast<BattleActor*>(m_CurActor))
        bActor->SetTargeted(false);

    // Actor 체크
    m_CurActor = m_pTurnManager->GetNextActor(m_vecPlayerParty,m_vecEnemyParty);
    if (m_CurActor == nullptr)
    {
        RequestBattleStateChange(BattleState::Start);
        return;
    }

    m_eMenuTxt = MenuTxt::None;
    m_iActionTID = 0;
    m_vecTargets.clear();

    // 턴 체크
    m_cutScenePlayer.Push(std::make_unique<TurnStartEvent>(m_CurActor));

    if (auto* bActor = dynamic_cast<BattleActor*>(m_CurActor))
        bActor->SetTargeted(true);

    RequestBattleStateChange(BattleState::Animation);
}

void BattleLevel::Phase_EnemyAI()
{
    if (m_CurActor == nullptr) 
        return;

    std::vector<Actor*> targets;
    for (auto* p : m_vecPlayerParty)
    {
        if (p == nullptr || p->IsDestroyRequested())
            continue;

        if (p->GetComponent<StatComponent>() == nullptr || p->GetComponent<StatComponent>()->IsDead())
            continue;

        targets.push_back(p);
    }

    if (targets.empty()) 
        return;

    int randIdx = Util::Random(0, (int)targets.size() - 1);
    Actor* target = targets[randIdx];

    auto cmd = std::make_unique<AtkCommand>(m_CurActor, target);
    m_queBattleCmd.push(std::move(cmd));

    RequestBattleStateChange(BattleState::Animation);
}

void BattleLevel::Phase_Animation(float fDeltaTime)
{
    m_cutScenePlayer.Update(m_BattleContext, fDeltaTime);

    if (m_pMenu) 
        m_pMenu->SetActive(false);

    if (m_cutScenePlayer.IsPlaying())
        return;

    if (!m_queBattleCmd.empty())
    {
        std::unique_ptr<IBattleCommand> cmd = std::move(m_queBattleCmd.front());
        m_queBattleCmd.pop();
        cmd->Execute(m_BattleContext);
        return;
    }

    if (m_CurActor != nullptr)
    {
        m_pTurnManager->TurnEnd();
        m_CurActor = nullptr;
        m_BattleContext.GetEventProcessor().FlushRemoval();
        RequestBattleStateChange(BattleState::TurnCheck);
    }
}

void BattleLevel::Phase_Log()
{
    if (Input::Get().GetKeyDown(VK_RETURN))
    {
        if (!m_logSystem.IsEmptyLog())
            m_logSystem.PopFrontLog();

        // 모든 로그를 확인했다면 다음 단계로
        if (m_logSystem.IsEmptyLog())
        {
            RequestBattleStateChange(IsFinishBattle() ? BattleState::Result : BattleState::TurnCheck);
        }
    }
}

void BattleLevel::Phase_Result()
{
    if (m_pBattleResult == nullptr)
    {
        int totalGold = 0;
        int totalExp = 0;
        char rank = 'F';

        if (IsPlayerWin())
        {
            // 승리 보상 계산 (예: 몬스터들로부터 골드/경험치 합산)
            for (auto* enemy : m_vecEnemyParty)
            {
                if (auto stat = enemy->GetComponent<StatComponent>())
                    totalExp += stat->GetStatData().iMaxExp;
            }

            Game::Get().ProcessBattleReward(totalGold, totalExp);
        }
        else
        {
            // 패배 시 로그 출력 혹은 게임 오버 처리
            m_logSystem.AddLog(L"파티가 전멸했습니다...");
            rank = 'F';
        }

        m_pBattleLayout->ShowBattleResult(totalGold, totalExp, rank);
        m_pBattleResult = m_pBattleLayout->GetResultUI();
        return;
    }

    // 연출이 Finished 상태일 때만 Enter 입력을 받음
    UI_BattleResult* pResultUI = m_pBattleLayout->GetResultUI();
    if (pResultUI->GetDisplayState() == UI_BattleResult::DisplayState::Finished)
    {
        if (Input::Get().GetKeyDown(VK_RETURN))
        {
            pResultUI->SetExit(true);
        }
    }


    // 종료 플래그가 세워졌다면 씬 전환
    if (m_pBattleResult->Exit() == true)
    {
        Game::Get().BattleEnd();
    }
}

void BattleLevel::ProcessStateRequest()
{
    if (m_bStateChangeRequested == false)
        return;

    m_eBattleState = m_RequestedState;

    m_bStateChangeRequested = false;
}

void BattleLevel::EnterTargetSelect(int iTID)
{
    const ActionData*  data = DataManager::Get().GetActionData(iTID);
    if (data == nullptr)
        return;

    std::vector<Actor*> targets;
    switch (data->targetType)
    {
    case ActionTargetType::Self:
        targets.emplace_back(m_CurActor);
        break;

    case ActionTargetType::SingleEnemy:
    case ActionTargetType::AllEnemy:
        targets = m_BattleContext.GetEnemyParty(m_CurActor);
        break;

    case ActionTargetType::SingleAlly:
    case ActionTargetType::AllAlly:
        targets = m_BattleContext.GetPlayerParty(m_CurActor);
        break;
    }

    std::vector<Actor*> finalTargets; //사망 제외
    for (Actor* actor : targets)
    {
        if (actor == nullptr || actor->IsDestroyRequested())
            continue;

        if (actor->GetComponent<StatComponent>() == nullptr || actor->GetComponent<StatComponent>()->IsDead())
            continue;

        finalTargets.emplace_back(actor);
    }

    if (finalTargets.empty()) // 선택 가능 대상 없음
    {
        RequestBattleStateChange(BattleState::CommandSelect);
        m_pMenu->SetActive(true);
        return;
    }

    m_vecTargets = finalTargets;

    bool bIsAll = (data->targetType == ActionTargetType::AllEnemy ||
        data->targetType == ActionTargetType::AllAlly);
    m_pTargetCursor->SetAllTargetMode(bIsAll);
    m_pTargetCursor->SetTargets(finalTargets);
    m_pTargetCursor->SetActive(true);

    RequestBattleStateChange(BattleState::TargetSelect);
}

void BattleLevel::BuildSkillMenu()
{
    std::vector<std::wstring> menu;

    auto skillComp = m_CurActor->GetComponent<SkillComponent>();
    if (!skillComp) return;

    for (auto& TID : skillComp->GetSkillList())
    {
        const ActionData* data = DataManager::Get().GetActionData(TID);
        if (data == nullptr)
            continue;

        menu.emplace_back(data->wstrName);
    }

    m_vecSkillMenu = menu;
}

void BattleLevel::BuildItemMenu()
{
    std::vector<std::wstring> menu;

    auto inven = m_CurActor->GetComponent<InventoryComponent>();
    if (inven == nullptr)
        return;

    for (auto& itemInst : inven->GetItems())
    {
        auto item = itemInst->GetItem();
        if (item == nullptr)
            continue;

        const ActionData* data = data = DataManager::Get().GetActionData(item->GetItemTID());
        if (data == nullptr)
            continue;

        std::wstring displayName = data->wstrName;
        displayName += L" x" + std::to_wstring(itemInst->GetCnt());

        menu.emplace_back(displayName);
    }

    if (m_pMenu != nullptr)
    {
        m_pMenu->SetBoxSize(24, 4);
        m_pMenu->SetItems(menu);
        m_pMenu->SetActive(true);
    }

    m_vecItemMenu = menu;
}

void BattleLevel::RequestBattleStateChange(BattleState state)
{
    m_RequestedState = state;
    m_bStateChangeRequested = true;
}

bool BattleLevel::IsFinishBattle()
{
    return IsPlayerWin() || IsEnemyWin();
}

bool BattleLevel::IsPlayerWin() const
{
    for (auto* enemy : m_vecEnemyParty)
    {
        if (enemy == nullptr)
            continue;

        if (enemy->IsDestroyRequested() == true)
            continue;

        if (enemy->GetComponent<StatComponent>() == nullptr)
            continue;

        if (enemy->GetComponent<StatComponent>()->IsDead() == false)
            return false;
    }

    return true;
}

bool BattleLevel::IsEnemyWin() const
{
    for (auto* player : m_vecPlayerParty)
    {
        if (player == nullptr)
            continue;

        if (player->IsDestroyRequested() == true)
            continue;

        if (player->GetComponent<StatComponent>() == nullptr)
            continue;

        if (player->GetComponent<StatComponent>()->IsDead() == false)
            return false;
    }

    return true;
}

void BattleLevel::ClearDialogue()
{
    // 내부 로그 시스템의 큐도 함께 비워주어 다음 페이즈에 영향이 없도록 합니다.
    while (!m_logSystem.IsEmptyLog())
    {
        m_logSystem.PopFrontLog();
    }
}

void BattleLevel::RemoveActor(Wannabe::Actor* actor)
{
    if (actor == nullptr)
        return;

    if (actor->IsDestroyRequested() == true)
        return;

    actor->Destroy();
}

void BattleLevel::CleanupDeacActor()
{
    m_vecActors.erase(
        std::remove_if(m_vecActors.begin(), m_vecActors.end(),
            [](const std::unique_ptr<Actor>& ptr)
    {
        return ptr == nullptr || ptr->IsDestroyRequested();
    }),
        m_vecActors.end()
    );
}