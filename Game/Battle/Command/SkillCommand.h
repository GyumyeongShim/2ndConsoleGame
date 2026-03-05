#pragma once
#include "Interface/IBattleCommand.h"

class SkillCommand : public Wannabe::IBattleCommand
{
public:
	SkillCommand(Wannabe::Actor* instigator, Wannabe::Actor* pTarget, int iSkillTid);

public:
	virtual void Execute(Wannabe::BattleContext& context) override;

private:
	int m_iSkillTid = 0;
};