#pragma once
#include "Core/Common.h"

#include "Math/Vector2.h"
#include "Render/Canvas.h"
#include "Render/Renderer.h"
#include "Render/Camera.h"
#include "Actor/Actor.h"
#include "UI/UI.h"

namespace Wannabe
{
	class WANNABE_API RenderSystem
	{
		struct ShakeEffect
		{
			float fDuration = 0.0f;
			float fIntensity = 0.0f;
			Vector2 offset = { 0, 0 };
		};

		struct ScreenEffect
		{
			bool invert = false;
			bool grayscale = false;
			float shakeIntensity = 0.f;
			float remainingTime = 0.f;
		};

	public:
		enum class RenderMode
		{
			Field,
			Battle
		};

		RenderSystem(Renderer& renderer);

		void BeginFrame();
		void RenderFrame();
		void DrawActor(const Actor& actor); //iorder 0
		void DrawFieldActor(const Actor& actor);
		void DrawBattleActor(const Actor& actor);
		void DrawUI(const std::wstring& txt,Vector2 pos,Color color,int iOrder = 100);
		void DrawDebug(const std::wstring& txt, Vector2 pos, Color color, int iOrder = 1000);
		void DrawEffect(const std::wstring& txt, Vector2 pos, Color color, int iOrder = 10000);

		void Shake(float fDuration, float fIntensity);
		void Update(float fDeltaTime);

		void SetRenderMode(RenderMode eMode) { m_eRenderMode = eMode; }
		void SetFollowMode(bool bIsFollow) { m_bIsFollow = bIsFollow; }

		Canvas& GetWorldCanvas() { return m_WorldCanvas; }
		Canvas& GetUICanvas() { return m_UICanvas; }
		Canvas& GetDebugCanvas() { return m_DebugCanvas; }
		Canvas& GetEffectCanvas() { return m_EffectCanvas; }

		void SetCamera(Camera& pCamera) { m_Camera = pCamera; }
		Camera& GetCamera() { return m_Camera; }

		Vector2 GetScreenSize() const { return m_Renderer.GetScreenSize(); }
	
	private:
		void Composite(const Canvas& canvas); //All canvas ÇÕ¼º
		bool IsInViewport(const Vector2& pos, const Actor& actor) const;

	private:
		Renderer& m_Renderer;

		Canvas m_WorldCanvas;
		Canvas m_UICanvas;
		Canvas m_DebugCanvas;
		Canvas m_EffectCanvas;
		Canvas m_FinalCanvas;

		Camera m_Camera;

		bool m_bIsFollow = true;
		RenderMode m_eRenderMode = RenderMode::Field;

		ShakeEffect m_Shake; // Èçµé¸² »óÅÂ °´Ã¼
	};
}