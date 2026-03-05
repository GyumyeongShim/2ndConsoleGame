#pragma once
#include <vector>
#include <string>

#include "Actor/Actor.h"
#include "Math/Vector2.h"
#include "Math/Color.h"
#include "Item/Item.h"

namespace Wannabe
{
	class BattleContext;
}

class BattleActor : public Wannabe::Actor
{
	RTTI_DECLARATIONS(BattleActor, Actor)
public:
	~BattleActor();

	void  CloneStat(Wannabe::Actor* origin,int slotNum = 0, Wannabe::Team teamType = Wannabe::Team::Enemy);

	virtual void Tick(float fDeltaTime) override;
	virtual void Draw(Wannabe::RenderSystem& renderSys) override;

	int GetActorNum() { return m_iActorNum; }
	void SetTargeted(bool bIsTargeted);

	Wannabe::Vector2 GetBattleScreenPosition() { return m_BattleScreenPos; }

	bool UseItem(Wannabe::Item* pItem, Wannabe::Actor* pTarget, Wannabe::BattleContext& eContext);
	bool EquipItem(Wannabe::Item* pItem, Wannabe::BattleContext& eContext);

private:
	void SetSlotPos(int slotNum = 0);

private:
	Wannabe::Color m_OrignColor = Wannabe::Color::White; //글자 색상
	Wannabe::Color m_ImageColor = Wannabe::Color::White; // low hp, 상태이상 등등 여러가지일때 표현할 것
	Wannabe::Actor* m_pOrigin = nullptr;
	int m_iActorNum = 0;
	Wannabe::Vector2 m_BattleScreenPos;
};