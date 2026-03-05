#include <cmath>

#include "BattleActor.h"
#include "Item/Item.h"
#include "Battle/BattleContext.h"
#include "Util/Utill.h"

#include "Player.h"
#include "Monster.h"

#include "UI/UI_HPBar.h"

#include "Engine/Engine.h" //todo
#include "Component/StatComponent.h"
#include "Component/StatusComponent.h"
#include "Component/DisplayComponent.h"
#include "Component/EquipmentComponent.h"
#include "Component/InventoryComponent.h"
#include "Component/SkillComponent.h"

BattleActor::~BattleActor()
{
    m_pOrigin = nullptr;
    SafeDelete(m_pStatComponent);
    SafeDelete(m_pStatusComponent);
    SafeDelete(m_pDisplayComponent);
    SafeDelete(m_pEquipmentComponent);
    SafeDelete(m_pInventoryComponent);
    SafeDelete(m_pSkillComponent);
}

void BattleActor::CloneStat(Wannabe::Actor* origin, int slotNum, Team teamType)
{
    if (origin == nullptr)
        return;

	m_pOrigin = origin;
    if (m_pOrigin->GetStat())
    {
        m_pStatComponent = new StatComponent(*m_pOrigin->GetStat());
        m_pStatComponent->ResetTurn(); // 턴 카운트 초기화 (턴 진행은 BattleActor에서 관리하기 때문에) } 
    }

    if (m_pOrigin->GetStatus())
    {
        m_pStatusComponent = new StatusComponent(*m_pOrigin->GetStatus());
        m_pStatusComponent->SetOwner(m_pOrigin);
    }

    if (m_pOrigin->GetDisplay())
    {
        m_pDisplayComponent = new DisplayComponent(*m_pOrigin->GetDisplay());
    }

    if (m_pOrigin->GetEquip())
    {
        m_pEquipmentComponent = new EquipmentComponent(*m_pOrigin->GetEquip());
    }

    if (m_pOrigin->GetInven())
    {
        m_pInventoryComponent = new InventoryComponent(*m_pOrigin->GetInven());
    }

    if (m_pOrigin->GetSKill())
    {
        m_pSkillComponent = new SkillComponent(*m_pOrigin->GetSKill());
    }

    m_iActorNum = slotNum; //0~4로 지정되어있음 
    m_eTeam = teamType; 
    SetSlotPos(slotNum);
}

void BattleActor::Tick(float deltaTime)
{
    super::Tick(deltaTime);
    m_pStatComponent->Update(deltaTime);
}

void BattleActor::Draw(RenderSystem& renderSys)
{
    super::Draw(renderSys);
}

void BattleActor::SetSlotPos(int slotNum)
{
    if (m_eTeam == Team::Enemy)
    {
        m_BattleScreenPos.x = 6 + (slotNum * 14);
        m_BattleScreenPos.y = 5;
    }
    else if (m_eTeam == Team::Player)
    {
        int height = Engine::Get().GetRenderSystem().GetScreenSize().y;

        m_BattleScreenPos.x = 2;
        m_BattleScreenPos.y = height - 5 + slotNum;
    }

    m_Pos = m_BattleScreenPos;
}

void BattleActor::SetTargeted(bool bIsTargeted)
{
    // 타켓 메뉴에서 지정 받았을 경우 진행
    if (m_pDisplayComponent == nullptr)
        return;

    m_pDisplayComponent->SetNameColor(bIsTargeted == true ? Color::RightMagenta : m_OrignColor);

    // 항상 원본 이름 기준으로 설정
    std::wstring name = m_pDisplayComponent->GetOriginName();
    m_pDisplayComponent->SetName(bIsTargeted ? L"[" + name + L"]" : name);
}

bool BattleActor::UseItem(Wannabe::Item * pItem, Wannabe::Actor* pTarget, Wannabe::BattleContext& eContext)
{
    if (pItem == nullptr)
        return false;

    if (pItem->GetItemType() == ItemType::Consumable)
    {
        return false;
    }

    if (pItem->GetItemType() == ItemType::Equipment)
    {
        return EquipItem(pItem, eContext);
    }

    return false;
}

bool BattleActor::EquipItem(Wannabe::Item* pItem, Wannabe::BattleContext& eContext)
{
    return false;
}
