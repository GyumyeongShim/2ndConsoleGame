#pragma once
#include <vector>

#include "Engine/Engine.h"
#include "Actor/Actor.h"

#include "Battle/Event/GameBattleEventFactory.h"

//레벨 관리에 사용할 열거형.
enum class State
{
	None = -1,
	Title = 0,
	GamePlay = 1,
	Battle = 2
};

class MainLevel;
class BattleLevel;

class Game : public Engine // 모든 레벨을 관리하는 총괄 매니저 느낌
{
public:
	Game();
	~Game();

	void ToggleMenu();

	void BattleStart(std::vector<Actor*> vecPlayerParty, std::vector<Actor*>vecEnemyParty);
	void BattleEnd();

	BattleLevel* GetBattleLevel() { return m_pBattleLevel; }
	static Game& Get();
private:
	void Init();

private:
	std::vector<Level*> m_vecLevels; // 게임에서 관리하는 레벨을 저장할 배열

	State state = State::GamePlay; // 현재 활성화된 레벨을 나타내는 상태 변수

	static Game* instance; // 싱글톤

	Level* m_pCurLevel = nullptr;
	BattleLevel* m_pBattleLevel = nullptr;
	std::unique_ptr<GameBattleEventFactory> m_pBattleFactory;
};

