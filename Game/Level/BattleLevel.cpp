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

#include "UI/UI_Dialogue.h"
#include "UI/UI_HPBar.h"
#include "UI/UI_MenuList.h"
#include "UI/UI_TargetCursor.h"
#include "UI/UI_TurnOrder.h"
#include "UI/UI_Inventory.h"

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
    {
        m_queBattleCmd.pop();
    }

    // 연출 정리
    m_cutScenePlayer.ClearCutscenePlayer();

    //턴 매니저 정리
    SafeDelete(m_pTurnManager);
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

    m_vecPlayerHPUI.clear();
    m_vecEnemyHPUI.clear();
    m_vecDialogue.clear();

    m_pMenu = nullptr;
    m_pTargetCursor = nullptr;
    m_pTurnOrder = nullptr;
    m_pInvenMenu = nullptr;
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

        //HP bar
        UI_HPBar* hpUI = new UI_HPBar(clone,i,Team::Player);
        hpUI->SetRenderSystem(&renderSys);
        hpUI->Init();
        AddNewActor(hpUI);
        m_vecPlayerHPUI.emplace_back(hpUI);
    }

    int cnt = 0;
    int maxEnemies = 4; //test 최대 4마리 todo test
    int monsterCnt = Util::Random(1, maxEnemies); //몬스터 개수
    while (cnt < monsterCnt)
    {
        // 랜덤하게 적 선택
        int idx = Util::Random(0, static_cast<int>(vecEnemy.size()) - 1);

        BattleActor* clone = new BattleActor();
        clone->CloneStat(vecEnemy[idx], cnt, Team::Enemy);
        AddNewActor(clone);
        m_vecEnemyParty.emplace_back(clone);

        //HP bar
        UI_HPBar* hpUI = new UI_HPBar(clone,cnt,Team::Enemy);
        hpUI->SetRenderSystem(&renderSys);
        hpUI->Init();
        AddNewActor(hpUI);
        m_vecEnemyHPUI.emplace_back(hpUI);

        ++cnt;
    }

    m_logSystem.AddLog(L"적과의 조우!");
    m_logSystem.AddLog(std::to_wstring(m_vecEnemyParty.size()) + L"명의 적 발견!");
    RequestBattleStateChange(BattleState::Start);
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
            m_vecPendingDestroy.push_back(actor);
    }

    for (auto* actor : m_vecEnemyParty)
    {
        if (actor)
            m_vecPendingDestroy.push_back(actor);
    }

    m_vecPlayerParty.clear();
    m_vecPlayerHPUI.clear();

    m_vecEnemyParty.clear();
    m_vecEnemyHPUI.clear();

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
    case BattleState::Start:
        Phase_Start();
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

    case BattleState::TurnCheck:
        Phase_TurnCheck();
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

    // 메뉴
    m_pMenu = new UI_MenuList();
    m_pMenu->SetRenderSystem(&renderSys);
    m_pMenu->Init();
    m_pMenu->SetAnchor(UI::UIAnchor::BottomRight);
    m_pMenu->SetBoxSize(28, 6);       // 스킬명과 MP 표기를 위해 가로 길이를 넉넉히 잡음
    m_pMenu->SetOffset(Vector2(- 4, -2));       // 화면 오른쪽 끝(-4), 아래 끝(-2)에서 안쪽으로 띄움
    m_pMenu->SetPadding(2, 1);
    m_pMenu->SetActive(false);
    m_pMenu->DefaultMenuItems();

    AddNewActor(m_pMenu);


    // 타겟 커서
    m_pTargetCursor = new UI_TargetCursor(this);
    m_pTargetCursor->SetRenderSystem(&renderSys);
    m_pTargetCursor->Init();
    m_pTargetCursor->SetActive(false);

    AddNewActor(m_pTargetCursor);


    // 턴 UI
    m_pTurnOrder = new UI_TurnOrder(m_pTurnManager, Vector2(70, 2));
    m_pTurnOrder->SetRenderSystem(&renderSys);
    m_pTurnOrder->Init();
    AddNewActor(m_pTurnOrder);

    // Inventory UI
    m_pInvenMenu = new UI_Inventory(nullptr);
    m_pInvenMenu->SetRenderSystem(&renderSys);
    m_pInvenMenu->Init();
    m_pInvenMenu->SetActive(false);

    AddNewActor(m_pInvenMenu);

    // Log 출력
    for (int i = 0; i < 2; ++i)
    {
        UI_Dialogue* log = new UI_Dialogue(L"Text", i, Color::Green);
        log->SetRenderSystem(&renderSys);
        log->Init(i);
        AddNewActor(log);
        m_vecDialogue.emplace_back(log);
    }

    //actor 제거
    m_BattleContext.GetEventProcessor().SetRemoveCallback(this);
}

