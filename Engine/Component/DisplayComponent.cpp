#include "DisplayComponent.h"

using namespace Wannabe;

void DisplayComponent::SetDisplayByData(CharacterVisualData data)
{
	m_vecAscii = data.ascii;
	m_wstrName = data.name;
	m_wstrOriginName = data.name;
	m_AsciiColor = data.color;
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