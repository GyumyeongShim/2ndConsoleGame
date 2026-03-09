#pragma once
#include <string>
#include <vector>

#include "Core/Common.h"
#include "Component/Component.h"
#include "../Game/Data/MonsterData.h"

enum class EAniType;

namespace Wannabe
{
	class WANNABE_API DisplayComponent : public Component//화면상에 이름, Ascii를 출력처리용 컴포넌트
	{
	public:
		DisplayComponent* Clone() const { return new DisplayComponent(*this); } //깊은 복사
		void SetDisplayByData(CharacterVisualData data);

		void SetNameColor(Color color);
		void SetAsciiColor(Color color);
		void SetName(std::wstring wstrName);
		void SetAscii(std::vector<std::wstring> vecAscii);
		void SetAnimations(const std::map<EAniType, std::vector<std::vector<std::wstring>>>& animations);

		Color GetNameColor() const { return m_NameColor; }
		Color GetAsciiColor() const { return m_AsciiColor; }
		std::wstring GetName() const { return m_wstrName; }
		std::wstring GetOriginName() const { return m_wstrOriginName; }
		const std::vector<std::wstring>& GetAscii() const { return m_vecAscii; }
		const std::vector<std::vector<std::wstring>>* GetAnimationFrames(EAniType type) const;

	private:
		Color m_AsciiColor = Color::White; // Ascii색상
		Color m_NameColor = Color::White; // 타겟 지정된 경우 색상
		std::wstring m_wstrName; //이름
		std::wstring m_wstrOriginName; //원본 이름
		std::vector<std::wstring> m_vecAscii; // 이미지 대신 Ascii로 표현하기 위한 벡터

		std::map<EAniType, std::vector<std::vector<std::wstring>>> m_mapAnimations; //ascii ani
	};
}
