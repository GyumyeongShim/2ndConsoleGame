#pragma once
enum class FieldState
{
    None,
    PlayerTurn,
    Move,
    EnemyTurn,
    EnemyMove,
    EventProcessing,
    BattleTransition,
};