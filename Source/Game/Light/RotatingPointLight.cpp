#include "Light/RotatingPointLight.h"

/*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
  Method:   RotatingPointLight::RotatingPointLight

  Summary:  Constructor

  Args:     const XMFLOAT4& position
              Position of the light
            const XMFLOAT4& color
              Position of the color
M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
RotatingPointLight::RotatingPointLight(_In_ const XMFLOAT4& position, _In_ const XMFLOAT4& color, FLOAT attenuationDistance)
    : PointLight(position, color, attenuationDistance)
{
}

/*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
  Method:   RotatingPointLight::Update

  Summary:  Rotates the light every frame

  Args:     FLOAT deltaTime
              Elapsed time
M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
void RotatingPointLight::Update(_In_ FLOAT deltaTime)
{
    XMMATRIX rotate = XMMatrixRotationY(-2.0f * deltaTime);
    XMVECTOR position = XMLoadFloat4(&m_position);
    position = XMVector3Transform(position, rotate);
    XMStoreFloat4(&m_position, position);

    // Create the view matrix
    // XMMatrixLookAtLH 카메라의 위치와 타겟의 위치를 입력하여 카메라가 타겟을. 바라보는 방향에 대한 View 행렬을 생성.
    XMVECTOR eye = position; // Eye : current position of the light
    XMVECTOR at = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
    XMVECTOR up = DEFAULT_UP;
    XMMATRIX view = XMMatrixLookAtLH(eye, at, up);
}
