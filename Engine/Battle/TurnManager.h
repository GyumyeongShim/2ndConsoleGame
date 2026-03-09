#pragma once
#include <vector>
#include "Core/Common.h"

namespace Wannabe 
{
	class Actor;

	class WANNABE_API TurnManager
	{
	public:
		TurnManager();
		~TurnManager() = default;

		Actor* GetNextActor(const std::vector<Actor*>& players, const std::vector<Actor*>& enemies);

		void ProgressTurns(); //각 Actor 턴 진행
		void TurnEnd();
		void ResetTurns();

		static TurnManager& Get();
		Actor* GetCurBattleActor() { return m_CurActor; }
		const std::vector<Actor*> GetTurnQueue();

	private:
		static TurnManager* instance; // 싱글톤

		std::vector<Actor*> m_vecActors; //내부에서 속도, 상태이상에 따른 우선순위를 위해 통합용
		Actor* m_CurActor = nullptr;
	};
}