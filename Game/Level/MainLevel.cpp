#include "MainLevel.h"

#include <iostream>
#include <queue>
#include <set>

#include "Engine/Engine.h"
#include "Game/Game.h"
#include "Core/Input.h"
#include "Util/Utill.h"
#include "Render/Renderer.h"

#include "World/TileMap.h"

#include "Manager/SaveManager.h"
#include "Battle/TurnManager.h"
#include "Actor/Player.h"
#include "Actor/Monster.h"

#include "Level/TownLevel.h"
#include "Component/StatComponent.h"

using namespace Wannabe;

MainLevel::MainLevel()
{
}

MainLevel::~MainLevel()
{
	m_vecMonsters.clear();
	SafeDelete(m_pTurnManager);
}

void MainLevel::BeginPlay()
{
	super::BeginPlay();
	ConfigureRenderer(Engine::Get().GetRenderSystem());
}

void MainLevel::Tick(float fDeltaTime)
{
	super::Tick(fDeltaTime);
	m_fTickCollisionTime += fDeltaTime * 0.5f;

	switch (m_eFieldPhase)
	{
	case FieldState::Idle:
		Phase_Idle();
		break;

	case FieldState::PlayerTurn:      
		Phase_PlayerTurn(fDeltaTime); 
		break;

	case FieldState::Move:            
		Phase_Move(fDeltaTime); 
		break;

	case FieldState::EnemyTurn:       
		Phase_EnemyTurn(); 
		break;

	case FieldState::EnemyMove:       
		Phase_EnemyMove(fDeltaTime); 
		break;

	case FieldState::BattleTransition: 
		Phase_Transition(fDeltaTime); 
		break;

	}
}

void MainLevel::Draw(Wannabe::RenderSystem& renderSys)
{
	if (m_worldMap)
		m_worldMap->Draw(renderSys);

	// 1. 이동 범위 하이라이트 (바닥)
	if (m_eFieldPhase == FieldState::PlayerTurn && m_bIsRangeCalculated)
	{
		for (const auto& tilePos : m_vecMoveRangeTiles)
		{
			Vector2 screenPos = tilePos - renderSys.GetCamera().GetPosition();
			renderSys.GetWorldCanvas().DrawTxt(screenPos.x, screenPos.y, L"\u2592", Color::BrightCyan,1);
		}
	}

	if (m_eFieldPhase == FieldState::EnemyTurn ||
		m_eFieldPhase == FieldState::EnemyMove ||
		m_vecMonsterRangeTiles.empty() == false)
	{
		for (const auto& vPos : m_vecMonsterRangeTiles)
		{
			Vector2 vScreen = renderSys.GetCamera().WorldToScreen(vPos);
			// 붉은색 쉐이드 타일로 위험 지역 표시
			renderSys.GetWorldCanvas().DrawTxt(vScreen.x, vScreen.y, L"\u2591", Color::Red,1);
		}
	}

	super::Draw(renderSys);

	// 2. 커서
	if (m_eFieldPhase != FieldState::BattleTransition)
	{
		Vector2 cursorScreenPos = renderSys.GetCamera().WorldToScreen(m_vCursorPos);
		renderSys.GetUICanvas().DrawTxt(cursorScreenPos.x, cursorScreenPos.y, L"\u25A3", Color::BrightLime,2);
	}

	// 3. 전환 연출 (최상단)
	if (m_eFieldPhase == FieldState::BattleTransition)
	{
		auto& effectCanvas = renderSys.GetEffectCanvas();
		effectCanvas.Clear(); // 이전 프레임 잔상 제거

		static struct Stream {
			int headY;           // 현재 줄의 머리 위치
			int length;          // 스트림 길이
		} streams[256]; // 최대 256 열 (필요시 effectCanvas.GetWidth() 사용)

		static bool initialized = false;
		if (!initialized)
		{
			for (int x = 0; x < effectCanvas.GetWidth(); ++x)
			{
				streams[x].headY = rand() % effectCanvas.GetHeight();
				streams[x].length = 3 + rand() % 6; // 3~8 길이 랜덤
			}
			initialized = true;
		}

		auto RandomChar = []() -> wchar_t {
			const wchar_t chars[] = L"089MNOP345UVW12JKLIQRSBCDEFGHAT#$%XYZ@67&*";
			return chars[rand() % (sizeof(chars) / sizeof(wchar_t) - 1)];
			};

		// 세로 스트림 그리기
		for (int x = 0; x < effectCanvas.GetWidth(); ++x)
		{
			for (int i = 0; i < streams[x].length; ++i)
			{
				int y = (streams[x].headY - i + effectCanvas.GetHeight()) % effectCanvas.GetHeight();
				effectCanvas.DrawTxt(x, y, std::wstring(1, RandomChar()), Color::Green);
			}

			// 머리 위치 이동 (아래로 흐르도록)
			streams[x].headY = (streams[x].headY + 1) % effectCanvas.GetHeight();

			// 길이 변경 (약간의 랜덤성 추가)
			if (rand() % 20 == 0) // 5% 확률로 길이 바꾸기
				streams[x].length = 3 + rand() % 6;
		}

		// 중앙 텍스트 고정
		int centerX = effectCanvas.GetWidth() / 2 - 8;
		int centerY = effectCanvas.GetHeight() / 2;
		effectCanvas.DrawTxt(centerX, centerY, L"!! ENCOUNTER !!", Color::Purple);
	}
}

