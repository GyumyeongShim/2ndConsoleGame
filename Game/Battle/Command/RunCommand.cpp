#include "RunCommand.h"
#include "Battle/BattleContext.h"

RunCommand::RunCommand(Wannabe::Actor* self)
{
}

void RunCommand::Execute(Wannabe::BattleContext& context)
{
	context.EndBattle();
}