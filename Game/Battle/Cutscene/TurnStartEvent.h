#pragma once
#include "Interface/ICutsceneEvent.h"

namespace Wannabe
{
	class Actor;
	class BattleContext;
}

class TurnStartEvent : public Wannabe::ICutsceneEvent
{
public:
	TurnStartEvent(Wannabe::Actor* actor);

	void OnStart(Wannabe::BattleContext& context);
	bool Update(Wannabe::BattleContext& context, float fDeltaTime);
	virtual CutsceneEventType GetEventType() override
	{
		m_eCutsceneEventType = CutsceneEventType::TurnStart;
		return m_eCutsceneEventType;
	};

private:
	Wannabe::Actor* m_pTarget;
};