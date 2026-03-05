#pragma once
#include "Actor/Actor.h"
#include "Data/MonsterData.h"

class Monster :  public Wannabe::Actor
{
	RTTI_DECLARATIONS(Monster,Actor)

public:
	Monster(int iTID);
	~Monster();

protected:
	virtual void Tick(float fDeltaTime) override;
	virtual void Draw(Wannabe::RenderSystem& renderSys) override;

private:
	void Init();

private:
	int m_iTID; //Monster TID;
	CharacterVisualData m_VisualData; //몬스터 시각적 데이터
};

