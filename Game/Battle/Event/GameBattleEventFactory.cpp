#include "GameBattleEventFactory.h"

#include "Battle/Cutscene/AsciiAnimationEvent.h"
#include "Battle/Cutscene/AsciiParticleEvent.h"
#include "Battle/Cutscene/BattlePhaseChangeEvent.h"
#include "Battle/Cutscene/DamageEvent.h"
#include "Battle/Cutscene/DeathEvent.h"
#include "Battle/Cutscene/DelayEvent.h"
#include "Battle/Cutscene/LogEvent.h"
#include "Battle/Cutscene/MoveEvent.h"
#include "Battle/Cutscene/VisualEffectEvent.h"
#include "Battle/Cutscene/TurnEndEvent.h"
#include "Battle/Cutscene/TurnStartEvent.h"

std::unique_ptr<Wannabe::ICutsceneEvent> GameBattleEventFactory::CreateAsciiAnimation(Wannabe::Actor* pOwner, EAniType eType, float fFrameDelay)
{
    return std::make_unique<AsciiAnimationEvent>(pOwner, eType, fFrameDelay);
}

std::unique_ptr<Wannabe::ICutsceneEvent> GameBattleEventFactory::CreateAsciiParticle(const Wannabe::Vector2& pos, int iParticleCnt, float fDuration)
{
    return std::make_unique<AsciiParticleEvent>(pos, iParticleCnt, fDuration);
}

std::unique_ptr<Wannabe::ICutsceneEvent> GameBattleEventFactory::CreatePhaseChange(BattleState nextState)
{
    return std::make_unique<BattlePhaseChangeEvent>(nextState);
}

std::unique_ptr<Wannabe::ICutsceneEvent> GameBattleEventFactory::CreateDmg(Wannabe::Actor* pAtker, Wannabe::Actor* pTarget, int iDmg, bool bCritical)
{
    return std::make_unique<DamageEvent>(pAtker, pTarget, iDmg, bCritical);
}

std::unique_ptr<Wannabe::ICutsceneEvent> GameBattleEventFactory::CreateDeath(Wannabe::Actor* target)
{
    return std::make_unique<DeathEvent>(target);
}

std::unique_ptr<Wannabe::ICutsceneEvent> GameBattleEventFactory::CreateDelay(float fDeltaTime)
{
    return std::make_unique<DelayEvent>(fDeltaTime);
}

std::unique_ptr<Wannabe::ICutsceneEvent> GameBattleEventFactory::CreateLog(const BattleLog& log)
{
    return std::make_unique<LogEvent>(log);
}

std::unique_ptr<Wannabe::ICutsceneEvent> GameBattleEventFactory::CreateMove(Wannabe::Actor* pActor, const Wannabe::Vector2& targetPos, float fDuration)
{
    return std::make_unique<MoveEvent>(pActor, targetPos, fDuration);
}

std::unique_ptr<Wannabe::ICutsceneEvent> GameBattleEventFactory::CreateVisualEffect(Wannabe::Vector2 start, Wannabe::Vector2 end, EffectMovementType eType, wchar_t txt, Wannabe::Color color, float fDuration)
{
    return std::make_unique<VisualEffectEvent>(start, end, eType, txt, color, fDuration);
}

std::unique_ptr<Wannabe::ICutsceneEvent> GameBattleEventFactory::CreateTurnEnd(Wannabe::Actor* actor)
{
    return std::make_unique<TurnEndEvent>(actor);
}

std::unique_ptr<Wannabe::ICutsceneEvent> GameBattleEventFactory::CreateTurnStart(Wannabe::Actor* actor)
{
    return std::make_unique<TurnStartEvent>(actor);
}