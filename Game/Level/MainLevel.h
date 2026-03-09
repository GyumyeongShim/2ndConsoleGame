#pragma once
#include "Level/Level.h"
#include "Engine/Engine.h"
#include "Enum/WorldType.h"
#include "Math/Vector2.h"

class Player;
class TileMap;
struct RunGameData;
namespace Wannabe
{
	class TurnManager;
}

class MainLevel : public Level
{
	RTTI_DECLARATIONS(MainLevel, Level)
public:
	MainLevel();
	~MainLevel();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float fDeltaTime) override;
	virtual void Draw(Wannabe::RenderSystem& renderSys) override;
	virtual void ConfigureRenderer(Wannabe::RenderSystem& renderer) const override;
	virtual void OnEnterLevel(RunGameData* pData) override;
	virtual void OnExitLevel(RunGameData* pData) override;
	virtual bool CanMove(const Vector2& nextPos) override;
	virtual void CheckPortal() override;

	void Phase_Idle();
	void Phase_PlayerTurn(float fDeltaTime);
	void Phase_Move(float fDeltaTime);
	void Phase_EnemyTurn();
	void Phase_EnemyMove(float fDeltaTime);
	void Phase_Transition(float fDeltaTime);

private:
	void Init();
	void CheckRandomEncounter();
	void CheckMonsterEncounter();
	void StartBattleTransition(Actor* pTarget);
	void CalcMoveRange();
	void CalcMonsterMoveRange(Actor* pTarget);
	void OnCursorMoved();

private:
	std::unique_ptr<TileMap> m_worldMap;
	Player* m_pPlayer = nullptr;
	bool m_RequestBattleTransition = false;

	Actor* m_pCurActor = nullptr;

	TurnManager* m_pTurnManager = nullptr; //턴 매니저
	FieldState m_eFieldPhase = FieldState::Idle; // 기본 상태 설정
	Vector2 m_vCursorPos = { 1, 1 };
	std::vector<Vector2> m_vecPath;
	std::vector<Vector2> m_vecMoveRangeTiles;
	std::vector<Vector2> m_vecMonsterRangeTiles;
	Actor* m_pFocusedMonster = nullptr;
	bool m_bIsRangeCalculated = false;

	// 연출용 변수
	float m_fTransitionTimer = 0.0f;
	const float m_fTransitionDuration = 2.0f;
	bool m_bShowFlash = false;
	float m_fMoveTimer = 0.0f;
	int m_iCurEnemyIdx = 0;

	Vector2 m_vPlayerStartPos = {1,1};
	Vector2 m_vLastPlayerPos;

	float m_fTickCollisionTime = 0.0f;
	std::vector<Actor*> m_vecPlayers;	
	std::vector<Actor*> m_vecMonsters;
};