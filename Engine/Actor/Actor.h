#pragma once
#include <string>
#include <vector>

#include "Core/RTTI.h" 
#include "Math/Vector2.h"
#include "Math/Color.h"
#include "Component/Component.h"

enum class EquipSlot;

namespace Wannabe
{
	enum class Team
	{
		Player,
		Enemy,
		Neutral
	};

	class Level;
	class RenderSystem;
	class ItemInstance;

	class StatComponent;
	class StatusComponent;
	class InventoryComponent;
	class EquipmentComponent;
	class DisplayComponent;
	class SkillComponent;

	//dll로 만들때 dll 밖에서 접근할때 필요한 선언 __declspec(dllexport)
	class WANNABE_API Actor : public RTTI 
	{
		RTTI_DECLARATIONS(Actor, RTTI); //RTTI 매크로 사용

	public:
		Actor(const wchar_t* image = L" ", const Vector2& pos = Vector2::Zero, Color color = Color::White);
		virtual ~Actor(); // 자식 클래스 소멸자 호출을 위해서 virtual 사용

		//게임 플레이 이벤트
		virtual void BeginPlay();
		virtual void Tick(float fDeltaTime);
		virtual void Draw(Wannabe::RenderSystem& renderSys);

		// 삭제
		void Destroy();
		virtual void OnDestroy(); // 삭제가 될 때 호출될 이벤트 함수
		
		// 충돌 여부 확인 함수
		bool TestIntersect(const Actor* const other);

		// 액터의 이미지 값 변경 함수
		void ChangeImage(const wchar_t* newImage);

		// 위치 변경 및 읽기 함수
		void SetPosition(const Vector2& pos) { m_Pos = pos; }
		inline const Vector2& GetPosition() const { return m_Pos; }

		//오너쉽
		void SetOwner(Level* newlevel) { m_Owner = newlevel; }
		inline Level* GetOwner() const { return m_Owner; }

		void AddComponent(Component* pComponent);

		//Getter
		Team GetTeam() const { return m_eTeam; }
		bool HasBeganPlay() const { return m_bHasBeganPlay; }
		bool IsActive() const { return m_bIsActive && !m_bDestroyRequested; }
		bool IsDestroyRequested() const { return m_bDestroyRequested; }
		int GetSortingOrder() const { return m_SortingOrder; }
		int GetWidth() const { return m_iWidth; }
		wchar_t GetGlyph() const { return *m_strImage.c_str(); }
		Color GetGlyphColor() const { return m_Color; }

		template <typename T>
		T* GetComponent()
		{
			for (auto* pComp : m_vecComponents)
			{
				T* pTarget = dynamic_cast<T*>(pComp);
				if (pTarget != nullptr)
				{
					return pTarget;
				}
			}
			return nullptr;
		}

		template <typename T>
		const T* GetComponent() const
		{
			for (auto* pComp : m_vecComponents)
			{
				const T* pTarget = dynamic_cast<const T*>(pComp);
				if (pTarget != nullptr)
				{
					return pTarget;
				}
			}
			return nullptr;
		}

		bool EquipItem(ItemInstance* inst);
		void UnequipItem(EquipSlot eSlot);

		int GetMoveRange() const { return m_iMoveRange; }
		int GetSightRange() const { return m_iSightRange; }
		void SetMovePath(const std::vector<Vector2>& path) { m_vecMovePath = path; }
		std::vector<Vector2>& GetMovePath() { return m_vecMovePath; }

	protected:
		bool m_bHasBeganPlay = false; // begin 이벤트 받았는지 여부
		bool m_bIsActive = true; //활성화 여부
		bool m_bDestroyRequested = false; //현재 프레임에 삭제 요청 받았는지

		std::wstring m_strImage; //필드에서 사용될 문자
		Color m_Color = Color::White; // 색상
		int m_iWidth = 0; // 문자열 길이

		//오너쉽 Ownership
		Level* m_Owner = nullptr;

		// 그리기 우선 순위
		int m_SortingOrder = 60;

		Vector2 m_Pos; // 위치

		Team m_eTeam;

		std::vector<Component*> m_vecComponents;

		int m_iMoveRange = 3;
		int m_iSightRange = 5;
		std::vector<Vector2> m_vecMovePath;
	};
}