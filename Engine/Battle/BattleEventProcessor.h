#pragma once
#include <functional>
#include <vector>
#include <string>

#include "Core/Common.h"
#include "Enum/CombatType.h"

class CutscenePlayer;

namespace Wannabe
{
	using RemoveActorFunc = void(*)(Actor*);
	class Actor;
	class IBattleEventFactory;
	class BattleContext;
	class IRemoveActorCallback;

	class WANNABE_API BattleEventProcessor //Event ½ÇÇà
	{
	public:
		using RemoveActorFunc = IRemoveActorCallback*;

		BattleEventProcessor(IBattleEventFactory* factory, IRemoveActorCallback* callback);

		void ProcessCombatEffectResult(BattleContext& context, const CombatEffectResult& result, int iDepth = 0);

		void MarkForRemoval(Actor* pTarget);
		void FlushRemoval();
		void RemoveActor(Actor* actor);

		void SetRemoveCallback(IRemoveActorCallback* callback);
		void SetEventFactory(IBattleEventFactory* factory);

	private:
		void ApplyDmg(BattleContext& context, const CombatEffect& effect);
		void ApplyHeal(BattleContext& context, const CombatEffect& effect);
		void ApplyStatus(BattleContext& context, const CombatEffect& effect);
		void ApplyEffect(BattleContext& context, const CombatEffect& effect);

	private:
		IRemoveActorCallback* m_pRemoveCallback = nullptr;
		IBattleEventFactory* m_pEventFactory = nullptr;
		std::vector<Actor*> m_vecPendingRemoval;

		int m_CurDepth = 0;
		int m_MaxDepth = 3;
	};
}