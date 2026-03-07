#include "Monster.h"
#include "Manager/DataManager.h"

#include "Level/BattleLevel.h"

#include "Component/StatComponent.h"
#include "Component/StatusComponent.h"
#include "Component/DisplayComponent.h"
#include "Component/EquipmentComponent.h"
#include "Component/InventoryComponent.h"
#include "Component/SkillComponent.h"

Monster::Monster(int iTID)
	:super(L" ", Vector2::Zero, Color::Magenta), m_iTID(iTID)
{
	m_eTeam = Team::Enemy;
	Init();
}

Monster::~Monster()
{
}

void Monster::Tick(float fDeltaTime)
{
	super::Tick(fDeltaTime);
}

void Monster::Draw(Wannabe::RenderSystem& renderSys)
{
	if (GetOwner()->IsTypeOf<BattleLevel>() == false)
		return;

	super::Draw(renderSys);
}

void Monster::Init()
{
	const MonsterData* data = DataManager::Get().GetMonsterData(m_iTID);
	if(data == nullptr)
		return;

	StatComponent* pStat = new StatComponent();
	if (pStat == nullptr)
		return;

	StatData stat;
	stat.iLevel = 1;
	stat.iMaxHp = data->baseStat.iMaxHp;
	stat.iHp = data->baseStat.iMaxHp;
	stat.iAtk = data->baseStat.iAtk;
	stat.iDef = data->baseStat.iDef;
	stat.iSpd = data->baseStat.iSpd;

	stat.iAccuracy = data->baseStat.iAccuracy;
	stat.iEvasion = data->baseStat.iEvasion;
	stat.iCritChance = data->baseStat.iCritChance;
	stat.iCritResist = data->baseStat.iCritResist;

	stat.iMaxExp = data->expReward;
	stat.iExp = 0;

	stat.iTurnCnt = 0;
	stat.iMaxTurnCnt = 10;

	pStat->SetStatByData(stat);
	AddComponent(pStat);

	// 시각적 데이터 초기화
	DisplayComponent* pDisplay = new DisplayComponent();
	pDisplay->SetDisplayByData(data->visual);
	AddComponent(pDisplay);

	StatusComponent* pStatus = new StatusComponent();
	pStatus->SetOwner(this);
	AddComponent(pStatus);
	
	SkillComponent* pSkill = new SkillComponent();
	for (int TID : data->skillIds)
	{
		pSkill->AddSkill(TID);
	}
	AddComponent(pSkill);
}