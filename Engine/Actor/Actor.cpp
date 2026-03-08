#include "Actor.h"
#include <iostream>

#include "Util/Utill.h"
#include "Render/RenderSystem.h"
#include "Component/StatComponent.h"
#include "Component/EquipmentComponent.h"

namespace Wannabe
{
	Actor::Actor(const wchar_t* image, const Vector2& pos, Color color)
		:m_Pos(pos), m_Color(color), m_eTeam(Team::Neutral), m_strImage(image)
	{
		m_iWidth = static_cast<int>(m_strImage.length());
	}

	Actor::~Actor()
	{
		for (auto* pComp : m_vecComponents)
			SafeDelete(pComp);

		m_vecComponents.clear();
	}

	void Actor::BeginPlay()
	{
		if (HasBeganPlay() == true)
			return;

		m_bHasBeganPlay = true;
	}

	void Actor::Tick(float fDeltaTime)
	{
		for (auto* pComp : m_vecComponents)
		{
			pComp->Update(fDeltaTime);
		}
	}

	void Actor::Draw(Wannabe::RenderSystem& renderSys)
	{
		renderSys.DrawActor(*this);
	}

	void Actor::Destroy()
	{
		if (m_bDestroyRequested == true)
			return;

		m_bDestroyRequested = true;

		OnDestroy();
	}

	void Actor::OnDestroy()
	{
	}

	bool Actor::TestIntersect(const Actor* const other)
	{
		//aabb
		//현재는 크기가 1이기때문에 x좌표만 고려하면 됨
		int xMin = m_Pos.x;
		int xMax = m_Pos.x + m_iWidth - 1;

		int otherxMin = other->GetPosition().x;
		int otherxMax = other->GetPosition().x + other->m_iWidth - 1;

		// 안 겹치는 조건 확인
		if (otherxMin > xMax) //내 오른쪽 좌표보다 더 오른쪾에 있는 경우
			return false;

		if (otherxMax < xMin) // 내 왼쪽 좌표보다 더 왼쪽에 있는 경우.
			return false;

		// y는 크기가 1이기 때문에 좌표가 같은지 여부만 확인.
		return m_Pos.y == other->m_Pos.y;
	}

	void Actor::ChangeImage(const wchar_t* newImage)
	{
		m_strImage = newImage;
		m_iWidth = static_cast<int>(m_strImage.length());
	}

	void Actor::AddComponent(Component* pComponent)
	{
		if (pComponent == nullptr) 
			return;

		pComponent->SetOwner(this);
		m_vecComponents.emplace_back(pComponent);
	}

	bool Actor::EquipItem(ItemInstance* inst)
	{
		auto* pEquip = GetComponent<EquipmentComponent>();
		auto* pStat = GetComponent<StatComponent>();

		if (pEquip && pStat)
		{
			return pEquip->Equip(inst, pStat);
		}

		return false;
	}

	void Actor::UnequipItem(EquipSlot eSlot)
	{
		auto* pEquip = GetComponent<EquipmentComponent>();
		auto* pStat = GetComponent<StatComponent>();

		if (pEquip && pStat)
		{
			pEquip->Unequip(eSlot, pStat);
		}
	}
}