#pragma once
enum class FieldState
{
    None,
    Idle,
    PlayerTurn,
    Move,
    EnemyTurn,
    EnemyMove,
    EventProcessing,
    BattleTransition,
};