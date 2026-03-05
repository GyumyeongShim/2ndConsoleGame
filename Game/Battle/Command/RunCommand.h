#pragma once
#include "Interface/IBattleCommand.h"
#include "Actor/Actor.h"

class RunCommand : public Wannabe::IBattleCommand
{
public:
	RunCommand(Wannabe::Actor* self);

public:
	virtual void Execute(Wannabe::BattleContext& context) override;
};