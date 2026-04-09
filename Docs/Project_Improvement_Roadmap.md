# 2ndConsoleGame 프로젝트 개선 로드맵 (Project Improvement Roadmap)

본 문서는 **2ndConsoleGame** 프로젝트의 현재 구조를 분석한 결과를 바탕으로, 기술적 부채를 해결하고 유지보수성 및 확장성을 높이기 위한 단계별 개선 방안을 제시합니다.

---

## 🏁 현재 상태 요약 (Current Status)
- **강점:** 엔진-게임 분리, 컴포넌트 기반 설계, 데이터 주도(JSON) 방식, 효율적인 콘솔 렌더링 시스템.
- **약점:** 메모리 관리 방식의 혼재(`new/delete` vs `smart pointer`), 액터와 레벨 간의 강한 결합, `BattleLevel` 클래스의 거대화(God Class 위험).

---

## 🛠 단계별 개선 과제 (Phases)

### **Phase 1: 메모리 안전성 및 기초 강화 (Memory & Foundation)**
*목표: 수동 메모리 관리를 제거하여 메모리 누수를 원천 차단하고 소유권을 명확히 함*

1.  **스마트 포인터 전면 도입:**
    - `Actor` 내부의 `std::vector<Component*>`를 `std::unique_ptr<Component>`로 전환.
    - `Engine`, `DataManager`, `Level` 등의 멤버 변수에서 `SafeDelete`를 제거하고 스마트 포인터 사용.
2.  **DataManager 안정화:**
    - `m_ItemTable` 등 포인터를 소유하는 맵 구조의 소멸자 처리 및 스마트 포인터 전환.
3.  **RTTI 최적화:**
    - 현재의 `dynamic_cast` 기반 컴포넌트 탐색을 보조할 수 있는 타입 ID 시스템 또는 캐싱 도입 고려.

### **Phase 2: 아키텍처 결합도 해소 (Decoupling & Flexibility)**
*목표: 객체 간의 의존성을 줄여 레벨이나 액터 추가 시 기존 코드 수정을 최소화함*

1.  **입력 제어 구조 개선 (Controller Pattern):**
    - `Player::Tick`에서 레벨 타입을 직접 체크(`IsTypeOf<BattleLevel>`)하는 로직 제거.
    - `InputComponent` 또는 `Controller` 클래스를 도입하여 레벨이 액터의 입력 권한을 제어하도록 변경(제어 역전).
2.  **UI 시스템 추상화:**
    - `BattleLevel`이 UI 객체들을 직접 소유하고 설정하는 대신, `UIManager`를 통해 메시지나 상태 변화를 전달하는 구조로 변경.
3.  **이벤트 시스템 (Observer Pattern) 도입:**
    - 전투 시 발생하는 효과(데미지, 상태 이상)를 `BattleEvent`로 정의하고, UI나 연출 시스템이 이를 구독하여 처리하도록 분리.

### **Phase 3: 전투 시스템 고도화 (Refactoring Battle System)**
*목표: 복잡한 전투 로직을 독립적인 모듈로 분리하여 관리 효율성을 높임*

1.  **State 패턴 적용:**
    - `BattleLevel`의 거대한 `switch-case` 문을 독립적인 `BattleState` 객체(Init, Command, Animation 등)로 분할.
2.  **명령(Command) 시스템 확장:**
    - `IBattleCommand`의 실행 결과와 연출(Cutscene) 간의 연결 고리를 더 정형화하여 새로운 스킬 추가 시 연출 구현을 용이하게 함.
3.  **전투 로그 시스템 최적화:**
    - 로그 출력 방식과 UI 갱신 주기를 분리하여 연출 속도를 조절할 수 있는 기능 추가.

### **Phase 4: 생산성 및 사용자 경험 (Productivity & UX)**
*목표: 개발 편의 기능을 추가하고 최종 폴리싱을 진행함*

1.  **데이터 핫 리로딩 (Hot Reloading):**
    - 게임 실행 중 JSON 파일 수정 시 특정 키(예: F5)를 통해 데이터를 즉시 다시 로드하는 기능.
2.  **렌더링 엔진 고도화:**
    - `ScreenBuffer`의 불필요한 `Clear()` 호출 최적화 및 컬러 ANSI 코드 활용 범위 확대.
3.  **예외 처리 및 로깅:**
    - 치명적인 오류 발생 시 파일로 로그를 남기는 엔진 레벨의 `Logger` 강화.

---

## 📈 기대 효과 (Expected Benefits)
- **안정성:** 메모리 누수 및 잘못된 포인터 접근으로 인한 크래시 방지.
- **확장성:** 새로운 캐릭터, 스킬, 레벨 타입을 추가할 때 기존 핵심 코드를 건드리지 않고 컴포넌트 추가만으로 구현 가능.
- **가독성:** 각 클래스의 책임이 명확해져 협업 및 사후 유지보수 비용 감소.

---
*작성일: 2026-04-09*
*작성자: Gemini CLI 분석 시스템*
