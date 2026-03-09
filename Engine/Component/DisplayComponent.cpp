#include "DisplayComponent.h"
#include "../Game/Data/ActionData.h"

using namespace Wannabe;

void DisplayComponent::SetDisplayByData(CharacterVisualData data)
{
	m_vecAscii = data.ascii;
	m_wstrName = data.name;
	m_wstrOriginName = data.name;
	m_AsciiColor = data.color;
	m_mapAnimations = data.animations;
}

void Wannabe::DisplayComponent::SetNameColor(Color color)
{
	m_NameColor = color;
}

void Wannabe::DisplayComponent::SetAsciiColor(Color color)
{
	m_AsciiColor = color;
}

void DisplayComponent::SetName(std::wstring wstrName)
{
	m_wstrName = wstrName;
}

void DisplayComponent::SetAscii(std::vector<std::wstring> vecAscii)
{
	m_vecAscii = vecAscii;
}

const std::vector<std::vector<std::wstring>>* DisplayComponent::GetAnimationFrames(EAniType type) const
{
	auto it = m_mapAnimations.find(type);
	if (it != m_mapAnimations.end())
		return &it->second;

	return nullptr;
}