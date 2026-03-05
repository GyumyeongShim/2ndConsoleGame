#pragma once
#include <vector>
#include <memory>

#include "Interface/IBattleCommand.h"

namespace Wannabe
{
    class Actor;
    class BattleContext;
}

class BattleActor;
class EnemyAI
{
public:
    static std::unique_ptr<Wannabe::IBattleCommand> Decide(Wannabe::Actor* self, const std::vector<Wannabe::Actor*>& players);

private:
    static Wannabe::Actor* SelectTarget_Hp(const std::vector<Wannabe::Actor*>& players);
    static Wannabe::Actor* SelectTarget_Random(const std::vector<Wannabe::Actor*>& players);

    static bool ShouldUseSkill(Wannabe::Actor* self);
    static bool ShouldRun(Wannabe::Actor* self);
    static int SelectUsableSkill(Wannabe::Actor* self);
};