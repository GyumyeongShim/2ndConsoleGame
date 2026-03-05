#include "ItemCommand.h"
#include "Enum/ItemType.h"
#include "Actor/Actor.h"
#include "Item/Item.h"
#include "Item/ItemInstance.h"
#include "Battle/BattleContext.h"
#include "Battle/BattleResolver.h"
#include "Battle/BattleEventProcessor.h"
#include "Battle/System/CutScenePlayer.h"
#include "Battle/Cutscene/LogEvent.h"
#include "Battle/Cutscene/DamageEvent.h"
#include "Battle/Cutscene/DelayEvent.h"
#include "Battle/Cutscene/DeathEvent.h"
#include "Component/InventoryComponent.h"
#include "Component/DisplayComponent.h"
#include "Manager/DataManager.h"
#include "Data/ActionData.h"

ItemCommand::ItemCommand(Wannabe::Actor* pIntsigator, Wannabe::Actor* pTarget, int iItemTID)
	:m_pInstigator(pIntsigator), m_pTarget(pTarget), m_iItemTID(iItemTID)
{
    m_eType = CommandType::Item;
}

void ItemCommand::Execute(Wannabe::BattleContext& context)
{
    if (context.IsValidActor(m_pTarget) == false || context.IsValidActor(m_pInstigator) == false || m_iItemTID == 0)
        return;

    auto& resolver = context.GetResolver();
    auto& processor = context.GetEventProcessor();
    ActCheckResult act = resolver.CanAct(m_pInstigator);
    if (!act.bCanAct)
    {
        BattleLog log;
        log.eLogType = LogType::Free;
        log.wstrTxt = act.wstrReason;
        context.GetCutscenePlayer().Push(
            std::make_unique<LogEvent>(log));
        return;
    }

    // 인벤토리에서 ItemInstance 가져오기
    auto* inventory = m_pInstigator->GetInven();
    if (inventory == nullptr)
        return;

    Wannabe::ItemInstance* itemInstance = inventory->FindItem(m_iItemTID);
    if (itemInstance == nullptr)
        return;

    // Item data
    const ActionData* itemData = Wannabe::DataManager::Get().GetActionData(itemInstance->GetItem()->GetItemTID());
    if (itemData == nullptr)
        return;

    // 타겟 선정
    auto targets = context.ResolveTargets(m_pInstigator, m_pTarget, itemData->targetType, itemData->iMaxTargetCnt);
    switch (itemData->targetType)
    {
    case ActionTargetType::Self:
        targets.push_back(m_pInstigator);
        break;

    case ActionTargetType::SingleEnemy:
        targets.push_back(m_pTarget);
        break;

    case ActionTargetType::AllEnemy:
        targets = context.IsEnemy(m_pInstigator) ?
            context.GetEnemyParty(m_pInstigator) : context.GetPlayerParty(m_pInstigator);
        break;

    case ActionTargetType::AllAlly:
        targets = context.IsEnemy(m_pInstigator) ?
            context.GetPlayerParty(m_pInstigator) : context.GetEnemyParty(m_pInstigator);
        break;

    case ActionTargetType::SingleAlly:
        targets.push_back(m_pTarget);
        break;

    case ActionTargetType::RandomEnemy:
        break;
    }
}