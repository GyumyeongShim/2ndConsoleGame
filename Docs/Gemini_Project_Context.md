# [System Prompt Extension: 2ndConsoleGame Project]

이 프롬프트는 `2ndConsoleGame` 프로젝트의 핵심 아키텍처와 개발 원칙을 담고 있습니다. 새로운 대화 시작 시 이 내용을 입력하면 AI가 프로젝트의 맥락을 즉시 파악합니다.

---

## 1. 프로젝트 정체성 (Project Identity)
- **기술 스택**: C++, Win32 Console API, ANSI, ECS(Actor-Component) 아키텍처
- **목표**: 엔진과 게임 로직이 분리된 확장 가능한 콘솔 RPG 프레임워크 유지 및 발전

## 2. 핵심 아키텍처 가이드라인 (Architectural Principles)
- **Engine vs Game 분리**: 모든 핵심 시스템(렌더링, 물리, 기초 전투 엔진)은 `Engine/` 폴더에 위치하며, 구체적인 콘텐츠(캐릭터, 아이템, 전투 시퀀스)는 `Game/` 폴더에서 엔진을 상속받거나 활용하여 구현한다.
- **전투 시스템 흐름 (Battle Simulation Flow)**:
    - `BattleResolver`: 순수 수치 계산 (Logic)
    - `BattleResult/CombatEffectResult`: 계산 결과 데이터 (Data)
    - `BattleEventProcessor`: 결과를 해석하여 데이터 반영 및 연출 생성 (Interpreter)
    - `CutscenePlayer`: 생성된 이벤트를 순차적으로 재생 (Presentation)
- **Actor-Component**: 기능을 추가할 때는 새로운 `Actor`를 만들기보다 기존 `Actor`에 붙일 수 있는 `Component`를 설계하는 것을 우선한다.

## 3. 코딩 표준 및 명명 규칙 (Coding Standards)
- **언어**: C++ (최신 표준 활용)
- **명명 규칙**: 
    - 클래스/구조체: `PascalCase`
    - 멤버 변수: `m_` 접두어 + `PascalCase` (예: `m_pStatComponent`)
    - 포인터 변수: `p` 접두어 활용 (예: `pTarget`)
    - 함수: `PascalCase`
- **메모리 관리**: 가능하면 `std::unique_ptr`, `std::shared_ptr`를 사용하여 메모리 누수를 방지한다. 생포인터 사용 시 해제 로직을 엄격히 검토한다.

## 4. 협업 프로세스 (Workflow)
1. **Research**: 변경 전 관련 파일(`Engine`과 `Game` 양쪽)을 확인하고 의존성을 파악한다.
2. **Strategy**: `Docs/Battle Simulation Flow.txt` 및 `Docs/BattleDiagram.md`를 참고하여 기존 설계 철학에 어긋나지 않는지 검토한다.
3. **Execution**: 최소한의 코드 변경(Surgical Update)으로 목표를 달성하며, 기존 기능을 파괴하지 않는다.
4. **Validation**: 변경 후 관련 UI 갱신 및 데이터 반영 여부를 반드시 확인한다.

## 5. 주요 참조 문서
- `Docs/Battle Simulation Flow.txt`: 전투 로직 상세 흐름
- `Docs/BattleDiagram.md`: 시스템 도식화 및 클래스 관계
- `Assets/*.json`: 밸런스 및 설정 데이터
