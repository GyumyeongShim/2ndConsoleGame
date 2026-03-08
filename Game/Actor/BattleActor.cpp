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
}

void BattleActor::CloneStat(Wannabe::Actor* origin, int slotNum, Team teamType)
{
    if (origin == nullptr)
        return;

	m_pOrigin = origin;
    m_iActorNum = slotNum; //0~4로 지정되어있음 
    m_eTeam = teamType;

    if (origin->GetComponent<StatComponent>())
    {
        StatComponent* pNewStat = new StatComponent(*origin->GetComponent<StatComponent>());
        pNewStat->ResetTurn();

        AddComponent(pNewStat);
    }

    if (origin->GetComponent<StatusComponent>())
    {
        StatusComponent* pNewStatus = new StatusComponent(*origin->GetComponent<StatusComponent>());
        pNewStatus->SetOwner(this);

        AddComponent(pNewStatus);
    }

    if (origin->GetComponent<DisplayComponent>())
    {
        AddComponent(new DisplayComponent(*origin->GetComponent<DisplayComponent>()));
    }

    if (origin->GetComponent<EquipmentComponent>())
    {
        AddComponent(new EquipmentComponent(*origin->GetComponent<EquipmentComponent>()));
    }

    if (origin->GetComponent<InventoryComponent>())
    {
        AddComponent(new InventoryComponent(*origin->GetComponent<InventoryComponent>()));
    }

    if (origin->GetComponent<SkillComponent>())
    {
        AddComponent(new SkillComponent(*origin->GetComponent<SkillComponent>()));
    }

    SetSlotPos(slotNum);
}

void BattleActor::Tick(float deltaTime)
{
    super::Tick(deltaTime);

    if (m_bIsMoving)
    {
        // 현재 위치에서 목표 위치로 서서히 이동 (보간)
        Wannabe::Vector2 currentPos = GetPosition();
        Wannabe::Vector2 dir = m_vTargetPos - currentPos;
        float distance = dir.Length();

        if (distance < 0.1f) // 도착 판정
        {
            SetPosition(m_vTargetPos);
            m_bIsMoving = false;
        }
        else
        {
            dir.Normalized();
            SetPosition(currentPos + (dir * m_fMoveSpeed * deltaTime));
        }
    }
}

void BattleActor::Draw(RenderSystem& renderSys)
{
    Wannabe::Vector2 screenPos = renderSys.GetCamera().WorldToScreen(GetPosition());
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
    auto* display = this->GetComponent<DisplayComponent>();
    if (display == nullptr)
        return;

    display->SetNameColor(bIsTargeted == true ? Color::Magenta : m_OrignColor);

    // 항상 원본 이름 기준으로 설정
    std::wstring name = display->GetOriginName();
    display->SetName(bIsTargeted ? L"[" + name + L"]" : name);
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

void BattleActor::MoveTo(const Wannabe::Vector2& targetGridPos)
{
    m_vTargetPos = targetGridPos;
    m_bIsMoving = true;
}