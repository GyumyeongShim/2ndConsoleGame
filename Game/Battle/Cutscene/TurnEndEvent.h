#pragma once
#include "Interface/ICutsceneEvent.h"

namespace Wannabe
{
	class Actor;
	class BattleContext;
}

class TurnEndEvent : public Wannabe::ICutsceneEvent
{
public:
	TurnEndEvent(Wannabe::Actor* actor);

	void OnStart(Wannabe::BattleContext& context);
	bool Update(Wannabe::BattleContext& context, float fDeltaTime);
	virtual CutsceneEventType GetEventType() override
	{
		m_eCutsceneEventType = CutsceneEventType::TurnEnd;
		return m_eCutsceneEventType;
	};

private:
	Wannabe::Actor* m_pTarget;
};