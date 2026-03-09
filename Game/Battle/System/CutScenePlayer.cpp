#include "CutscenePlayer.h"
#include "Battle/BattleContext.h"
#include "Interface/ICutsceneEvent.h"

void CutscenePlayer::Push(std::unique_ptr<Wannabe::ICutsceneEvent> event)
{
    if (event == nullptr)
        return;

    // PhaseChange 이벤트인 경우 중복 체크
    if (event->GetEventType() == Wannabe::ICutsceneEvent::CutsceneEventType::BattlePhaseChange)
    {
        if (m_queCutSceneEvent.empty() == false)
        {
            auto& lastEvent = m_queCutSceneEvent.back();
            if (lastEvent->GetEventType() == Wannabe::ICutsceneEvent::CutsceneEventType::BattlePhaseChange)
                return;
        }
    }

    m_queCutSceneEvent.push(std::move(event));
    if (m_eState != State::Play)
        m_eState = State::Play;
}

void CutscenePlayer::Update(Wannabe::BattleContext& context, float fDeltaTime)
{
    if (m_eState != State::Play) //재생 여부
        return;

    if (m_queCutSceneEvent.empty() == true) // 재생 완료
    {
        m_eState = State::Idle;
        return;
    }

    if (m_bSkipRequested) //스킵
    {
        while (!m_queCutSceneEvent.empty())
            m_queCutSceneEvent.pop();

        m_bIsStarted = false;
        m_eState = State::Idle;
        m_bSkipRequested = false;
        return;
    }

    m_fEventCooldown -= fDeltaTime; //이벤트 간 최소 간격 대기
    if (m_fEventCooldown > 0)
        return;

    auto& event = m_queCutSceneEvent.front();
    if (event->IsValid(context) == false)
    {
        m_queCutSceneEvent.pop();
        m_bIsStarted = false;
        return;
    }

    if (m_bIsStarted == false) //이벤트 시작 처리
    {
        event->OnStart(context);
        m_bIsStarted = true;
        return;
    }

    bool bIsPlay = event->Update(context, fDeltaTime); //이벤트 업데이트 (재생 true / 종료 false)
    if (bIsPlay == false || event->IsFinished())  //종료 판정
    {
        event->OnEnd(context);

        m_queCutSceneEvent.pop();
        m_bIsStarted = false;
        m_fEventCooldown = m_fEventInterval;
    }
}

void CutscenePlayer::ClearCutscenePlayer()
{
    while (!m_queCutSceneEvent.empty())
        m_queCutSceneEvent.pop();

    m_bIsStarted = false;
    m_eState = State::Idle;
}

const bool CutscenePlayer::IsPlaying() const
{
    return m_eState == State::Play;
}

const bool CutscenePlayer::IsFinished() const
{
    return m_eState == State::Finish;
}

void CutscenePlayer::RequestSkip()
{
    m_bSkipRequested = true;
}

bool CutscenePlayer::IsSkippable() const
{
    return true;
}