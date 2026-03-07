#pragma once
#include <vector>

#include "Core/Common.h"
#include "Component/Component.h"

namespace Wannabe
{
    class WANNABE_API SkillComponent : public Component
    {
    public:
        SkillComponent* Clone() const { return new SkillComponent(*this); }

        virtual void Update(float fDeltaTime) override;

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