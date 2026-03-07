#pragma once
#include <unordered_map>

#include "Core/Common.h"

#include "../Game/Data/PlayerData.h"
#include "../Game/Data/MonsterData.h"
#include "../Game/Data/ActionData.h"

namespace Wannabe
{
	class WANNABE_API DataManager
	{
	public:
		static DataManager& Get()
		{
			static DataManager instance;
			return instance;
		}
		void Init();

		void LoadPlayers(const std::vector<PlayerData>& players);
		void LoadMonsters(const std::vector<MonsterData>& monsters);
		void LoadActions(const std::vector<ActionData>& monsters); // 스킬, 아이템

		const PlayerData* GetPlayerData(int iTID);
		const MonsterData* GetMonsterData(int iTID);
		const ActionData* GetActionData(int iTID); //스킬, 아이템 통합

	private:
		std::unordered_map<int, PlayerData> m_PlayerTable;
		std::unordered_map<int, MonsterData> m_MonsterTable;
		std::unordered_map<int, ActionData> m_ActionTable;
	};
}