void MainLevel::ConfigureRenderer(Wannabe::RenderSystem& renderer) const
{
	renderer.SetRenderMode(Wannabe::RenderSystem::RenderMode::Field);
	renderer.GetCamera().SetFollowMode(true);
}

void MainLevel::OnEnterLevel(RunGameData* pData)
{
	if (pData == nullptr)
		return;

	// 맵 불러오기
	std::string mapFileName;
	std::vector<int> spawnMonsterTIDs;
	std::vector<Vector2> spawnPoints;
	if (m_worldMap == nullptr)
	{
		// Init에서 할당되지 않았을 경우에 대비
		m_worldMap = std::make_unique<TileMap>();
		m_worldMap->Init(Engine::Get().GetSetting().width, Engine::Get().GetSetting().height);
	}

	if (pData->m_CurLevelId == 1)
	{
		mapFileName = "Assets/dungeon.txt";
		spawnMonsterTIDs = { 2, 2, 3, 3 };
		spawnPoints = { {15, 8}, {40, 6}, {30, 15},{5, 15} };

		if (pData->m_NextWorldPos == Vector2::Zero)
			pData->m_NextWorldPos = Vector2(2, 2); // 던전 입구
	}
	else if (pData->m_CurLevelId == 2)
	{
		mapFileName = "Assets/forest.txt";
		spawnMonsterTIDs = { 1, 1, 4, 4 };
		spawnPoints = { {5, 5}, {10, 5}, {45, 16}, {50, 5} };

		pData->m_NextWorldPos = Vector2(30, 11); // 포탈 근처
	}
	m_worldMap->LoadFromFile(mapFileName);

	// 최초 진입한 경우
	if (m_pPlayer == nullptr) 
	{
		m_pPlayer = new Player(1, pData->m_NextWorldPos);
		if (m_pPlayer->GetComponent<StatComponent>())
			m_pPlayer->GetComponent<StatComponent>()->SetStatByData(pData->m_PlayerStat);

		AddNewActor(m_pPlayer);
		m_vecPlayers.emplace_back(m_pPlayer);

		// 몬스터 최초 스폰 (최초 1회만)
		m_vecMonsters.clear();
		for (const auto& pos : spawnPoints) // 별도의 스폰 포인트 리스트 활용
		{
			Monster* pMonster = new Monster(Util::Random(1, 10));
			pMonster->SetPosition(pos);
			AddNewActor(pMonster);
			m_vecMonsters.emplace_back(pMonster);
		}
	}
	// 전투 후 복귀한 경우
	else
	{
		int iBeforeCount = (int)m_vecMonsters.size();

		auto it = std::remove_if(m_vecMonsters.begin(), m_vecMonsters.end(),
			[](Actor* p) {
			return p == nullptr || p->IsDestroyRequested();
			});
		m_vecMonsters.erase(it, m_vecMonsters.end());

		int iKilled = iBeforeCount - (int)m_vecMonsters.size();
		pData->m_iKillCount += iKilled;

		if (m_pTurnManager)
			m_pTurnManager->ResetTurns();

		if (m_vecMonsters.empty())
		{
			pData->m_PlayerStat = m_pPlayer->GetComponent<StatComponent>()->GetStatData();
			pData->m_bIsClear = true;
			// TODO: 여기서 바로 저장하기보다, "이름 입력 UI"를 띄우는 상태(Phase)로 전환하는 것이 좋습니다.
				// 임시로 레코드 저장 호출
			std::string recordName = pData->m_UserName + "_" + Util::GetCurrentDateTimeString();
			if (SaveManager::Get().SaveRecord(recordName, *pData))
			{
				std::cout << "Final Victory Recorded!" << std::endl;
			}
		}
	}

	//카메라 설정
	if (m_pCamera == nullptr)
	{
		Vector2 vScreenSize = Vector2(Engine::Get().GetSetting().width, Engine::Get().GetSetting().height);
		m_pCamera = new Camera(vScreenSize.x, vScreenSize.y);
	}

	if (m_pCamera != nullptr)
	{
		m_pCamera->SetFollowMode(true);
		if (m_pPlayer != nullptr)
		{
			m_pCamera->SetFollowTarget(&m_pPlayer->GetPosition());
		}
	}

	m_pCamera->SetFollowMode(true);
	Engine::Get().GetRenderSystem().SetCamera(*m_pCamera);

	// 데이터 정리 및 상태 초기화
	if (pData->m_pEncounteredEnemy != nullptr)
	{
		pData->m_pEncounteredEnemy = nullptr;
		pData->m_vecBattleMonsters.clear();
	}

	if (m_pTurnManager == nullptr)
		m_pTurnManager = new TurnManager();

	m_eFieldPhase = FieldState::Idle;
	m_fTransitionTimer = 0.0f;
	m_bShowFlash = false;
}

