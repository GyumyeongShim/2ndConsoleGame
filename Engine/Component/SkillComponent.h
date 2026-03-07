#pragma once
#include <vector>

#include "Core/Common.h"

namespace Wannabe
{
    class WANNABE_API SkillComponent
    {
    public:
        SkillComponent* Clone() const { return new SkillComponent(*this); }
        void Update(float fDeltaTime);

        void AddSkill(int iSkillTID);
        void RemoveSkill(int iSkillTID);

        bool HasSkill(int iSkillTID) const;

        const std::vector<int>& GetSkillList() const { return m_vecSkillTID; }
        int GetSkillCount() const { return (int)m_vecSkillTID.size(); }

        void Clear();

    private:
        std::vector<int> m_vecSkillTID;
    };
}