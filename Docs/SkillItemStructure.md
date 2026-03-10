# 스킬/아이템 생성 및 사용 구조 (Skill & Item Structure)

2ndConsoleGame은 스킬과 아이템을 공통된 데이터 구조인 `ActionData`를 통해 통합 관리하며, 이를 액터의 컴포넌트 시스템을 통해 사용합니다.

## 1. 데이터 정의 (Data Definition)

### 1.1 `ActionData`
스킬과 아이템의 공통 속성을 정의하는 구조체입니다.
- **TID**: 고유 식별자.
- **Name**: 스킬/아이템 이름.
- **ItemType**: 소모품, 장비, 스킬 등의 분류.
- **CombatEffectData**: 발동 시 발생하는 효과들의 리스트(대미지, 힐, 상태 이상 등).
- **StatModifier**: 아이템(주로 장비) 착용 시 부여되는 스탯 보정치.

## 2. 생성 및 보유 (Creation & Storage)

### 2.1 `DataManager`
- 게임 시작 시 `ActionData` 리스트를 JSON 등에서 로드하여 캐싱합니다 (`m_ActionTable`).

### 2.2 보유 컴포넌트
- **SkillComponent**: 액터가 보유한 스킬의 TID 리스트를 관리합니다 (`AddSkill`, `RemoveSkill`).
- **InventoryComponent**: 액터가 소유한 아이템 인스턴스(`ItemInstance`) 리스트를 관리합니다 (`AddItem`, `UseItem`, `RemoveItem`).

## 3. 사용 로직 (Usage Logic)

### 3.1 명령 선택 (Command Select)
- `BattleLevel`에서 플레이어의 조작을 통해 스킬 또는 아이템을 선택합니다.
- 해당 명령(`SkillCommand`, `ItemCommand`)이 생성되고, 대상(Target) 정보를 담아 큐에 추가됩니다.

### 3.2 효과 해결 (Resolve Action)
- `BattleResolver::ResolveAction()`이 선택된 `ActionData`를 분석합니다.
- `ActionData` 내의 `vecEffects`를 순회하며 각 효과를 계산합니다.
- **대미지 스킬**: `CalcSkillDamage()` 호출.
- **회복 스킬**: `CalcSkillHeal()` 호출.
- **아이템**: `ItemType`에 따라 소모(Consume) 또는 장착(Equip) 처리를 수행합니다.

---
## 스킬/아이템 사용 흐름

```mermaid
graph LR
    User[Actor] --> UI[Command UI]
    UI -- Select -- CMD[IBattleCommand]
    CMD -- Execute -- Resolver[BattleResolver]
    
    subgraph Data
        DM[DataManager] --> AD[ActionData]
    end
    
    Resolver -- Fetch Data -- AD
    AD -- Resolve Effects -- Effects[Combat Effects]
    Effects -- Apply -- Target[Target Actor]
```
