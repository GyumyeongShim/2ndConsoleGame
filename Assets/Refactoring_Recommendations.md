# 리팩토링 및 최적화 추천 목록 (Refactoring & Optimization Recommendations)

본 문서는 `1stConsoleGame` 프로젝트의 코드베이스 분석을 바탕으로, 아키텍처의 유연성, 코드 품질, 성능 및 유지보수 효율을 높이기 위한 리팩토링 포인트를 정리한 것입니다.

---

## 1. 아키텍처 리팩토링 (Architectural Refactoring)

### 1.1 Actor-Component 시스템 결합도 해제 (Decoupling)
*   **현황**: `Actor` 클래스가 `Stat`, `Status`, `Equipment` 등 구체적인 컴포넌트 타입을 멤버 변수로 직접 소유하고 있으며, 관련 헤더를 모두 포함(Include)하고 있습니다.
*   **제안**:
    *   `std::unordered_map<size_t, Component*>`를 사용하여 컴포넌트를 관리하는 방식으로 전환.
    *   `GetComponent<T>()` 템플릿 메서드를 도입하여 필요한 컴포넌트를 동적으로 조회하도록 변경.
*   **기대 효과**: `Actor.h`의 헤더 의존성을 획기적으로 줄이고, 새로운 기능을 추가할 때 `Actor` 클래스를 수정하지 않아도 됨 (개방-폐쇄 원칙 준수).

### 1.2 인터페이스 계층 간소화 (Simplification)
*   **현황**: `IBattleCommand`, `IBattleEventFactory`, `IBattleLevel` 등 프로젝트 규모에 비해 인터페이스가 과하게 분화되어 있습니다.
*   **제안**:
    *   실질적으로 다형성이 필요한 부분(예: `IBattleCommand`)만 남기고, 단순히 데이터 전달이나 고정된 로직을 가진 인터페이스는 구체 클래스로 통합.
*   **기대 효과**: 코드 추적(Trace)이 쉬워지고 불필요한 추상화 오버헤드 감소.

---

## 2. 코드 품질 및 안전성 (Code Quality & Safety)

### 2.1 현대적 메모리 관리 (Modern C++ Smart Pointers)
*   **현황**: `SafeDelete` 매크로와 원시 포인터(Raw Pointer)를 사용하여 수동으로 `delete`를 수행하고 있습니다.
*   **제안**:
    *   `Actor`, `Component`, `Item` 등의 수명 주기를 `std::unique_ptr` 또는 `std::shared_ptr`로 관리.
    *   소유권이 없는 참조는 원시 포인터나 `std::weak_ptr` 사용.
*   **기대 효과**: 메모리 누수(Memory Leak) 및 댕글링 포인터(Dangling Pointer) 위험 원천 차단.

### 2.2 JSON 파싱 자동화 및 보일러플레이트 제거
*   **현황**: `DataLoader.cpp`에서 각 데이터 시트마다 수동으로 JSON 값을 대입하는 코드가 반복되고 있습니다.
*   **제안**:
    *   `nlohmann/json`의 `NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE` 매크로를 사용하여 구조체와 JSON 간의 변환을 자동화.
    *   데이터 로딩 로직을 템플릿화하여 `LoadData<T>(path)` 형태로 통일.
*   **기대 효과**: 데이터 추가 시 작성해야 하는 코드 양 80% 이상 절감, 오타로 인한 런타임 에러 방지.

---

## 3. 성능 최적화 (Performance Optimization)

### 3.1 렌더링 파이프라인 효율화 (Dirty Region Update)
*   **현황**: `Renderer`가 `bDirty` 플래그를 확인하지만, 여전히 모든 셀을 순회하며 비교 연산을 수행합니다.
*   **제안**:
    *   변경된 영역의 최소/최대 좌표(Bounding Box)를 계산하여, `WriteConsoleOutputW` 호출 시 해당 영역만 업데이트하도록 변경.
*   **기대 효과**: 콘솔 출력 부하 감소 및 대량의 오브젝트 출력 시 프레임 안정성 향상.

### 3.2 전투 로직(Battle Chain) 경량화
*   **현황**: 전투의 모든 과정이 클래스 기반 `ChainNode`로 분리되어 객체 생성/소멸 및 가상 함수 호출 비용이 발생합니다.
*   **제안**:
    *   반복적으로 발생하는 단순 체크 로직은 함수 포인터 테이블이나 `std::function` 기반의 가벼운 이벤트 시스템으로 대체 고려.
*   **기대 효과**: 전투 처리 속도 향상 및 클래스 개수 감소로 인한 프로젝트 복잡도 완화.

---

## 4. 기타 권장 사항
*   **상수 관리**: 소스 코드 곳곳에 흩어진 매직 넘버(Magic Number)를 `Config/Setting.txt` 또는 전역 `Constants.h`로 통합.
*   **로깅 시스템**: `BattleLogSystem` 외에도 엔진 전반의 상태를 추적할 수 있는 범용 `Logger` 클래스 도입 (파일 출력 지원).
