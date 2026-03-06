#pragma once
#include <memory>

#include "Common/Common.h"
#include "Render/RenderSystem.h"
#include "Render/Renderer.h"

namespace Wannabe
{
	class Input;
	class Level;

	class WANNABE_API Engine
	{
		struct EngineSetting
		{
			float framerate = 0.0f; //프레임 속도
			int width = 0; 
			int height = 0;
		};

	public:
		Engine();
		virtual ~Engine();

		void Run();
		
		void QuitEngine();
		
		void SetNewLevel(class Level* newLevel);

		static Engine& Get();

		inline int GetWidth() const { return m_EngineSetting.width; }
		inline int GetHeight() const { return m_EngineSetting.height; }
		inline EngineSetting GetSetting() const { return m_EngineSetting; }
		RenderSystem& GetRenderSystem() { return *m_RenderSystem; }

	protected:
		void Shutdown(); //clear
		void LoadEngineSetting(); //Config

		void BeginPlay();
		void Tick(float fDeltaTime);
		void Draw();
		virtual void OnFrameEnd() = 0;

	protected:
		bool m_bisQuit = false; // 엔진 종료 플래그

		EngineSetting m_EngineSetting; //엔진 설정 구조체

		Input* m_pInput = nullptr; // 입력 관리자
		Level* m_pMainLevel = nullptr; //메인 레벨
		
		std::unique_ptr<Renderer> m_Renderer; //콘솔 출력
		std::unique_ptr<RenderSystem> m_RenderSystem; //렌더 시스템

		static Engine* instance; // 전역 변수
	};
}