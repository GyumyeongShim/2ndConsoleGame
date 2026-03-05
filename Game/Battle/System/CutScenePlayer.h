#pragma once
#include <queue>
#include <memory>

#include "Interface/ICutsceneEvent.h"
#include "Interface/IEngineCutscenePlayer.h"

namespace Wannabe
{
    class BattleContext;
}

class CutscenePlayer : public Wannabe::IEngineCutscenePlayer
{
private:
    enum class State
    {
        Idle,
        Play,
        Finish
    };

public:
    void Push(std::unique_ptr<Wannabe::ICutsceneEvent> event) override; //연출 추가
    void Update(Wannabe::BattleContext& context, float fDeltaTime = 0.f);
    void ClearCutscenePlayer();

    const bool IsPlaying() const; //연출 중 체크
    const bool IsFinished() const; //연출 종료 체크
    void RequestSkip();
    bool IsSkippable() const;

private:
    State m_eState = State::Idle;
    std::queue<std::unique_ptr<Wannabe::ICutsceneEvent>> m_queCutSceneEvent;
    bool m_bIsStarted = false;

    bool m_bSkipRequested = false;
    float m_fEventCooldown = 0.f;
    float m_fEventInterval = 0.2f; // 로그 출력 최소 간격
};