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

	void MoveTo(const Wannabe::Vector2& targetPos);

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float fDeltaTime) override;
	virtual void Draw(Wannabe::RenderSystem& renderSys) override;

private:
	void Init();
	void MoveLogic(float fDeltaTime);
	void MainLevelMovement();

private:
	MovementIntent m_MoveIntent;
	int m_iTid;

	Wannabe::Vector2 m_vTargetPos;
	bool    m_bIsMoving = false;
	float   m_fMoveTimer = 0.0f;
};