void BattleLevel::Phase_Start()
{
    const auto& logs = m_logSystem.GetLog(); // 로그 목록
    int iCnt = std::min(2, (int)logs.size()); //최대 2개까지만 보여준다.
    if (iCnt >= 1)
    {
        // 1. 최신 로그 (가장 아래줄)
        m_vecDialogue[1]->ChangeTxt(logs[iCnt - 1].c_str());

        // 2. 이전 로그 (윗줄로 밀려남)
        if (iCnt >= 2)
            m_vecDialogue[0]->ChangeTxt(logs[iCnt - 2].c_str());
        else
            m_vecDialogue[0]->ChangeTxt(L""); // 로그가 1개뿐이면 윗줄은 비움
    }

    if (Input::Get().GetKeyDown(VK_RETURN))
    {
        //start Phase, enter시 로그 지우고 다음 phase로
        for (auto* pUI : m_vecDialogue)
        {
            pUI->ChangeTxt(L"");
        }

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

    auto inven = m_CurActor->GetInven();
    if (inven == nullptr)
        return;

    const auto& items = inven->GetItems();
    if (items.empty())
        return;

    if (Input::Get().GetKeyDown(VK_UP))
        m_pInvenMenu->MoveCursor(-1);

    if (Input::Get().GetKeyDown(VK_DOWN))
        m_pInvenMenu->MoveCursor(1);

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
        int idx = m_pInvenMenu->GetCursorIdx();
        if (idx >= items.size())
            return;

        Wannabe::Item* item = items[idx].GetItem();
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

    auto skillComp = m_CurActor->GetSKill();
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

    // Actor 체크
    m_CurActor = m_pTurnManager->GetNextActor(m_vecPlayerParty,m_vecEnemyParty);
    if (m_CurActor == nullptr)
    {
        // 모든 액터가 턴을 진행했으면 턴 카운트 증가 후 다시 체크
        m_pTurnManager->ProgressTurns();
        return;
    }

    if (m_CurActor && m_CurActor->GetTeam() == Team::Player)
    {
        m_pInvenMenu->SetInventory(m_CurActor->GetInven());
    }

    m_eMenuTxt = MenuTxt::None;
    m_iActionTID = 0;
    m_vecTargets.clear();
    // 턴 체크
    m_cutScenePlayer.Push(std::make_unique<TurnStartEvent>(m_CurActor));
    RequestBattleStateChange(BattleState::Animation);
}

void BattleLevel::Phase_Animation(float fDeltaTime)
{
    if (m_pMenu)
        m_pMenu->SetActive(false);

    m_cutScenePlayer.Update(m_BattleContext, fDeltaTime);

    // 연출 종료 + 명령 있음
    if (m_cutScenePlayer.IsPlaying() == false && m_queBattleCmd.empty() == false)
    {
        std::unique_ptr<Wannabe::IBattleCommand> cmd = std::move(m_queBattleCmd.front());
        m_queBattleCmd.pop();

        cmd->Execute(m_BattleContext);
        return;
    }
    // 연출 종료
    if (m_cutScenePlayer.IsPlaying() == false && m_queBattleCmd.empty() == true)
    {
        if (m_pTurnManager->GetCurBattleActor() != nullptr)
        {
            m_pTurnManager->TurnEnd();
            m_cutScenePlayer.Push(std::make_unique<TurnEndEvent>(m_CurActor));
            m_CurActor = nullptr;
            return;
        }

        m_BattleContext.GetEventProcessor().FlushRemoval();
        m_eMenuTxt = MenuTxt::None;
        RequestBattleStateChange(BattleState::TurnCheck);
    }
}

void BattleLevel::Phase_Log()
{
    if (m_logSystem.IsEmptyLog() == false)
    {
        m_vecDialogue[0]->ChangeTxt(m_vecDialogue[1]->GetTxt().c_str());

        std::wstring nextLog = m_logSystem.PopFrontLog();
        m_vecDialogue[1]->ChangeTxt(nextLog.c_str());
    }

    // 5. 모든 로그가 소모되었고 Enter를 누르면 페이즈 전환
    if (Input::Get().GetKeyDown(VK_RETURN))
    {
        if (m_logSystem.IsEmptyLog() == true)
        {
            // UI 텍스트 초기화
            for (auto* pUI : m_vecDialogue)
            {
                pUI->ChangeTxt(L"");
            }

            CleanupDeacActor();
            RequestBattleStateChange(IsFinishBattle() ? BattleState::Result : BattleState::TurnCheck);
        }
        // 로그가 남아있다면 다음 프레임/입력에서 위 if문이 다시 실행되어 다음 줄이 출력됩니다.
    }
    const auto& logs = m_logSystem.GetLog(); // 로그 목록
    int iCnt = std::min(2, (int)logs.size()); //최대 2개까지만 보여준다.
    if (iCnt >= 1)
    {
        // 1. 최신 로그 (가장 아래줄)
        m_vecDialogue[1]->ChangeTxt(logs[iCnt - 1].c_str());

        // 2. 이전 로그 (윗줄로 밀려남)
        if (iCnt >= 2)
            m_vecDialogue[0]->ChangeTxt(logs[iCnt - 2].c_str());
        else
            m_vecDialogue[0]->ChangeTxt(L""); // 로그가 1개뿐이면 윗줄은 비움
    }

    if (Input::Get().GetKeyDown(VK_RETURN))
    {
        CleanupDeacActor();
        RequestBattleStateChange(IsFinishBattle() ? BattleState::Result : BattleState::TurnCheck);
    }
}

void BattleLevel::Phase_Result()
{
    Game::Get().BattleEnd();
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
        targets.push_back(m_CurActor);
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
        if (actor && actor->GetStat()->IsDead() == false)
            finalTargets.emplace_back(actor);
    }

    if (finalTargets.empty()) // 선택 가능 대상 없음
    {
        RequestBattleStateChange(BattleState::CommandSelect);
        m_pMenu->SetActive(true);
        return;
    }

    m_vecTargets = finalTargets;

    m_pTargetCursor->SetTargets(finalTargets);
    m_pTargetCursor->SetActive(true);

    RequestBattleStateChange(BattleState::TargetSelect);
}

