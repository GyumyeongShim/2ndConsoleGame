#pragma once
#include <queue>
#include <vector>
#include <memory>
#include <string>

#include "Level/Level.h"
#include "Engine/Engine.h"
#include "Actor/Actor.h"

#include "Interface/IBattleCommand.h"
#include "Interface/IBattleLevel.h"
#include "Interface/IBattleEventFactory.h"
#include "Interface/IRemoveActorCallback.h"

#include "Enum/CombatType.h"

#include "Battle/BattleContext.h"
#include "Battle/BattleLogSystem.h"
#include "Battle/System/CutScenePlayer.h"

class UI_Dialogue;
class UI_MenuList;
class UI_TargetCursor;
class UI_TurnOrder;
class UI_Inventory;
class UI_BattleResult;
class BattleActor;
class IBattleLevel;
class UI_BattleLayout;

namespace Wannabe
{
	class TurnManager;
	class Actor;
}

class BattleLevel : public Level, public IBattleLevel, public IRemoveActorCallback
{
	RTTI_DECLARATIONS(BattleLevel, Level)

	enum class MenuTxt
	{
		None,
		Command,
		Skill,
		Item
	};

public:
	BattleLevel();
	~BattleLevel();

	void SetupBattle(std::vector<Wannabe::Actor*> vecPlayer, std::vector<Wannabe::Actor*> vecEnemy);
	void FinishBattle();

	void SetEventFactory(IBattleEventFactory* factory);
	static void RemoveActorCallback(Wannabe::Actor* actor);

	virtual BattleState GetBattleState() override { return m_eBattleState; }
	virtual void RequestBattleStateChange(BattleState state) override;
	virtual void AddActor(std::unique_ptr<Wannabe::Actor>actor) override;
	virtual void OnRemoveActor(Actor* actor) override;
	
	virtual const std::vector<Actor*>& GetEnemyParty() const override { return m_vecEnemyParty; }
	virtual const std::vector<Actor*>& GetPlayerParty() const override { return m_vecPlayerParty; }

	virtual void PushCommand(std::unique_ptr<Wannabe::IBattleCommand> cmd) override;

	void SetActionDone(bool bDone) { m_bIsActionDone = bDone; }
	bool IsActionDone() const { return m_bIsActionDone; }

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float fDeltaTime)override;
	virtual void Draw(Wannabe::RenderSystem& renderSys) override;
	virtual void ConfigureRenderer(Wannabe::RenderSystem& renderer) const override;

private:
	void Init();

	void Phase_Init();
	void Phase_Start();
	void Phase_CommandSelect();
	void Input_MenuSelect();
	void Phase_TargetSelect();
	void Input_TargetSelect();
	void Phase_InvenSelect();
	void Input_InvenSelect();
	void Phase_SkillSelect();
	void Input_SkillSelect();
	void Phase_TurnCheck();
	void Phase_EnemyAI();
	void Phase_Animation(float fDeltaTime);
	void Phase_Log();
	void Phase_Result();

	void ProcessStateRequest();

	void EnterTargetSelect(int iTID);
	void BuildSkillMenu();
	void BuildItemMenu();

	bool IsFinishBattle();
	bool IsPlayerWin() const;
	bool IsEnemyWin() const;
	void ClearDialogue();

	void RemoveActor(Wannabe::Actor* actor);
	void CleanupDeacActor();

private:
	bool m_bStateChangeRequested = false; //state machine
	BattleState m_RequestedState = BattleState::Start;
	BattleState m_eBattleState = BattleState::Start;

	Wannabe::Actor* m_CurActor = nullptr; //현재 순서인 액터

	std::vector<Actor*> m_vecPlayerParty;
	std::vector<Actor*> m_vecEnemyParty;
	std::vector<Actor*> m_vecPendingDestroy;

	std::vector<Actor*> m_vecTargets; //선택된 대상

	int m_iActionTID = 0; // 스킬, 아이템 TID
	CommandType m_selectedCmd = CommandType::Atk; //command
	std::queue<std::unique_ptr<Wannabe::IBattleCommand>> m_queBattleCmd;

	TurnManager* m_pTurnManager = nullptr; //턴 매니저
	Wannabe::BattleContext m_BattleContext; // 전투 관련 기능들
	BattleLogSystem m_logSystem; // 전투 로그
	CutscenePlayer m_cutScenePlayer; // 전투 연출 재생기
	IBattleEventFactory* m_EventFactory = nullptr; // 전투연출 (이벤트)

	std::vector<std::unique_ptr<Wannabe::Actor>> m_vecActors; //전투 중 생성된 actor, 특별 처리

	UI_BattleLayout* m_pBattleLayout = nullptr; // 통합 UI
	UI_TargetCursor* m_pTargetCursor = nullptr;
	UI_Inventory* m_pInvenMenu = nullptr;
	UI_BattleResult* m_pBattleResult = nullptr;
	UI_MenuList* m_pMenu = nullptr;
	UI_TurnOrder* m_pTurnOrder = nullptr;

	MenuTxt m_eMenuTxt = MenuTxt::None;
	std::vector<std::wstring> m_vecSkillMenu;
	std::vector<std::wstring> m_vecItemMenu;

	bool m_bIsActionDone = false;
};