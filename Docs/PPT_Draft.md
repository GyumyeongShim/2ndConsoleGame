# [PPT 초안] 2ndConsoleGame 시스템 아키텍처 분석

이 문서는 프로젝트의 핵심 시스템을 PPT 발표 자료로 구성하기 위한 초안입니다. 각 슬라이드의 구성 요소와 발표 스크립트 가이드를 포함하고 있습니다.

---

## 📋 슬라이드 구성안

### Slide 1: 타이틀 (Title)
*   **제목:** 2ndConsoleGame: 컴포넌트 기반 콘솔 RPG 엔진 및 시스템 분석
*   **부제:** 엔진 아키텍처, 전투 로직 및 액터 라이프사이클 중심
*   **발표자:** [사용자 이름]
*   **날짜:** 2026. 03. 10.

---

### Slide 2: 엔진 구조 (Engine Structure)
*   **핵심 키워드:** Wannabe Engine, Component-Actor Pattern, Level Management
*   **주요 내용:**
    *   **Engine Core:** `Wannabe::Engine` 클래스를 통한 중앙 집중형 게임 루프(Tick/Draw) 관리.
    *   **Level System:** 타이틀, 필드, 전투 등 각 독립된 게임 상태를 `Level` 단위로 분리 및 전환.
    *   **Actor-Component:** 유연한 기능 확장을 위해 데이터(Actor)와 로직(Component)을 분리한 설계.
*   **시각화 요소:** `Engine` -> `Level` -> `Actor` -> `Components` 계층 구조도.
*   **발표 포인트:** "우리 엔진은 컴포넌트 패턴을 채택하여, 새로운 기능을 추가할 때 액터 클래스를 수정하지 않고 컴포넌트만 부착하면 되는 높은 확장성을 갖추고 있습니다."

---

### Slide 3: 전투 시스템 아키텍처 (Battle Structure)
*   **핵심 키워드:** BattleContext, Resolver, TurnManager, Phase Machine
*   **주요 내용:**
    *   **BattleContext:** 전투 내 모든 데이터(Log, Resolver, Turn)를 보유한 중앙 저장소.
    *   **BattleResolver:** 수치 계산(대미지, 크리티컬, 상태 이상)의 핵심 엔진.
    *   **TurnManager:** 캐릭터 속도 기반의 동적 턴 큐(Queue) 관리.
    *   **Phases:** `Command -> Target -> Animation -> Log` 순의 상태 머신 기반 흐름.
*   **시각화 요소:** 전투 턴 진행 시퀀스 다이어그램 (Player/Enemy Turn 분기 처리).
*   **발표 포인트:** "전투 시스템은 `BattleLevel`의 상태 머신에 의해 제어됩니다. 특히 `BattleResolver`를 통해 모든 대미지 계산 로직을 캡슐화하여 유지보수성을 극대화했습니다."

---

### Slide 4: 필드 및 탐험 구조 (Field Structure)
*   **핵심 키워드:** TileMap, FieldState, Encounter System
*   **주요 내용:**
    *   **TileMap System:** 타일 단위 충돌 및 이동 범위 계산(`CanMove`, `CalcMoveRange`).
    *   **Field Phases:** `Idle -> Player Move -> Enemy Move`의 순환 구조.
    *   **Encounter:** 필드 몬스터 조우 및 랜덤 인카운터를 통한 `BattleLevel` 전환 로직.
*   **시각화 요소:** 필드 이동 상태와 전투 전환 시점의 흐름도(Flowchart).
*   **발표 포인트:** "필드 시스템은 플레이어와 몬스터의 턴을 번갈아 처리하며, 전투 조건 충족 시 부드럽게 데이터를 전달하며 레벨을 전환합니다."

---

### Slide 5: 전투 액터 생명주기 (Actor Lifecycle)
*   **핵심 키워드:** Setup, Tick/Draw, Pending Destroy, Cleanup
*   **주요 내용:**
    *   **Spawn:** 전투 시작 시 `SetupBattle`을 통해 파티 액터들을 동적 생성 및 등록.
    *   **Runtime:** 매 프레임 `Tick`과 `Draw`를 통해 상태 갱신 및 콘솔 렌더링.
    *   **Destruction:** 체력 고갈 시 비활성화(`m_bIsActive = false`) 후 `CleanupDeacActor`에서 안전한 메모리 해제.
*   **시각화 요소:** 액터 상태 전이도 (State Diagram: Born -> Active -> Inactive -> Dead).
*   **발표 포인트:** "전투 중 발생하는 액터의 파괴는 즉시 수행되지 않고, 렌더링 루프의 안전을 위해 '파괴 예약' 단계를 거쳐 프레임 끝에서 안전하게 정리됩니다."

---

### Slide 6: 스킬 및 아이템 시스템 (Skill & Item)
*   **핵심 키워드:** ActionData, DataManager, CombatEffect
*   **주요 내용:**
    *   **Unified Data:** 스킬과 아이템을 `ActionData` 구조체로 통합 관리하여 유연성 확보.
    *   **DataManager:** 외부 JSON 데이터를 로드하여 테이블 형태로 캐싱 및 제공.
    *   **Component Logic:** `SkillComponent`는 TID를 보유하고, `Inventory`는 `ItemInstance`를 관리하여 실제 효과 발동.
*   **시각화 요소:** `ActionData` 구조 내 `CombatEffectData` 리스트 구조 시각화.
*   **발표 포인트:** "스킬과 아이템은 동일한 데이터 구조를 공유합니다. 이는 개발자가 코드 수정 없이 데이터 편집만으로 새로운 콘텐츠를 추가할 수 있게 합니다."

---

### Slide 7: 결론 및 Q&A
*   **요약:**
    *   유연한 컴포넌트 기반 아키텍처 구축.
    *   데이터 중심(Data-Driven)의 스킬/아이템 설계.
    *   견고한 상태 머신 기반의 전투 및 필드 전환 시스템.
*   **질의응답:** 질의응답 및 마무리.

---

## 💡 PPT 제작 팁
1.  **다이어그램 활용:** `EngineStructure.md` 등 다른 문서에 포함된 Mermaid 다이어그램 이미지를 활용하세요.
2.  **코드 스니펫:** 핵심 로직(예: `ActionData` 구조체 정의)을 일부 삽입하면 기술적 전문성이 강조됩니다.
3.  **스크린샷:** 현재 게임의 필드 화면과 전투 화면 스크린샷을 예시 이미지로 활용하세요.