void BattleLevel::BuildSkillMenu()
{
    std::vector<std::wstring> menu;

    auto skillComp = m_CurActor->GetSKill();
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

    auto inven = m_CurActor->GetInven();
    if (inven == nullptr)
        return;

    for (auto& itemInst : inven->GetItems())
    {
        auto item = itemInst.GetItem();
        if (item == nullptr)
            continue;

        const ActionData* data = data = DataManager::Get().GetActionData(item->GetItemTID());
        if (data == nullptr)
            continue;

        menu.emplace_back(data->wstrName);
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
    bool bFinished = false;
    if (m_vecPlayerParty.empty() == true)
        bFinished = true;

    if (m_vecEnemyParty.empty() == true)
        bFinished = true;

    return bFinished;
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
    auto Cleanup = [this](std::vector<Actor*>& party, std::vector<UI_HPBar*>& uiVec)
    {
        for (auto it = party.begin(); it != party.end(); )
        {
            Actor* actor = *it;

            if (actor == nullptr || actor->GetStat()->IsDead())
            {
                // HPBar 제거
                uiVec.erase(
                    std::remove_if(uiVec.begin(), uiVec.end(),
                        [actor](UI_HPBar* ui)
                {
                    return ui == nullptr ||
                        ui->GetBattleActorOwner() == actor;
                }),
                    uiVec.end()
                );

                m_vecPendingDestroy.push_back(actor);
                it = party.erase(it);
            }
            else
                ++it;
        }
    };

    Cleanup(m_vecPlayerParty, m_vecPlayerHPUI);
    Cleanup(m_vecEnemyParty, m_vecEnemyHPUI);

    m_vecActors.erase(
        std::remove_if(m_vecActors.begin(), m_vecActors.end(),
            [](const std::unique_ptr<Actor>& ptr)
    {
        return ptr == nullptr || ptr->IsDestroyRequested();
    }),
        m_vecActors.end()
    );
}