#include <vector>
#include <memory>

#include "EnemyAI.h"
#include "Manager/DataManager.h"
#include "Util/Utill.h"

#include "Actor/BattleActor.h"

#include "Battle/Command/AtkCommand.h"
#include "Battle/Command/SkillCommand.h"
#include "Battle/Command/RunCommand.h"

#include "Component/StatComponent.h"
#include "Component/SkillComponent.h"

std::unique_ptr<Wannabe::IBattleCommand> EnemyAI::Decide(Wannabe::Actor* self, const std::vector<Wannabe::Actor*>& players)
{
    if (self == nullptr || players.empty())
        return nullptr;

    // 1. 도망 판단
    bool bIsRun = ShouldRun(self);
    if(bIsRun) 
        return std::make_unique<RunCommand>(self);

    // 2. 타겟 선정
    Wannabe::Actor* target = SelectTarget_Hp(players);

    // 3. 스킬 사용 판단
    if (ShouldUseSkill(self) == true)
    {
        // 4. 스킬 공격
        int skillTid = SelectUsableSkill(self);
        if(skillTid != -1)
            return std::make_unique<SkillCommand>(self, target, skillTid);
    }

    //5. 기본 공격
    return std::make_unique<AtkCommand>(self, target);
}

Wannabe::Actor* EnemyAI::SelectTarget_Hp(const std::vector<Wannabe::Actor*>& players)
{
    Wannabe::Actor* pTarget = nullptr;

    for (Wannabe::Actor* p : players)
    {
        if (p == nullptr || p->IsDestroyRequested())
            continue;

        if (pTarget == nullptr)
        {
            pTarget = p;
            continue;
        }

        if (p->GetComponent<StatComponent>()->GetHp() < pTarget->GetComponent<StatComponent>()->GetHp())
            pTarget = p;
    }

    return pTarget;
}

Wannabe::Actor* EnemyAI::SelectTarget_Random(const std::vector<Wannabe::Actor*>& players)
{
    int idx = Util::Random(0, static_cast<int>(players.size()) - 1);
    return players[idx];
}

bool EnemyAI::ShouldUseSkill(Wannabe::Actor* self)
{
    // 체력 50% 이하일 때 30% 확률로 스킬 사용
    int hp = self->GetComponent<StatComponent>()->GetHp();
    int maxHp = self->GetComponent<StatComponent>()->GetMaxHp();

    if (hp < maxHp / 2)
    {
        int rand = Util::Random(0, 100);
        return rand < 30;
    }

    return false;
}

bool EnemyAI::ShouldRun(Wannabe::Actor* self)
{
    //// 체력 20% 이하 → 도망 50% 확률
    //int hp = self->GetStat()->GetHp();
    //int maxHp = self->GetStat()->GetMaxHp();

    //if (hp < maxHp / 5)
    //{
    //    int rand = Util::Random(0, 100);
    //    return rand < 50;
    //}

    return false;
}

int EnemyAI::SelectUsableSkill(Wannabe::Actor* self)
{
    const SkillComponent* skillCompo= self->GetComponent<SkillComponent>();
    if (skillCompo == nullptr)
        return -1;

    std::vector<int> skills = skillCompo->GetSkillList();

    std::vector<int> usableSkills;
    for (int tid : skills)
    {
        const ActionData* data = DataManager::Get().GetActionData(tid);
        if (data == nullptr)
            continue;

        if (data->eItemType != ItemType::None)
            continue;

        // 조건 목록

        usableSkills.push_back(tid); //todo 임시로 다 추가
    }

    if (usableSkills.empty() == true)
        return -1;

    int idx = Util::Random(0, static_cast<int>(usableSkills.size()) - 1);
    return usableSkills[idx];
}
