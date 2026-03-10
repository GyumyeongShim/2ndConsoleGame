# BattleLevel Actor Lifecycle

전투 레벨(`BattleLevel`)에서 액터(Actor)가 생성되고, 관리되며, 소멸되는 생명주기(Lifecycle)를 정리합니다.

## 1. 생성 및 초기화 (Spawn & Initialization)

전투가 시작될 때 플레이어 파티와 적 파티의 액터들이 생성됩니다.

### 1.1 `SetupBattle()`
- `vecPlayer`, `vecEnemy`를 인자로 받아 전투 참가를 확정합니다.
- `BattleActor` 등의 클래스로 액터를 생성하고, 필요한 컴포넌트(`Stat`, `Skill`, `Display` 등)를 초기화합니다.
- `Level::AddNewActor()`를 호출하여 레벨에 등록합니다.

### 1.2 `BeginPlay()`
- 액터가 레벨에 추가된 후 처음 한 번 실행되는 이벤트입니다.
- 초기 위치 설정, 초기 렌더링 세팅 등을 수행합니다.

## 2. 업데이트 및 렌더링 (Update & Render)

### 2.1 `Tick(float fDeltaTime)`
- 레벨의 `Tick()` 함수 내에서 소속된 모든 액터의 `Tick()`이 호출됩니다.
- 전투 중 실시간 애니메이션이나 상태 변화를 처리합니다.

### 2.2 `Draw(RenderSystem& renderSys)`
- 액터의 현재 상태(위치, 이미지 등)를 캔버스에 그립니다.
- `DisplayComponent`를 통해 캐릭터의 외형을 렌더링합니다.

## 3. 상태 관리 (Active & Destroy)

### 3.1 `Actor::m_bIsActive`
- 액터의 활성 상태를 나타냅니다. 체력이 0이 되어 패배한 액터는 `m_bIsActive`가 `false`로 설정됩니다.

### 3.2 `OnRemoveActor()` / `RemoveActorCallback()`
- 액터가 파괴되거나 전투에서 이탈할 때 호출됩니다.
- 전투 레벨의 파티 리스트(`m_vecPlayerParty`, `m_vecEnemyParty`)에서 제거됩니다.
- `m_vecPendingDestroy` 리스트에 담겨 안전한 시점에 메모리에서 해제됩니다.

### 3.3 `CleanupDeacActor()`
- 매 틱마다 비활성화된 액터들을 검사하고 정리 로직을 수행합니다.

---
## 액터 생명주기 다이어그램

```mermaid
stateDiagram-v2
    [*] --> Setup : SetupBattle()
    Setup --> BeginPlay : Level::AddNewActor()
    BeginPlay --> Tick : Every Frame
    Tick --> Draw : Every Frame
    
    Tick --> Dead : HP <= 0 (Action Phase)
    Dead --> Inactive : SetActive(false)
    Inactive --> PendingDestroy : RemoveActorCallback()
    PendingDestroy --> Destroyed : CleanupDeacActor() / ~Actor()
    Destroyed --> [*]
```