void MainLevel::OnExitLevel(RunGameData* pData)
{
	if (pData == nullptr || m_pPlayer == nullptr)
		return;

	pData->m_NextWorldPos = m_pPlayer->GetPosition();

	if (m_pPlayer->GetComponent<StatComponent>())
		pData->m_PlayerStat = m_pPlayer->GetComponent<StatComponent>()->GetStatData();
}

bool MainLevel::CanMove(const Vector2& nextPos)
{
	Vector2 tilePos = m_worldMap->WorldToTile(nextPos);

	return m_worldMap->IsWalkable(tilePos.x, tilePos.y);
}

void MainLevel::CheckPortal()
{
	Vector2 tilePos = m_worldMap->WorldToTile(m_pPlayer->GetPosition());
	const Tile* pCurrentTile = m_worldMap->GetTile(tilePos.x, tilePos.y);

	if (pCurrentTile && pCurrentTile->eType == TileType::Portal)
	{
		RunGameData* pRunData = Game::Get().GetRunData();
		if (pRunData)
		{
			// 현재 Forest(2)라면 Dungeon(1)으로, 반대면 반대로
			//if (pRunData->m_CurLevelId == 2)
			//{
			//	pRunData->m_CurLevelId = 1;
			//	pRunData->m_NextWorldPos = Vector2(2, 2); // 던전 시작점
			//}
			//else
			//{
			//	pRunData->m_CurLevelId = 2;
			//	pRunData->m_NextWorldPos = Vector2(45, 11); // 포탈 앞
			//}
			pRunData->m_lastWorldPos = { 1, 1 }; // 마을 입구 좌표
			Game::Get().RequestChangeLevel(TownLevel::TypeIdClass());
		}
	}
}

