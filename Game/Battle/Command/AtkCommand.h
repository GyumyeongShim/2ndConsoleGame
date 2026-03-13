#pragma once
#include "Interface/IBattleCommand.h"

class AtkCommand : public Wannabe::IBattleCommand
{
public:
	AtkCommand(Wannabe::Actor* instigator, Wannabe::Actor* pTarget);
	virtual ~AtkCommand() override {}

public:
	virtual bool Execute(Wannabe::BattleContext& context) override;
};