#pragma once
#include <queue>
#include <vector>
#include <memory>

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

class UI_HPBar;
class UI_Dialogue;
class UI_MenuList;
class UI_TargetCursor;
class UI_TurnOrder;
class UI_Inventory;
class BattleActor;
class IBattleLevel;

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
	void RequestBattleStateChange(BattleState state) override;
	void AddActor(std::unique_ptr<Wannabe::Actor>actor) override;
	const std::vector<Actor*>& GetEnemyParty() const override { return m_vecEnemyParty; }
	const std::vector<Actor*>& GetPlayerParty() const override { return m_vecPlayerParty; }
	virtual void OnRemoveActor(Actor* actor) override;
	void PushCommand(std::unique_ptr<Wannabe::IBattleCommand> cmd) override;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float fDeltaTime)override;
	virtual void Draw(Wannabe::RenderSystem& renderSys) override;
	virtual void ConfigureRenderer(Wannabe::RenderSystem& renderer) const override;

private:
	void Init();

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
	void Phase_Animation(float fDeltaTime);
	void Phase_Log();
	void Phase_Result();

	void ProcessStateRequest();

	void EnterTargetSelect(int iTID);
	void BuildSkillMenu();
	void BuildItemMenu();

	bool IsFinishBattle();
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

	std::vector<Actor*> m_vecTargets; //선택된 태상

	int m_iActionTID = 0; // 스킬, 아이템 TID
	int m_iTargetCursor = 0; // 타겟, input,select시 사용
	CommandType m_selectedCmd = CommandType::Atk; //command
	std::queue<std::unique_ptr<Wannabe::IBattleCommand>> m_queBattleCmd;

	TurnManager* m_pTurnManager = nullptr; //턴 매니저
	Wannabe::BattleContext m_BattleContext; // 전투 관련 기능들
	BattleLogSystem m_logSystem; // 전투 로그
	CutscenePlayer m_cutScenePlayer; // 전투 연출 재생기
	IBattleEventFactory* m_EventFactory = nullptr; // 전투연출 (이벤트)
	IRemoveActorCallback* m_RemoveCallback = nullptr; // 액터 삭제

	std::vector<std::unique_ptr<Wannabe::Actor>> m_vecActors; //전투 중 생성된 actor, 특별 처리

	std::vector<UI_HPBar*> m_vecPlayerHPUI; //UI
	std::vector<UI_HPBar*> m_vecEnemyHPUI;
	std::vector<UI_Dialogue*> m_vecDialogue; // 메시지 표기, 재활용을 위해 사용.
	UI_TargetCursor* m_pTargetCursor = nullptr;
	UI_TurnOrder* m_pTurnOrder = nullptr;
	UI_Inventory* m_pInvenMenu = nullptr;
	UI_MenuList* m_pMenu = nullptr;

	MenuTxt m_eMenuTxt = MenuTxt::None;
	std::vector<std::wstring> m_vecSkillMenu;
	std::vector<std::wstring> m_vecItemMenu;
};