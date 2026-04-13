# 2ndConsoleGame 프로젝트 개선 로드맵 (Project Improvement Roadmap) - Revised

본 문서는 **2ndConsoleGame** 프로젝트의 현재 구조를 분석한 결과를 바탕으로, 기술적 부채를 해결하고 유지보수성 및 확장성을 높이기 위한 단계별 개선 방안을 제시합니다.

---

## 🏁 현재 상태 요약 (Current Status)
- **강점:** 엔진-게임 분리, 컴포넌트 기반 설계, 데이터 주도(JSON) 방식, 효율적인 콘솔 렌더링 시스템.
- **약점:** 메모리 관리 방식의 혼재, 액터와 레벨 간의 강한 결합, `BattleLevel` 및 `Actor` 클래스의 비대화(Fat Class 위험), RTTI 탐색 비용.

---

## 🛠 단계별 개선 과제 (Phases)

### **Phase 1: 메모리 안전성 및 기초 강화 (Memory & Foundation)**
*목표: 수동 메모리 관리를 제거하고, 데이터와 상태를 분리하여 시스템의 안정성을 확보함*

1.  **스마트 포인터 및 소유권 명확화:**
    - `Actor` 내부의 `std::vector<Component*>`를 `std::unique_ptr<Component>`로 전환.
    - `Engine`, `DataManager`, `Level` 등의 멤버 변수에서 `SafeDelete`를 제거하고 스마트 포인터 사용.
2.  **RTTI 및 컴포넌트 탐색 최적화:**
    - 현재의 `dynamic_cast` 기반 순회 방식을 개선하기 위해 각 컴포넌트에 고유 `TypeID` 부여.
    - 컴포넌트 추가 시 `unordered_map` 등을 활용한 캐싱 시스템 도입으로 탐색 속도 향상.
3.  **데이터-인스턴스 분리 (StatComponent):**
    - `StatComponent` 내의 원본 데이터(StatData)와 현재 변동 수치(Instance State)를 엄격히 분리.
    - 버프/디버프 계산 시 항상 원본 데이터를 참조하여 수치 누적 오류 방지.
4.  **컴포넌트 생명주기 세분화:**
    - `OnEnable`, `OnDisable`, `OnComponentAdded` 등의 훅을 추가하여 컴포넌트 간 의존성 해결 타이밍 명확화.

### **Phase 2: 아키텍처 결합도 해소 (Decoupling & Flexibility)**
*목표: 객체 간의 의존성을 줄이고 기능을 모듈화하여 확장성을 높임*

1.  **액터 경량화 (Lean Actor):**
    - `Actor` 클래스에 포함된 팀 구분, 아이템 장착, 경로 탐색 로직을 전용 컴포넌트로 이관.
    - 엔진 레벨의 `Actor`는 최소한의 변환(Transform)과 컴포넌트 컨테이너 역할에 집중.
2.  **입력 제어 및 커맨드 패턴:**
    - 폴링 방식의 입력을 큐(Queue)에 쌓아 처리하는 입력 버퍼링 시스템 도입.
    - 입력을 `Command` 객체로 변환하여 `Controller`를 통해 액터에 전달(제어 역전).
3.  **타입 안전한 이벤트 시스템 (Event Bus):**
    - 단순 `Observer` 패턴을 넘어, 템플릿 기반의 `EventBus`를 도입하여 컴파일 타임에 타입 체크가 가능한 통신 구조 구축.
4.  **UI 시스템 추상화:**
    - `BattleLevel`이 UI 객체를 직접 제어하는 대신, `UIManager`를 통해 상태 변화를 브로드캐스팅하는 구조로 변경.

### **Phase 3: 전투 시스템 고도화 (Refactoring Battle System)**
*목표: 복잡한 전투 로직을 독립적인 모듈로 분리하여 관리 효율성을 높임*

1.  **State 패턴 적용:**
    - `BattleLevel`의 거대한 `switch-case` 문을 독립적인 `BattleState` 객체(Init, Command, Animation 등)로 분할.
2.  **전투 로그 및 연출 분리:**
    - 로그 출력과 실제 UI 애니메이션 주기를 분리하여 연출 속도를 자유롭게 조절할 수 있는 기능 추가.
3.  **AI 로직 모듈화:**
    - `BattleLevel` 내부에 하드코딩된 적 AI 로직을 `AIComponent` 또는 외부 스크립트로 분리.

### **Phase 4: 생산성 및 사용자 경험 (Productivity & UX)**
*목표: 개발 편의 기능을 추가하고 렌더링 성능을 최적화함*

1.  **렌더링 엔진 최적화:**
    - 매 프레임 전체 화면을 갱신하는 대신, 변경된 부분만 다시 그리는 `Dirty Rect` 기법 검토.
    - 문자열 빈번 생성 방지를 위한 내부 고정 버퍼 활용 및 ANSI 컬러 코드 최적화.
2.  **데이터 핫 리로딩 (Hot Reloading):**
    - 게임 실행 중 JSON 파일 수정 시 특정 키(예: F5)를 통해 데이터를 즉시 다시 로드하는 기능.
3.  **예외 처리 및 통합 로깅:**
    - 크래시 발생 시 상세 호출 스택을 포함한 파일 로그를 남기는 엔진 레벨의 `Logger` 강화.

---

## 📈 기대 효과 (Expected Benefits)
- **안정성:** 메모리 누수 방지 및 데이터 무결성 확보.
- **확장성:** 새로운 기능 추가 시 기존 코드 수정 없이 컴포넌트/상태 추가만으로 구현 가능.
- **성능:** 컴포넌트 탐색 및 렌더링 부하 감소로 쾌적한 플레이 환경 제공.

---
*최종 수정일: 2026-04-13*
*작성자: Gemini CLI 분석 시스템*