void MainLevel::Phase_Idle()
{
	RunGameData* pRunData = Game::Get().GetRunData();
	if (pRunData && pRunData->m_pEncounteredEnemy != nullptr)
		pRunData->m_pEncounteredEnemy = nullptr;

	CheckMonsterEncounter();

	if (m_eFieldPhase == FieldState::BattleTransition)
		return;

	if (m_pPlayer != nullptr)
	{
		// 이동 범위 계산 플래그 초기화 (PlayerTurn 진입 시 다시 계산하도록)
		m_bIsRangeCalculated = false;
		m_vecMoveRangeTiles.clear();
		m_vecMonsterRangeTiles.clear();
	}

	m_pCurActor = m_pTurnManager->GetNextActor(m_vecPlayers, m_vecMonsters);
	if (m_pCurActor != nullptr)
	{
		if (m_pCurActor->IsTypeOf<Player>())
		{
			m_eFieldPhase = FieldState::PlayerTurn;
			m_vCursorPos = m_pCurActor->GetPosition();
			m_bIsRangeCalculated = false;
		}
		else if (m_pCurActor->IsTypeOf<Monster>())
		{
			m_eFieldPhase = FieldState::EnemyTurn;
			CalcMonsterMoveRange(m_pCurActor);
		}
	}
	else
		m_pTurnManager->ProgressTurns();
}

void MainLevel::Phase_PlayerTurn(float fDeltaTime)
{
	if (!m_bIsRangeCalculated)
		CalcMoveRange();

	// 1. 커서 이동 변수 관리
	static float fCursorMoveTimer = 0.0f;
	const float fInitialDelay = 0.25f; // 처음 눌렀을 때 다음 칸으로 넘어가기 전 대기 시간
	const float fRepeatDelay = 0.05f;  // 꾹 누르고 있을 때 이동 속도 (값이 작을수록 빠름)

	Vector2 vDir = Vector2::Zero;
	bool bIsMoving = false;

	// 2. 입력 감지 (GetKey를 사용하여 누르고 있는 상태를 체크)
	if (Input::Get().GetKey(VK_UP)) 
	{ 
		vDir.y = -1; 
		bIsMoving = true;
	}
	if (Input::Get().GetKey(VK_DOWN)) 
	{ 
		vDir.y = 1;  
		bIsMoving = true;
	}
	if (Input::Get().GetKey(VK_LEFT)) 
	{ 
		vDir.x = -1; 
		bIsMoving = true;
	}
	if (Input::Get().GetKey(VK_RIGHT)) 
	{ 
		vDir.x = 1;  
		bIsMoving = true;
	}

	if (bIsMoving)
	{
		// 처음 누르는 순간(Down)이거나, 누르고 있는 시간이 딜레이를 초과했을 때
		bool bInitialHit = (Input::Get().GetKeyDown(VK_UP) || Input::Get().GetKeyDown(VK_DOWN) ||
			Input::Get().GetKeyDown(VK_LEFT) || Input::Get().GetKeyDown(VK_RIGHT));

		fCursorMoveTimer += fDeltaTime;

		// 꾹 누르기 시작할 때는 fInitialDelay만큼 기다렸다가, 그 뒤부터는 fRepeatDelay 속도로 연사
		float currentDelay = (fCursorMoveTimer < fInitialDelay) ? fInitialDelay : fRepeatDelay;

		if (bInitialHit || fCursorMoveTimer >= currentDelay)
		{
			Vector2 vNextPos = m_vCursorPos + vDir;

			if (m_worldMap->IsInMap((int)vNextPos.x, (int)vNextPos.y))
			{
				m_vCursorPos = vNextPos;
				OnCursorMoved(); // 몬스터 범위 갱신 등
			}

			// 초기 입력을 감지했다면 타이머를 약간 진행시켜 쾌적하게 만듦
			if (bInitialHit) 
				fCursorMoveTimer = 0.0f;
			else 
				fCursorMoveTimer = fInitialDelay; // 반복 구간으로 진입
		}
	}
	else
	{
		fCursorMoveTimer = 0.0f; // 키를 떼면 타이머 초기화
	}

	// 3. 선택 (Enter 키)
	if (Input::Get().GetKeyDown(VK_RETURN))
	{
		bool bIsInRange = (std::find(m_vecMoveRangeTiles.begin(), m_vecMoveRangeTiles.end(),
			m_vCursorPos) != m_vecMoveRangeTiles.end());

		if (bIsInRange || m_vCursorPos == m_pPlayer->GetPosition())
		{
			if (m_vCursorPos == m_pPlayer->GetPosition())
			{
				m_eFieldPhase = FieldState::EnemyTurn;
				return;
			}

			m_vecPath = m_worldMap->FindPath(m_pPlayer->GetPosition(), m_vCursorPos);

			if (!m_vecPath.empty())
			{
				m_eFieldPhase = FieldState::Move;
				m_bIsRangeCalculated = false;
			}
		}
	}
}

