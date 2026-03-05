#pragma once
#include <vector>

#include "Actor/Actor.h"
#include "Math/Vector2.h"

class Player : public Wannabe::Actor
{
	struct MovementIntent
	{
		bool bMoveRequested = false;
		Wannabe::Vector2 TargetPos;
	};

	RTTI_DECLARATIONS(Player, Actor)

public:
	Player(int iTid, const Wannabe::Vector2& pos);
	~Player();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float fDeltaTime) override;
	virtual void Draw(Wannabe::RenderSystem& renderSys) override;

private:
	void Init();

private:
	MovementIntent m_MoveIntent;
	int m_iTid;
};
