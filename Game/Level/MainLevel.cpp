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

#include "Actor/Player.h"
#include "Actor/Monster.h"

using namespace Wannabe;

MainLevel::MainLevel()
{
}

MainLevel::~MainLevel()
{
	m_vecMonsters.clear();
}

void MainLevel::BeginPlay()
{
	super::BeginPlay();
}

void MainLevel::Tick(float fDeltaTime)
{
	super::Tick(fDeltaTime);
	m_fTickCollisionTime += fDeltaTime * 0.5f;

	switch (m_eFieldPhase)
	{
	case FieldState::PlayerTurn:      
		Phase_PlayerTurn(); 
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
	if (m_bIsRangeCalculated)
	{
		for (const auto& tilePos : m_vecMoveRangeTiles)
		{
			Vector2 screenPos = tilePos - renderSys.GetCamera().GetPosition();
			renderSys.GetWorldCanvas().DrawTxt(screenPos.x, screenPos.y, L"\u2592", Color::Cyan);
		}
	}

	super::Draw(renderSys);

	// 2. 커서
	if (m_eFieldPhase != FieldState::BattleTransition)
	{
		Vector2 cursorScreenPos = renderSys.GetCamera().WorldToScreen(m_vCursorPos);
		renderSys.GetWorldCanvas().DrawTxt((int)cursorScreenPos.x, (int)cursorScreenPos.y, L"\u25A3", Color::BrightLime);
	}

	// 3. 전환 연출 (최상단)
	if (m_eFieldPhase == FieldState::BattleTransition)
	{
		auto& uiCanvas = renderSys.GetUICanvas();
		float progress = m_fTransitionTimer / m_fTransitionDuration;
		const std::wstring& fillStr = (progress < 0.5f) ? L"\u2592" : L"\u2588";

		if (m_bShowFlash)
		{
			uiCanvas.Clear();
		}
		else
		{
			uiCanvas.Clear();
			uiCanvas.DrawTxt(uiCanvas.GetWidth() / 2 - 7, uiCanvas.GetHeight() / 2, L"!! ENCOUNTER !!", Color::Red);
		}
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

		if (pData->m_NextWorldPos == Vector2::Zero)
			pData->m_NextWorldPos = Vector2(30, 11); // 포탈 근처
	}

	m_worldMap->LoadFromFile(mapFileName);

	//player stat
	m_pPlayer = new Player(1, pData->m_NextWorldPos);
	if (m_pPlayer->GetComponent<StatComponent>())
	{
		m_pPlayer->GetComponent<StatComponent>()->SetStatByData(pData->m_PlayerStat);
	}
	AddNewActor(m_pPlayer);
	m_vecPlayers.emplace_back(m_pPlayer);

	m_vecMonsters.clear();
	for (const auto& pos : spawnPoints)
	{
		Monster* pMonster = new Monster(Util::Random(1, 4)); // 랜덤 종류
		pMonster->SetPosition(pos);
		AddNewActor(pMonster);
		m_vecMonsters.emplace_back(pMonster);
	}

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

	if (pData->m_pEncounteredEnemy != nullptr)
	{
		auto it = std::find(m_vecMonsters.begin(), m_vecMonsters.end(), 
			pData->m_pEncounteredEnemy);

		// 몬스터 목록에서 제거 요청
		if (it != m_vecMonsters.end())
		{
			(*it)->Destroy(); // 필드에서 삭제
			m_vecMonsters.erase(it);
		}

		// 데이터 초기화
		pData->m_pEncounteredEnemy = nullptr;
		pData->m_vecBattleMonsters.clear();
	}
}

void MainLevel::OnExitLevel(RunGameData* pData)
{
	if (pData == nullptr || m_pPlayer == nullptr)
		return;

	pData->m_NextWorldPos = m_pPlayer->GetPosition();
	// 현재 스탯(HP, Exp 등) 백업은 Battle 종료 시나 특정 시점에 수행할 수도 있음
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
			if (pRunData->m_CurLevelId == 2)
			{
				pRunData->m_CurLevelId = 1;
				pRunData->m_NextWorldPos = Vector2(2, 2); // 던전 시작점
			}
			else
			{
				pRunData->m_CurLevelId = 2;
				pRunData->m_NextWorldPos = Vector2(45, 11); // 포탈 앞
			}
			Game::Get().RequestChangeLevel(MainLevel::TypeIdClass());
		}
	}
}