void MainLevel::Phase_Move(float fDeltaTime)
{
	if (m_vecPath.empty())
	{
		m_pTurnManager->TurnEnd();
		CheckMonsterEncounter();

		m_vecMonsterRangeTiles.clear();
		m_vecMoveRangeTiles.clear();

		if (m_eFieldPhase == FieldState::BattleTransition)
			return;

		CheckPortal();

		if (m_eFieldPhase != FieldState::BattleTransition)
			m_eFieldPhase = FieldState::EnemyTurn;

		return;
	}

	m_fMoveTimer += fDeltaTime;

	// 0.1초마다 한 칸씩 이동
	if (m_fMoveTimer >= 0.1f)
	{
		m_pPlayer->SetPosition(m_vecPath.front());
		m_vecPath.erase(m_vecPath.begin());
		m_fMoveTimer = 0.0f;
	}
}

void MainLevel::Phase_EnemyTurn()
{
	Monster* pMonster = static_cast<Monster*>(m_pCurActor);
	if (pMonster == nullptr || pMonster->IsDestroyRequested())
	{
		m_eFieldPhase = FieldState::Idle; // 행동 불가 시 다시 결정
		return;
	}

	Vector2 vMonsterPos = pMonster->GetPosition();
	Vector2 vPlayerPos = m_pPlayer->GetPosition();

	// 거리 계산
	Vector2 vDiff;
	float fDistToPlayer = vDiff.Distance(vMonsterPos, vPlayerPos);

	const float fSenseRange = 5.0f;
	if (fDistToPlayer <= fSenseRange)
	{
		// 플레이어 방향으로 경로 탐색
		std::vector<Vector2> fullPath = m_worldMap->FindPath(vMonsterPos, vPlayerPos);

		if (fullPath.size() > 1)
		{
			// 이동력만큼 자르기
			size_t iLimit = (size_t)pMonster->GetMoveRange() + 1;
			if (fullPath.size() > iLimit)
			{
				fullPath.resize(iLimit);
			}

			// 겹침 방지, 경로의 마지막 칸(목적지)에 이미 누가 있는지 확인
			while (fullPath.size() > 1)
			{
				Vector2 vFinalDest = fullPath.back();

				// 플레이어와 겹치거나, 다른 몬스터가 이미 점유 중이라면
				if (vFinalDest == vPlayerPos || IsTileOccupied(vFinalDest, pMonster))
					fullPath.pop_back(); // 한 칸 뒤로 후퇴
				else
					break; // 빈 칸을 찾았으면 확정
			}

			pMonster->SetMovePath(fullPath);
		}
	}
	else
		pMonster->GetMovePath().clear();

	// 첫 번째 몬스터부터 이동 연출을 시작하기 위해 인덱스 초기화
	m_iCurEnemyIdx = 0;
	m_eFieldPhase = FieldState::EnemyMove;
}

