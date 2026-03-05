#pragma once
#include "Interface/IBattleCommand.h"

namespace Wannabe
{
	class Actor;
	class Item;
}

class ItemCommand : public Wannabe::IBattleCommand
{
public:
	ItemCommand(Wannabe::Actor* pIntsigator, Wannabe::Actor* pTarget, int iItemTID);

public:
	virtual void Execute(Wannabe::BattleContext& context) override;

private:
	Wannabe::Actor* m_pInstigator = nullptr;
	Wannabe::Actor* m_pTarget = nullptr;
	int m_iItemTID = 0;
};