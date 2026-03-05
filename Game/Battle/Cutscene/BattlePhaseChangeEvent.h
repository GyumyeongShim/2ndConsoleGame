#pragma once
#include "Interface/ICutsceneEvent.h"
#include "Level/BattleLevel.h"

class BattlePhaseChangeEvent : public Wannabe::ICutsceneEvent
{
public:
	BattlePhaseChangeEvent(BattleState nextState);

	void OnStart(Wannabe::BattleContext& context) override;
	bool Update(Wannabe::BattleContext& context,float fDeltaTime) override;
	void OnEnd(Wannabe::BattleContext& context) override;
	bool IsValid(Wannabe::BattleContext& context) const override;

private:
	BattleState m_eNextState;
	float m_Elapsed = 0.0f;
};