void MainLevel::Phase_PlayerTurn()
{
	if (!m_bIsRangeCalculated)
		CalcMoveRange();

	// 2. 커서 이동 제어
	if (Input::Get().GetKeyDown(VK_UP))    m_vCursorPos.y--;
	if (Input::Get().GetKeyDown(VK_DOWN))  m_vCursorPos.y++;
	if (Input::Get().GetKeyDown(VK_LEFT))  m_vCursorPos.x--;
	if (Input::Get().GetKeyDown(VK_RIGHT)) m_vCursorPos.x++;

	// 3. 선택 (Enter 키)
	if (Input::Get().GetKeyDown(VK_RETURN))
	{
		// 커서 위치가 이동 가능 범위 내에 있는지 확인
		auto it = std::find(m_vecMoveRangeTiles.begin(), m_vecMoveRangeTiles.end(), m_vCursorPos);

		if (it != m_vecMoveRangeTiles.end() || m_vCursorPos == m_pPlayer->GetPosition())
		{
			// 만약 현재 위치라면 이동 생략하고 바로 다음 단계로
			if (m_vCursorPos == m_pPlayer->GetPosition())
			{
				m_eFieldPhase = FieldState::EnemyTurn; // 혹은 메뉴 오픈
				return;
			}

			// A*를 이용해 실제 이동 경로 생성
			m_vecPath = m_worldMap->FindPath(m_pPlayer->GetPosition(), m_vCursorPos);

			if (!m_vecPath.empty())
			{
				m_eFieldPhase = FieldState::Move;
				m_bIsRangeCalculated = false; // 이동 완료 후 다음 턴을 위해 초기화
			}
		}
	}
}

void MainLevel::Phase_Move(float fDeltaTime)
{
	if (m_vecPath.empty())
	{
		CheckMonsterEncounter();

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
	for (Actor* pActor : m_vecMonsters)
	{
		Monster* pMonster = static_cast<Monster*>(pActor);
		if (pMonster == nullptr || pMonster->IsDestroyRequested())
		{
			continue;
		}

		// 플레이어와의 직선 거리 계산
		Vector2 temp;
		float fDist = temp.Distance(pMonster->GetPosition(), m_pPlayer->GetPosition());

		// 1. 시야(SightRange) 내에 플레이어가 있을 때만 추격 시작
		if (fDist <= (float)pMonster->GetSightRange())
		{
			// 플레이어 위치까지의 전체 경로 계산
			std::vector<Vector2> fullPath = m_worldMap->FindPath(pMonster->GetPosition(), m_pPlayer->GetPosition());

			// 경로가 존재하고, 플레이어 바로 옆칸까지만 이동하도록 조절 (공격 거리 고려)
			if (fullPath.size() > 1)
			{
				// 이동력(MoveRange)만큼만 경로 잘라내기 (+1은 현재 위치 포함 기준)
				size_t iLimit = (size_t)pMonster->GetMoveRange() + 1;
				if (fullPath.size() > iLimit)
				{
					fullPath.resize(iLimit);
				}

				// 마지막 목적지가 플레이어와 겹치지 않게 (겹침 방지 로직)
				if (!fullPath.empty() && fullPath.back() == m_pPlayer->GetPosition())
				{
					fullPath.pop_back();
				}

				pMonster->SetMovePath(fullPath);
			}
		}
	}

	// 첫 번째 몬스터부터 이동 연출을 시작하기 위해 인덱스 초기화
	m_iCurEnemyIdx = 0;
	m_eFieldPhase = FieldState::EnemyMove;
}

void MainLevel::Phase_EnemyMove(float fDeltaTime)
{
	// 모든 몬스터의 이동 처리가 끝났다면 플레이어 턴으로 복귀
	if (m_iCurEnemyIdx >= (int)m_vecMonsters.size())
	{
		m_eFieldPhase = FieldState::PlayerTurn;
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
	if (m_fMoveTimer >= 0.15f)
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
		if (pEnemy->IsDestroyRequested()) continue;

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
			if (pRunData->m_vecBattleMonsters.size() >= 4) break; // 최대 4마리 제한
		}
	}

	// 2. 필드 상태를 연출 상태로 전환
	pRunData->m_pEncounteredEnemy = pTarget;
	m_eFieldPhase = FieldState::BattleTransition;
	m_fTransitionTimer = 0.0f;
	m_bShowFlash = true;
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

void MainLevel::UpdateCursorInput()
{
	if (Input::Get().GetKeyDown(VK_UP))    m_vCursorPos.y -= 1;
	if (Input::Get().GetKeyDown(VK_DOWN))  m_vCursorPos.y += 1;
	if (Input::Get().GetKeyDown(VK_LEFT))  m_vCursorPos.x -= 1;
	if (Input::Get().GetKeyDown(VK_RIGHT)) m_vCursorPos.x += 1;

	if (Input::Get().GetKeyDown(VK_RETURN))
	{
		Vector2 cursorGridPos = m_vCursorPos;

		// 선택한 커서 위치가 미리 계산한 이동 범위 안에 있는지 확인
		auto it = std::find(m_vecMoveRangeTiles.begin(), m_vecMoveRangeTiles.end(), cursorGridPos);
		if (it != m_vecMoveRangeTiles.end() || cursorGridPos == m_pPlayer->GetPosition())
		{
			m_pPlayer->MoveTo(cursorGridPos);
			m_bIsRangeCalculated = false;
			// 이동 페이즈로 전환
			m_eFieldPhase = FieldState::Move;
		}
	}
}