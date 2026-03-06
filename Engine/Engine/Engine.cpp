#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <iostream>

#include "Engine.h"
#include "Level/Level.h"
#include "Core/Input.h"
#include "Util/Utill.h"
#include "Manager/UIManager.h"

namespace Wannabe
{
	Engine* Engine::instance = nullptr;

	BOOL WINAPI ConsoleHandler(DWORD ctrlType) 
	{
		if (ctrlType == CTRL_CLOSE_EVENT)
		{
			Engine::Get().QuitEngine();
			return TRUE;
		}

		return FALSE;
	}

	Engine::Engine()
	{
		instance = this;

		LoadEngineSetting();

		// 객체 맴버는 이니셜라이저 리스트로만, 그러나 render는 설정값이 필요.
		// rawpointer or smartpointer 사용이 필요하다
		m_Renderer = std::make_unique<Renderer>(Vector2(m_EngineSetting.width, m_EngineSetting.height));

		m_RenderSystem = std::make_unique<RenderSystem>(*m_Renderer);

		//input manager, system이 되려면 다양하게 대응되야함
		m_pInput = new Input();

		SetConsoleCtrlHandler(ConsoleHandler, TRUE);

		//커서 끄기
		Util::TurnOffCursor();
		Util::SetRandomSeed();
	}

	Engine::~Engine()
	{
		m_pMainLevel = nullptr;
		SafeDelete(m_pInput);
	}

	void Engine::Run()
	{
		LARGE_INTEGER frequency; //정밀도 ms
		QueryPerformanceFrequency(&frequency);

		int64_t CurrentTime = 0;
		int64_t PreviousTime = 0;

		LARGE_INTEGER time;
		QueryPerformanceCounter(&time); //단위가 하드웨어의 클락으로 표기됨

		//엔진 시작 직전 두 시간값을 같게 맞춤, 프레임 초기화
		CurrentTime = time.QuadPart;
		PreviousTime = CurrentTime;

		//기준 프레임 설정 (초 단위)
		//float targetFrameRate = 120.0f;
		//float oneFrameTime = 1.0f / targetFrameRate;
		m_EngineSetting.framerate = m_EngineSetting.framerate == 0.0f ? 60.0f : m_EngineSetting.framerate;
		float oneFrameTime = 1.0f / m_EngineSetting.framerate;

		while (true)
		{
			//현재 시간 구하기
			QueryPerformanceCounter(&time);
			CurrentTime = time.QuadPart;

			//프레임 시간 계산
			float fDeltaTime = static_cast<float>(CurrentTime - PreviousTime);
			//초 단위 변환
			fDeltaTime = fDeltaTime / static_cast<float>(frequency.QuadPart);

			if (fDeltaTime >= oneFrameTime)
			{
				PreviousTime = CurrentTime;
				if (m_bIsShutdown)
				{
					// 종료 연출 로직
					m_fShutdownTimer -= fDeltaTime;

					m_Renderer->BeginFrame();
					m_RenderSystem->BeginFrame();
					DrawShutdownScreen();

					m_RenderSystem->RenderFrame();

					if (m_fShutdownTimer <= 0.0f)
						break;
				}
				else
				{
					m_pInput->ProcessInput();

					m_Renderer->BeginFrame();
					m_RenderSystem->BeginFrame();

					BeginPlay();
					Tick(fDeltaTime);

					m_RenderSystem->GetCamera().Update();

					Draw();

					m_RenderSystem->RenderFrame();

					m_pInput->SavePreviousInputStates();

					if (m_pMainLevel)
					{
						m_pMainLevel->ProcessAddAndDestroyActors();
					}

					OnFrameEnd();

					if (m_bisQuit == true)
						m_bIsShutdown = true;
				}
			}

		}

		Shutdown();
	}
	
	void Engine::QuitEngine()
	{
		m_bisQuit = true;
	}

	void Engine::SetNewLevel(Level* newLevel)
	{
		if (!newLevel)
			return;

		m_pMainLevel = newLevel;
	}

	Engine& Engine::Get()
	{
		//예외처리
		if (!instance)
		{
			// Silent is vioent
			std::cout << "Error: Engine::Get() instance is null\n";
			__debugbreak();
		}

		return *instance;
	}

	void Engine::Shutdown()
	{
		Util::TurnOnCursor();
	}

	void Engine::LoadEngineSetting()
	{
		FILE* file = nullptr;
		fopen_s(&file, "../Config/Setting.txt", "rt");
		if (file == nullptr)
		{
			std::cout << "Engine Setting File Open Failed.\n";
			__debugbreak();
			return;
		}

		char buffer[2048] = {};
		size_t readSize = fread_s(buffer, sizeof(buffer), sizeof(char), 2048, file);

		// fread는 문자열 끝을 보장하지 않음
		buffer[readSize] = '\0';

		char* context = nullptr;
		char* token = strtok_s(buffer, "\n", &context);

		while (token)
		{
			// Windows 개행(\r\n) 대응
			token[strcspn(token, "\r")] = '\0';

			// 빈 줄 스킵
			if (strlen(token) == 0)
			{
				token = strtok_s(nullptr, "\n", &context);
				continue;
			}

			char key[64] = {};
			char value[64] = {};

			// 공백 상관없이 = 기준 파싱
			if (sscanf_s(token, " %63[^=]= %63s",
				key, (unsigned)_countof(key),
				value, (unsigned)_countof(value)) == 2)
			{
				// key 뒤쪽 공백 제거
				key[strcspn(key, " \t")] = '\0';

				if (strcmp(key, "framerate") == 0)
					m_EngineSetting.framerate = (float)atof(value);

				else if (strcmp(key, "width") == 0)
					m_EngineSetting.width = atoi(value);

				else if (strcmp(key, "height") == 0)
					m_EngineSetting.height = atoi(value);
			}

			token = strtok_s(nullptr, "\n", &context);
		}

		fclose(file);
	}

	void Engine::BeginPlay()
	{
		if (m_pMainLevel == nullptr)
			return;

		m_pMainLevel->BeginPlay();
	}

	void Engine::Tick(float fDeltaTime)
	{
		if (m_pMainLevel == nullptr)
			return;

		m_pMainLevel->Tick(fDeltaTime);
	}

	void Engine::Draw()
	{
		if (m_pMainLevel == nullptr)
			return;

		m_pMainLevel->Draw(*m_RenderSystem);
	}

	void Engine::DrawShutdownScreen()
	{
		int screenWidth = m_EngineSetting.width;
		int screenHeight = m_EngineSetting.height;

		std::wstring message = L"THANK YOU FOR PLAYING!";
		std::wstring subMessage = L"Closing the engine...";

		int startX = (screenWidth / 2) - (static_cast<int>(message.length()) / 2);
		int subStartX = (screenWidth / 2) - (static_cast<int>(subMessage.length()) / 2);

		m_RenderSystem->DrawUI(message, Vector2(startX, screenHeight / 2), Color::Cyan);
		m_RenderSystem->DrawUI(subMessage, Vector2(subStartX, (screenHeight / 2) + 1), Color::White);
	}
}