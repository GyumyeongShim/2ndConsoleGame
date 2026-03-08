# 전투 시스템 도식화 (Battle System Diagram)

## 1. 전투 루프 흐름도 (Battle Flow Chart)

```mermaid
graph TD
    A[Start] --> B{Phase_TurnCheck}
    B -- No actor ready --> C[TurnManager: ProgressTurns]
    C --> B
    B -- Actor Found --> D[Push TurnStartEvent]
    D --> E[Phase_Animation]
    E -- TurnStartEvent: OnStart --> F{Team Check}
    
    F -- Player --> G[Phase_CommandSelect]
    G --> H[Input: Atk / Skill / Item]
    H --> I[Phase_TargetSelect]
    I --> J[Push IBattleCommand to Queue]
    J --> K[Phase_Animation]
    
    F -- Enemy --> L[EnemyAI: Decide]
    L --> J
    
    K -- Play CutsceneEvents --> M{Command Exists?}
    M -- Yes --> N[Command Execute]
    N --> O[BattleResolver Resolve]
    O --> P[BattleEventProcessor Create Events]
    P --> Q[Push Events to CutscenePlayer]
    Q --> K
    
    M -- No Command & Anim End --> R[TurnManager TurnEnd]
    R --> S[Push TurnEndEvent]
    S --> B
    
    B -- Victory / Defeat --> T[Phase_Result]
    T --> U[End]

## 2. 시뮬레이션 및 연출 계층 (Layered Architecture)

```mermaid
sequenceDiagram
    participant BattleLevel as Level
    participant IBattleCommand as Cmd
    participant BattleResolver as Res
    participant BattleEventProcessor as Proc
    participant CutscenePlayer as Play
    participant StatComponent as Stat

    Level->>Level: Phase_Animation Update
    Level->>Cmd: Execute(Context)

    Cmd->>Res: ResolveAction(Attacker, Target, Data)
    Res-->>Cmd: CombatEffectResult

    Cmd->>Proc: ProcessCombatEffectResult(Result)

    Proc->>Stat: ApplyDamage / Heal / Status
    Proc->>Play: Push VisualEffectEvent
    Proc->>Play: Push DamageEvent
    Proc->>Play: Push LogEvent

    loop Animation
        Level->>Play: Update(deltaTime)
        Play->>Play: Play Sequential Events
    end

## 3. 클래스 관계도 (Class Diagram)

```mermaid
classDiagram

    class BattleLevel {
        -BattleState battleState
        -queue<IBattleCommand> commandQueue
        +Tick(deltaTime)
        +Phase_TurnCheck()
        +Phase_Animation()
    }

    class TurnManager {
        -Actor currentActor
        +GetNextActor()
        +ProgressTurns()
        +TurnEnd()
    }

    class BattleContext {
        +BattleResolver resolver
        +BattleEventProcessor processor
        +CutscenePlayer cutscenePlayer
    }

    class BattleResolver {
        +ResolveBasicAttack()
        +ResolveAction()
        +ResolveStatusEffects()
    }

    class BattleEventProcessor {
        -IBattleEventFactory eventFactory
        +ProcessCombatEffectResult()
        +ApplyDamage()
        +ApplyStatus()
    }

    class CutscenePlayer {
        -queue<ICutsceneEvent> eventQueue
        +Update()
        +Push()
    }

    class IBattleCommand {
        <<interface>>
        +Execute(context)
    }

    BattleLevel *-- TurnManager
    BattleLevel *-- BattleContext

    BattleContext *-- BattleResolver
    BattleContext *-- BattleEventProcessor
    BattleContext *-- CutscenePlayer

    IBattleCommand <.. BattleLevel
