#pragma once
#include <string>

#include "Core/RTTI.h" 
#include "Math/Vector2.h"
#include "Math/Color.h"

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

	class StatComponent;
	class StatusComponent;
	class InventoryComponent;
	class EquipmentComponent;
	class DisplayComponent;
	class SkillComponent;

	class ItemInstance;

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

		// 삭제 요청 함수
		void Destroy();

		// 삭제가 될 때 호출될 이벤트 함수
		virtual void OnDestroy();
		
		// 충돌 여부 확인 함수
		bool TestIntersect(const Actor* const other);

		// 액터의 이미지 값 변경 함수
		void ChangeImage(const wchar_t* newImage);

		// 위치 변경 및 읽기 함수
		void SetPosition(const Vector2& pos);
		inline const Vector2& GetPosition() const { return m_Pos; }

		//오너쉽 추가읽기 함수
		void SetOwner(Level* newlevel) { m_Owner = newlevel; }
		inline Level* GetOwner() const { return m_Owner; }

		// 해당 엑터가 갖고 있는 컴포넌트
		StatComponent* GetStat() const { return m_pStatComponent; }
		StatusComponent* GetStatus() const { return m_pStatusComponent; }
		EquipmentComponent* GetEquip() const { return m_pEquipmentComponent; }
		InventoryComponent* GetInven() const { return m_pInventoryComponent; }
		DisplayComponent* GetDisplay() const { return m_pDisplayComponent; }
		SkillComponent* GetSKill() const { return m_pSkillComponent; }

		//Getter
		Team GetTeam() const { return m_eTeam; }
		bool HasBeganPlay() const { return m_bHasBeganPlay; }
		bool IsActive() const { return m_bIsActive && !m_bDestroyRequested; }
		bool IsDestroyRequested() const { return m_bDestroyRequested; }
		int GetSortingOrder() const { return m_SortingOrder; }
		int GetWidth() const { return m_iWidth; }
		wchar_t GetGlyph() const { return *m_strImage.c_str(); }
		Color GetGlyphColor() const { return m_Color; }

		bool EquipItem(ItemInstance* inst);
		void UnequipItem(EquipSlot eSlot);

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
		int m_SortingOrder = 0;

		Vector2 m_Pos; // 위치

		Team m_eTeam;

		StatComponent* m_pStatComponent = nullptr; // 데이터 값
		StatusComponent* m_pStatusComponent = nullptr; //상태이상
		EquipmentComponent* m_pEquipmentComponent = nullptr; //장비 장착
		InventoryComponent* m_pInventoryComponent = nullptr; //아이템 소비
		DisplayComponent* m_pDisplayComponent = nullptr; //화면에 그리는 정보 (이름, Ascii 등)
		SkillComponent* m_pSkillComponent = nullptr; // 스킬
	};
}