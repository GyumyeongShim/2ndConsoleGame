#pragma once
#include "Math/Vector2.h"
//screenX = worldX - cameraX
//screenY = worldY - cameraY

namespace Wannabe
{
	class  WANNABE_API Camera
	{
	public:
		enum class CameraMode
		{
			Fixed,
			Follow
		};

		Camera(int iWidth, int iHeight)
			:m_iWidth(iWidth),m_iHeight(iHeight){	}

		void Update();

		void Move(const Vector2& delta) { m_vPos += delta; }

		void SetFollowMode(bool bFollow);
		void SetFollowTarget(const Vector2* target);
		void SetViewport(int w, int h) { m_iHeight = h; m_iWidth = w; }
		void SetPosition(const Vector2& pos) { m_vPos = pos; }

		Vector2 WorldToScreen(const Vector2& worldPos) const;

		const Vector2& GetPosition() const { return m_vPos; }
		int GetWidth() const { return m_iWidth; }
		int GetHeight() const { return m_iHeight; }

	private:
		CameraMode m_eCameraMode = CameraMode::Fixed;
		
		const Vector2* m_pTarget = nullptr;
		Vector2 m_vPos = {0,0};

		int m_iWidth = 0;
		int m_iHeight = 0;
	};
}