# 필드 구조 (Field Structure)

필드(Field)는 플레이어가 월드 맵을 이동하며 탐험하고, 이벤트나 몬스터 조우를 경험하는 공간입니다. `MainLevel`, `TownLevel` 등이 이에 해당합니다.

## 1. 필드 시스템 아키텍처

### 1.1 월드 맵 및 타일 (TileMap)
- **TileMap**: 필드의 배경과 충돌 정보를 담고 있는 데이터 구조입니다.
- **Tile**: 각 타일은 속성(이동 가능 여부 등)을 가집니다.
- `CanMove()`: 액터가 특정 타일로 이동 가능한지 판별합니다.

### 1.2 필드 상태 머신 (FieldState)
필드 레벨은 다음과 같은 상태들을 순환합니다:
- **Phase_Idle**: 플레이어 입력 대기 상태.
- **Phase_PlayerTurn**: 플레이어가 이동할 수 있는 턴.
- **Phase_Move**: 캐릭터가 실제 타일 위를 이동하는 도중의 연출.
- **Phase_EnemyTurn / Phase_EnemyMove**: 몬스터 등의 NPC 액터들이 이동하는 단계.
- **Phase_Transition**: 전투 전환 시의 페이드인/아웃 연출.

## 2. 주요 기능

### 2.1 이동 및 경로 탐색
- **이동 범위 계산**: 캐릭터의 이동 수치에 따라 도달 가능한 타일을 하이라이트합니다 (`CalcMoveRange()`).
- **커서 및 경로**: 사용자가 선택한 목적지까지의 경로를 실시간으로 표시합니다.

### 2.2 인카운터 (Encounter)
- **몬스터 조우**: 필드 위의 몬스터 액터와 겹칠 때 발생합니다 (`CheckMonsterEncounter()`).
- **랜덤 인카운터**: 이동 시 확률적으로 발생할 수 있습니다 (`CheckRandomEncounter()`).
- **전투 전환**: 전투 조건이 충족되면 `StartBattleTransition()`을 호출하여 `BattleLevel`로 레벨을 전환합니다.

### 2.3 포탈 및 상호작용
- **포탈 (Portal)**: 특정 위치에 도달하면 다른 레벨(예: 마을 -> 던전)로 이동하는 로직을 처리합니다 (`CheckPortal()`).

---
## 필드 이동 및 전투 전환 흐름

```mermaid
graph TD
    Idle[Phase_Idle] --> PlayerTurn[Phase_PlayerTurn]
    PlayerTurn --> Move[Phase_Move]
    Move --> EnemyTurn[Phase_EnemyTurn]
    EnemyTurn --> EnemyMove[Phase_EnemyMove]
    EnemyMove --> Idle

    Move -- Encounter --|Transition| BattleLevel[BattleLevel Setup]
```