void MainLevel::Phase_EnemyMove(float fDeltaTime)
{
	// 모든 몬스터의 이동 처리가 끝났다면 플레이어 턴으로 복귀
	if (m_iCurEnemyIdx >= (int)m_vecMonsters.size())
	{
		m_pTurnManager->TurnEnd();
		m_eFieldPhase = FieldState::Idle;
		m_vCursorPos = m_pPlayer->GetPosition(); // 커서를 플레이어 위치로 초기화
		m_bIsRangeCalculated = false;            // 다음 턴 범위 계산 준비
		return;
	}

	Monster* pCurrentMonster = static_cast<Monster*>(m_vecMonsters[m_iCurEnemyIdx]);

	// 이동할 경로가 없거나 이미 파괴된 몬스터는 즉시 다음으로 패스
	if (pCurrentMonster->IsDestroyRequested() || pCurrentMonster->GetMovePath().empty())
	{
		CheckMonsterEncounter();
		if (m_eFieldPhase == FieldState::BattleTransition)
			return;

		m_iCurEnemyIdx++;
		m_fMoveTimer = 0.0f;
		return;
	}

	m_fMoveTimer += fDeltaTime;

	// 0.15초~0.2초 간격으로 한 칸씩 이동 (플레이어보다 약간 느리게 설정하여 가시성 확보)
	if (m_fMoveTimer >= 0.2f)
	{
		auto& path = pCurrentMonster->GetMovePath();

		// 현재 서 있는 위치가 경로의 시작점이면 제거
		if (!path.empty() && path.front() == pCurrentMonster->GetPosition())
		{
			path.erase(path.begin());
		}

		if (!path.empty())
		{
			pCurrentMonster->SetPosition(path.front());
			path.erase(path.begin());
			m_fMoveTimer = 0.0f;
		}

		// 해당 몬스터가 목적지에 도착했다면 다음 몬스터로 교체
		if (path.empty())
		{
			m_iCurEnemyIdx++;
			m_fMoveTimer = 0.0f;
		}
	}
}

void MainLevel::Phase_Transition(float fDeltaTime)
{
	m_fTransitionTimer += fDeltaTime;
	if (m_fTransitionTimer > 0.1f) 
		m_bShowFlash = false;

	if (m_fTransitionTimer >= m_fTransitionDuration)
		Game::Get().BattleStart(m_vecPlayers, m_vecMonsters);
}

void MainLevel::Init()
{
	Vector2 vScreenSize = Vector2(Engine::Get().GetSetting().width, Engine::Get().GetSetting().height);
	m_pCamera = new Camera(vScreenSize.x, vScreenSize.y);
	m_fTickCollisionTime = 0;

	//todo 추가하는 방법 수정 필요...
	AddNewActor(new Monster(1));
	AddNewActor(new Monster(2));
	AddNewActor(new Monster(3));
	AddNewActor(new Monster(4));

	for (Actor* const actor : m_vecAddRequestedActors) //전체 오브젝트를 순회
	{
		if (actor->IsTypeOf<Monster>() == true)
		{
			m_vecMonsters.emplace_back(actor);
			continue;
		}
	}

	m_worldMap = std::make_unique<TileMap>();
	m_worldMap->Init(Engine::Get().GetSetting().width, Engine::Get().GetSetting().height);
	m_worldMap->LoadFromFile("Assets/forest.txt");

	// Player 추가
	Player* player = new Player(1, m_vPlayerStartPos);
	m_pPlayer = player;
	AddNewActor(player);
	for (Actor* const actor : m_vecAddRequestedActors) //전체 오브젝트를 순회
	{
		if (actor->IsTypeOf<Player>() == true)
		{
			m_vecPlayers.emplace_back(actor);
			continue;
		}
	}

	m_pCamera->SetFollowMode(true);
	m_pCamera->SetFollowTarget(&m_pPlayer->GetPosition());

	ConfigureRenderer(Engine::Get().GetRenderSystem());
}

