#pragma once
#include <vector>

#include "Engine/Engine.h"
#include "Actor/Actor.h"

#include "Battle/Event/GameBattleEventFactory.h"

enum class State
{
	None = -1,
	Title = 0,
	GamePlay = 1,
	Battle = 2
};

class MainLevel;
class BattleLevel;
struct RunGameData;

class Game : public Engine
{
public:
	Game();
	~Game();

	virtual void OnFrameEnd() override;

	void InitNewGame();
	void RequestChangeLevel(const size_t levelID);
	void ChangeLevel(const size_t levelID);

	void BattleStart(std::vector<Actor*> vecPlayerParty, std::vector<Actor*>vecEnemyParty);
	void BattleEnd();
	void ProcessBattleReward(int gold, int exp);

	void SaveGame(int iSlotIdx);
	void LoadGame(int iSlotIdx);

	BattleLevel* GetBattleLevel() { return m_pBattleLevel; }
	RunGameData* GetRunData() { return m_pRunData.get(); }

	static Game& Get();

private:
	void Init();
	Level* CreateLevel(const size_t levelID);

private:
	static Game* instance; // 싱글톤

	State m_eState = State::Title;
	
	size_t m_nNextLevelID = 0; // 다음 level id 예약
	bool m_bLevelChangeReserved = false;

	std::vector<Level*> m_vecLevels;
	Level* m_pCurLevel = nullptr; //필드/던전, 마을
	BattleLevel* m_pBattleLevel = nullptr; //현재 전투 레벨

	std::unique_ptr<GameBattleEventFactory> m_pBattleFactory; //log,dmg,delay 등등 이벤트

	std::unique_ptr<RunGameData> m_pRunData; // 레벨전환시 해당 데이터로 재생성
};