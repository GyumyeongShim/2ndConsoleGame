#include "SkillComponent.h"

#include "Manager/DataManager.h"

void Wannabe::SkillComponent::Update(float fDeltaTime)
{
    return;
}

void Wannabe::SkillComponent::AddSkill(int iSkillTID)
{
    if (HasSkill(iSkillTID))
        return;

    m_vecSkillTID.emplace_back(iSkillTID);
}

void Wannabe::SkillComponent::RemoveSkill(int iSkillTID)
{
    auto it = std::remove(m_vecSkillTID.begin(), m_vecSkillTID.end(), iSkillTID);

    m_vecSkillTID.erase(it, m_vecSkillTID.end());
}

bool Wannabe::SkillComponent::HasSkill(int iSkillTID) const
{
    bool bHasSkill = std::find(m_vecSkillTID.begin(), m_vecSkillTID.end(), iSkillTID) != m_vecSkillTID.end();
    return bHasSkill;
}

void Wannabe::SkillComponent::Clear()
{
    m_vecSkillTID.clear();
}