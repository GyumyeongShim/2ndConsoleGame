#pragma once
#include "Common/Common.h"

namespace Wannabe
{
	class WANNABE_API Timer
	{
	public:
		Timer(float targerTime = 1.0f);

		void Tick(float fDeltaTime); // 타이머 업데이트 함수
		void Reset(); //경과시간 리셋 함수

		bool IsTimeOut() const; // 설정 시간값 체크

		void SetTargerTime(float newTargetTime); //목표 시간 설정 함수

	private:
		float m_fElapedTime = 0.0f; // 경과 시간
		float m_fTargetTime = 0.0f; // 목표 시간
	};
}