void MainLevel::CheckRandomEncounter()
{
	if (m_pPlayer == nullptr)
		return;

	if (m_pPlayer->GetPosition() == m_vLastPlayerPos) //같은 위치
		return;

	m_vLastPlayerPos = m_pPlayer->GetPosition();

	Vector2 tilePos = m_worldMap->WorldToTile(m_pPlayer->GetPosition());
	const Tile* tile = m_worldMap->GetTile(tilePos.x, tilePos.y);
	if (tile == nullptr)
		return;

	if (tile->eType == TileType::Bush)
	{
		int chance = Util::Random(1, 100);

		if (chance <= 10)
		{
			m_fTickCollisionTime = 0;
			m_eFieldPhase = FieldState::BattleTransition; // 상태만 변경
			m_fTransitionTimer = 0.0f;
			m_bShowFlash = true;
		}
	}
}

void MainLevel::CheckMonsterEncounter()
{
	if (m_pPlayer == nullptr) 
		return;

	Vector2 playerPos = m_pPlayer->GetPosition();
	for (auto* pEnemy : m_vecMonsters)
	{
		if (pEnemy == nullptr || pEnemy->IsDestroyRequested())
			continue;

		Vector2 monsterPos = pEnemy->GetPosition();

		Vector2 temp;
		float fDist = temp.Distance(playerPos, monsterPos);

		if (fDist < 0.8f) // 같은 칸에 겹쳤을 때
		{
			StartBattleTransition(pEnemy);
			return;
		}
	}
}

void MainLevel::StartBattleTransition(Actor* pTarget)
{
	RunGameData* pRunData = Game::Get().GetRunData();
	if (pRunData == nullptr) 
		return;

	// 1. 전투에 참여할 멤버 확정
	pRunData->m_vecBattlePlayers.clear();
	pRunData->m_vecBattlePlayers.emplace_back(m_pPlayer); // 필드의 플레이어 추가

	pRunData->m_vecBattleMonsters.clear();
	pRunData->m_vecBattleMonsters.emplace_back(pTarget); // 부딪힌 그 몬스터 추가

	// [고도화] 주변에 다른 몬스터가 있다면 난입시키기
	Vector2 temp;
	for (Actor* pOthers : m_vecMonsters)
	{
		if (pOthers == pTarget) 
			continue;

		// 부딪힌 적 주변 3칸 이내에 다른 적이 있다면 같이 전투 참여
		if (temp.Distance(pTarget->GetPosition(), pOthers->GetPosition()) <= 3.0f)
		{
			pRunData->m_vecBattleMonsters.emplace_back(pOthers);

			if (pRunData->m_vecBattleMonsters.size() >= 4) 
				break; // 최대 4마리 제한
		}
	}

	// 2. 필드 상태를 연출 상태로 전환
	pRunData->m_pEncounteredEnemy = pTarget;
	m_eFieldPhase = FieldState::BattleTransition;
	m_fTransitionTimer = 0.0f;
	m_bShowFlash = true;
}

bool MainLevel::IsTileOccupied(const Vector2& targetPos, Actor* pSelf)
{
	// Player 위치
	if (m_pPlayer != nullptr && m_pPlayer->GetPosition() == targetPos)
		return true;

	// 다른 몬스터들 위치 확인
	for (auto* pMonster : m_vecMonsters)
	{
		if (pMonster == nullptr || pMonster == pSelf || pMonster->IsDestroyRequested())
			continue;

		if (pMonster->GetPosition() == targetPos)
			return true;
	}

	return false;
}

