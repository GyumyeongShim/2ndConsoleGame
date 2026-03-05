#include "Camera.h"

using namespace Wannabe;

Vector2 Camera::WorldToScreen(const Vector2& worldPos) const
{
    return worldPos - m_vPos;
}

void Camera::SetFollowMode(bool bFollow)
{
    m_eCameraMode = bFollow ? CameraMode::Follow: CameraMode::Fixed;
}

void Camera::SetFollowTarget(const Vector2* target)
{
    m_pTarget = target;
}

void Camera::Update()
{
    if (m_eCameraMode == CameraMode::Follow && m_pTarget)
    {
        int halfW = m_iWidth / 2;
        int halfH = m_iHeight / 2;

        m_vPos.x = m_pTarget->x - halfW;
        m_vPos.y = m_pTarget->y - halfH;
    }
}