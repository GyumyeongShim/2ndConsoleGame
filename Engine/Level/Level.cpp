#include "Level.h"
#include "Actor/Actor.h"
#include "Util/Utill.h"

namespace Wannabe
{
	Level::Level()
	{
	}

	Level::~Level()
	{
		for (Actor*& actor : m_vecActors)
			delete actor;

		m_vecActors.clear();

		for (Actor*& actor : m_vecAddRequestedActors)
			delete actor;

		m_vecAddRequestedActors.clear();

		SafeDelete(m_pCamera);
	}

	void Level::BeginPlay()
	{
		// 액터에 이벤트 흘리기
		for (Actor* actor : m_vecActors)
		{
			// 게임 진행 도중에 생성된 경우에도 진행해야하니까.
			//고정된 형태로 한다면 필요 없을 수 있음.
			if (actor->HasBeganPlay())
				continue;

			actor->BeginPlay();
		}
	}

	void Level::Tick(float fDeltaTime)
	{
		// 액터에 이벤트 흘리기
		for (Actor* actor : m_vecActors)
		{
			if (actor->IsDestroyRequested())
				continue;

			actor->Tick(fDeltaTime);
		}
	}

	void Level::Draw(Wannabe::RenderSystem& renderSys)
	{
		for (Actor* const actor : m_vecActors)
		{
			if (actor->IsActive() == false)
				continue;

			actor->Draw(renderSys);
		}
	}

	void Level::AddNewActor(Actor* newActor)
	{
		m_vecAddRequestedActors.emplace_back(newActor);

		//오너쉽 설정
		newActor->SetOwner(this);
	}

	void Level::ProcessAddAndDestroyActors()
	{
		//추가 먼저하면 loop가 많아진 상태로 도니까 제거 처리
		for(int ix=0;ix<static_cast<int>(m_vecActors.size());)
		{
			//제거 요청된 액터라면
			if (m_vecActors[ix]->IsDestroyRequested())
			{
				delete m_vecActors[ix];
				m_vecActors[ix] = nullptr;
				m_vecActors.erase(m_vecActors.begin() + ix);
				continue;
			}

			++ix;
		}

		//추가 처리
		if (m_vecAddRequestedActors.empty() == true)
			return;

		for(Actor* const actor : m_vecAddRequestedActors)
		{
			m_vecActors.emplace_back(actor);
		}

		m_vecAddRequestedActors.clear();
	}
}