void MainLevel::CalcMoveRange()
{
	m_vecMoveRangeTiles.clear();
	if (m_pPlayer == nullptr || m_worldMap == nullptr)
		return;

	Vector2 startPos = m_pPlayer->GetPosition();
	int iRange = m_pPlayer->GetMoveRange();

	// {좌표, 소모된 이동력}을 담는 큐
	std::queue<std::pair<Vector2, int>> q;
	q.push({ startPos, 0 });

	// 중복 방문 방지를 위한 셋 (x, y 좌표를 pair로 저장)
	std::set<std::pair<int, int>> visited;
	visited.insert({ (int)startPos.x, (int)startPos.y });

	while (!q.empty())
	{
		Vector2 curPos = q.front().first;
		int curDist = q.front().second;
		q.pop();

		// 시작 위치를 제외하고 이동 가능 목록에 추가
		if (curPos != startPos)
		{
			m_vecMoveRangeTiles.emplace_back(curPos);
		}

		// 이동력을 모두 소모했다면 더 이상 확장하지 않음
		if (curDist >= iRange)
		{
			continue;
		}

		// 상, 하, 좌, 우 4방향 탐색
		Vector2 dirs[] = { {0, -1}, {0, 1}, {-1, 0}, {1, 0} };
		for (const auto& dir : dirs)
		{
			Vector2 nextPos = curPos + dir;

			// 1. 맵 범위 및 이동 가능 여부 체크
			if (!m_worldMap->IsWalkable((int)nextPos.x, (int)nextPos.y))
			{
				continue;
			}

			// 2. 이미 방문한 타일인지 체크
			if (visited.find({ (int)nextPos.x, (int)nextPos.y }) == visited.end())
			{
				visited.insert({ (int)nextPos.x, (int)nextPos.y });
				q.push({ nextPos, curDist + 1 });
			}
		}
	}

	m_bIsRangeCalculated = true;
}

void MainLevel::CalcMonsterMoveRange(Actor* pTarget)
{
	if (pTarget == nullptr)
		return;

	int iRange = pTarget->GetMoveRange();
	Vector2 vStart = pTarget->GetPosition();

	m_vecMonsterRangeTiles.clear();

	Vector2 startPos = pTarget->GetPosition();
	int maxRange = pTarget->GetMoveRange();

	// BFS를 위한 큐와 방문 체크 맵
	std::queue<std::pair<Vector2, int>> q;
	std::set<std::pair<int, int>> visited;

	q.push({ startPos, 0 });
	visited.insert({ (int)startPos.x, (int)startPos.y });

	while (!q.empty())
	{
		Vector2 vCurr = q.front().first;
		int iDist = q.front().second;
		q.pop();

		if (iDist > 0) 
			m_vecMonsterRangeTiles.push_back(vCurr);

		if (iDist >= iRange) 
			continue;

		Vector2 vDirs[] = { {0,1}, {0,-1}, {1,0}, {-1,0} };
		for (const auto& vDir : vDirs)
		{
			Vector2 vNext = vCurr + vDir;
			// 맵 안이고, 갈 수 있는 타일인 경우
			if (m_worldMap->IsWalkable((int)vNext.x, (int)vNext.y))
			{
				if (visited.find({ (int)vNext.x, (int)vNext.y }) == visited.end())
				{
					visited.insert({ (int)vNext.x, (int)vNext.y });
					q.push({ vNext, iDist + 1 });
				}
			}
		}
	}
}

void MainLevel::OnCursorMoved()
{
	m_pFocusedMonster = nullptr;
	m_vecMonsterRangeTiles.clear();

	// 2. 현재 커서 위치에 몬스터가 있는지 확인
	for (auto* pActor : m_vecMonsters)
	{
		if (pActor->GetPosition() == m_vCursorPos)
		{
			m_pFocusedMonster = static_cast<Monster*>(pActor);
			break;
		}
	}

	// 3. 몬스터를 찾았다면 이동 반경 계산
	if (m_pFocusedMonster)
	{
		CalcMonsterMoveRange(m_pFocusedMonster);
	}
}
