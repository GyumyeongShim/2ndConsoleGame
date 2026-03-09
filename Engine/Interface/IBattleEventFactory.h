#pragma once
#include <memory>
#include <string>

#include "Interface/ICutsceneEvent.h"

enum class EAniType;

namespace Wannabe
{
	class Actor;
	class Vector2;
	class IBattleEventFactory
	{
	public:
			virtual ~IBattleEventFactory() = default;
			virtual std::unique_ptr<ICutsceneEvent> CreateAsciiAnimation(Actor* pOwner, EAniType eType, float fFrameDelay = 0.4f) = 0;
			virtual std::unique_ptr<ICutsceneEvent> CreateAsciiParticle(const Vector2& pos, int iParticleCnt, float fDuration) = 0;
			virtual std::unique_ptr<ICutsceneEvent> CreatePhaseChange(BattleState nextState) = 0;
			virtual std::unique_ptr<ICutsceneEvent> CreateDmg(Actor* pAtker, Actor* pTarget,
				int iDmg, bool bCritical) = 0;
			virtual std::unique_ptr<ICutsceneEvent> CreateDeath(Actor* pTarget) = 0;
			virtual std::unique_ptr<ICutsceneEvent> CreateDelay(float fDeltaTime) = 0;
			virtual std::unique_ptr<ICutsceneEvent> CreateLog(const BattleLog& log) = 0;
			virtual std::unique_ptr<ICutsceneEvent> CreateMove(Actor* pActor, const Vector2& targetPos, float fDuration) = 0;
			virtual std::unique_ptr<ICutsceneEvent> CreateVisualEffect(Vector2 start, Vector2 end,
				EffectMovementType eType, wchar_t txt, Color color, float fDuration) = 0;
			virtual std::unique_ptr<ICutsceneEvent> CreateTurnEnd(Actor* actor) = 0;
			virtual std::unique_ptr<ICutsceneEvent> CreateTurnStart(Actor* actor) = 0;
	};
}