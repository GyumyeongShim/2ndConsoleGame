#pragma once
#include <vector>
#include <memory>

namespace Wannabe
{
    class Actor;
    class IBattleCommand;
}

enum class BattleState;
class IBattleLevel
{
public:
    virtual ~IBattleLevel() = default;

    virtual void AddActor(std::unique_ptr<Wannabe::Actor>actor) = 0;
    virtual void RequestBattleStateChange(BattleState state) = 0;
    virtual void PushCommand(std::unique_ptr<Wannabe::IBattleCommand> cmd) = 0;

    virtual const std::vector<Wannabe::Actor*>& GetEnemyParty() const = 0;
    virtual const std::vector<Wannabe::Actor*>& GetPlayerParty() const = 0;
};