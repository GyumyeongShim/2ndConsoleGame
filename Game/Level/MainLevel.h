#pragma once
#include "Level/Level.h"
#include "Engine/Engine.h"
#include "Math/Vector2.h"

class Player;
class TileMap;

class MainLevel : public Level
{
	RTTI_DECLARATIONS(MainLevel, Level)
public:
	MainLevel();
	~MainLevel();

	bool CanMove(const Vector2& player, const Vector2 nextPos);

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float fDeltaTime) override;
	virtual void Draw(Wannabe::RenderSystem& renderSys) override;
	virtual void ConfigureRenderer(Wannabe::RenderSystem& renderer) const override;

private:
	void Init();
	void CheckRandomEncounter();

private:
	std::unique_ptr<TileMap> m_worldMap;
	Player* m_pPlayer = nullptr;
	bool m_RequestBattleTransition;
	Actor* m_BattleTarget;

	Vector2 m_vPlayerStartPos = {1,1};
	Vector2 m_vLastPlayerPos;

	float m_fTickCollisionTime;
	std::vector<Actor*> m_vecPlayers;	
	std::vector<Actor*> m_vecMonsters;	
};