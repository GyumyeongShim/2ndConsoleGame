#pragma once
#include <memory>
#include <vector>
#include <functional>

#include "Core/Common.h"
#include "Battle/BattleEventProcessor.h"
#include "Battle/BattleResolver.h"
#include "Battle/CheckChainSystem.h"

class IBattleLevel;
class BattleLogSystem;
class CutscenePlayer;
enum class BattleState;
enum class ActionTargetType;

namespace Wannabe
{
    class Actor;
    class IBattleEventFactory;
    class IRemoveActorCallback;

    class WANNABE_API BattleContext
    {
    public:
        using RemoveActorFunc = std::function<void(Wannabe::Actor*)>;

        BattleContext(BattleLogSystem* logSystem, CutscenePlayer* cutscenePlayer, RemoveActorFunc removeFunc,
            IBattleLevel* level, IBattleEventFactory* factory, IRemoveActorCallback* callback);

        void SpawnActor(Actor* actor);
        void SetBattleState(BattleState state);
        void EndBattle();

        void SetBattleLevel(IBattleLevel* level);
        void SetEventFactory(IBattleEventFactory* factory);
        void SetRemoveCallback(BattleEventProcessor::RemoveActorFunc func);

        bool IsValidActor(Actor* pTarget) const;
        bool IsEnemy(Actor* actor) const;

        BattleState GetBattleState();
        std::vector<Actor*> GetEnemyParty(Actor* actor);
        std::vector<Actor*> GetPlayerParty(Actor* actor);

        IBattleLevel& GetBattleLevel() { return *m_pLevel; }
        BattleLogSystem& GetLogSystem() { return *m_pBattleLogSystem; }
        CutscenePlayer& GetCutscenePlayer() { return *m_pCutscenePlayer; }
        BattleEventProcessor& GetEventProcessor() { return *m_BattleEventProcessor; }
        BattleResolver& GetResolver() { return m_BattleResolver; }
        CheckChainSystem& GetCheckChain() { return m_CheckChain; }

    private:
        std::unique_ptr<BattleEventProcessor> m_BattleEventProcessor;
        BattleResolver m_BattleResolver;
        CheckChainSystem m_CheckChain;
        BattleLogSystem* m_pBattleLogSystem = nullptr;
        CutscenePlayer* m_pCutscenePlayer = nullptr;

        RemoveActorFunc m_RemoveFunc;
        IBattleLevel* m_pLevel = nullptr;
        IBattleEventFactory* m_pEventFactory = nullptr;
    };
}