#pragma once
#include <vector>

#include "Core/RTTI.h"
#include "Math/Vector2.h"
#include "Actor/Actor.h"
#include "Render/Camera.h"
#include "Render/RenderSystem.h"
#include "Render/Canvas.h"
#include "../Game/Data/RunGameData.h"

namespace Wannabe
{
	// 레벨에 있는 모든 액터 관리
	class WANNABE_API Level : public RTTI
	{
		RTTI_DECLARATIONS(Level, RTTI);

	public:
		Level();
		virtual ~Level();

		//게임 플레이 이벤트
		virtual void BeginPlay();
		virtual void Tick(float fDeltaTime);
		virtual void Draw(Wannabe::RenderSystem& renderSys);

		virtual void ConfigureRenderer(RenderSystem& renderer) const = 0; // actor draw mode 지정
		virtual void OnEnterLevel(struct RunGameData* pData) {};
		virtual void OnExitLevel(struct RunGameData* pData) {};
		virtual bool CanMove(const Vector2& nextPos) { return true; }
		virtual void CheckPortal() {};

		//액터 추가 함수
		void AddNewActor(Actor* newActor); //전방선언을 여기서 해버림

		//액터 배열 접근자
		void ProcessAddAndDestroyActors();

		Camera* GetCamera() const { return m_pCamera; }

	protected:
		bool m_bBegunPlay = false;
		Camera* m_pCamera;
		std::vector<Actor*> m_vecActors; //액터 배열
		std::vector<Actor*> m_vecAddRequestedActors; //실행 중 추가 요청된 액터의 배열
	};
}