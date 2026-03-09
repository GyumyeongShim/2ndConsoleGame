#pragma once
#include <memory>
#include <vector>
#include <string>

#include "Enum/CombatType.h"
#include "Interface/IBattleEventFactory.h"

class Wannabe::Actor;

class GameBattleEventFactory : public Wannabe::IBattleEventFactory
{
public:
    std::unique_ptr<Wannabe::ICutsceneEvent> CreateAsciiAnimation(const std::vector<std::vector<std::wstring>>& frames, float fFrameDelay) override;
    std::unique_ptr<Wannabe::ICutsceneEvent> CreateAsciiParticle(const Wannabe::Vector2& pos, int iParticleCnt, float fDuration) override;
    std::unique_ptr<Wannabe::ICutsceneEvent> CreatePhaseChange(BattleState nextState) override;
    std::unique_ptr<Wannabe::ICutsceneEvent> CreateDmg(Wannabe::Actor* pAtker, Wannabe::Actor* pTarget,
        int iDmg,bool bCritical) override;
    std::unique_ptr<Wannabe::ICutsceneEvent> CreateDeath(Wannabe::Actor* pTarget) override;
    std::unique_ptr<Wannabe::ICutsceneEvent> CreateDelay(float fDeltaTime) override;
    std::unique_ptr<Wannabe::ICutsceneEvent> CreateLog(const BattleLog& log) override;
    std::unique_ptr<Wannabe::ICutsceneEvent> CreateMove(Wannabe::Actor* pActor, const Wannabe::Vector2& targetPos, float fDuration) override;
    
    std::unique_ptr<Wannabe::ICutsceneEvent> CreateVisualEffect(Wannabe::Vector2 start, Wannabe::Vector2 end,
        EffectMovementType eType, wchar_t txt, Wannabe::Color color, float fDuration) override;
    std::unique_ptr<Wannabe::ICutsceneEvent> CreateTurnEnd(Wannabe::Actor* actor) override;
    std::unique_ptr<Wannabe::ICutsceneEvent> CreateTurnStart(Wannabe::Actor* actor) override;
};