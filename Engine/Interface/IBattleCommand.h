#pragma once
#include "Common/Common.h"
#include "Actor/Actor.h"
#include "Battle/BattleContext.h"

enum class CommandType
{
	Atk,
	Skill,
	Item,
	Run
};

namespace Wannabe
{
	class WANNABE_API IBattleCommand
	{
	public:
		IBattleCommand();
		virtual ~IBattleCommand();
		virtual void Execute(BattleContext& context) = 0;

	protected:
		Actor* m_pInstigator = nullptr;
		Actor* m_pTarget = nullptr;
		CommandType m_eType = CommandType::Atk;
	};
}