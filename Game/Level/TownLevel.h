#pragma once
#include "Level/Level.h"
#include "Math/Vector2.h"

class Player;
class TileMap;
class RunGameData;

class TownLevel : public Level
{
	RTTI_DECLARATIONS(TownLevel, Level)
public:
	TownLevel();
	virtual ~TownLevel();

protected:
	virtual void Tick(float fDeltaTime)override;
	virtual void Draw(Wannabe::RenderSystem& renderSys) override;
	virtual void ConfigureRenderer(Wannabe::RenderSystem& renderer) const override;
	virtual void OnEnterLevel(RunGameData* pData) override;
	virtual void OnExitLevel(RunGameData* pData) override;
	virtual bool CanMove(const Vector2& nextPos);

private:
    void Init();
    void CheckPortal();

private:
    std::unique_ptr<TileMap> m_worldMap;

    Player* m_pPlayer = nullptr;
    Vector2 m_vPlayerStartPos = { 2, 2 };

    std::vector<Actor*> m_vecNPCs;
};