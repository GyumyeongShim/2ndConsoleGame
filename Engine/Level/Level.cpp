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
			if (actor == nullptr) 
				continue;

			if (actor->HasBeganPlay())
				continue;

			actor->BeginPlay();
		}

		m_bBegunPlay = true;
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

			if (actor->IsDestroyRequested())
				continue;

			actor->Draw(renderSys);
		}
	}

	void Level::AddNewActor(Actor* newActor)
	{
		if (newActor == nullptr)
			return;

		m_vecAddRequestedActors.emplace_back(newActor);

		//오너쉽 설정
		newActor->SetOwner(this);
	}

	void Level::ProcessAddAndDestroyActors()
	{
		// 추가 먼저하면 loop가 많아진 상태로 도니까 제거 처리
		for (auto it = m_vecActors.begin(); it != m_vecActors.end(); )
		{
			if ((*it)->IsDestroyRequested() == true) //제거 요청된 액터라면
			{
				delete* it;
				it = m_vecActors.erase(it);
			}
			else
			{
				++it;
			}
		}

		// 추가 처리
		if (m_vecAddRequestedActors.empty() == true)
			return;

		for(Actor* const actor : m_vecAddRequestedActors)
		{
			m_vecActors.emplace_back(actor);

			if (m_bBegunPlay == true && actor->HasBeganPlay() == false)
			{
				actor->BeginPlay();
			}
		}

		m_vecAddRequestedActors.clear();
	}
}