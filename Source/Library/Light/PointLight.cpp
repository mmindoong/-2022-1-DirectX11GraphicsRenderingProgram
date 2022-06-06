#include "Light/PointLight.h"

#include "Renderer/DataTypes.h"

namespace library
{
    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   PointLight::PointLight

      Summary:  Constructor

      Args:     const XMFLOAT4& position
                  Position of the light
                const XMFLOAT4& color
                  Position of the color

      Modifies: [m_position, m_color, m_eye, m_at,
				 m_up, m_view, m_projection].
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    PointLight::PointLight(_In_ const XMFLOAT4& position, _In_ const XMFLOAT4& color, _In_ FLOAT attenuationDistance)
        : m_position(position)
        , m_color(color)
        , m_attenuationDistance(attenuationDistance)
        , m_up(DEFAULT_UP)
        , m_eye(XMVectorSet(position.x, position.y, position.z, position.w))
        , m_at(XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f))
        {}

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   PointLight::GetPosition

      Summary:  Returns the position of the light

      Returns:  const XMFLOAT4&
                  Position of the light
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    const XMFLOAT4& PointLight::GetPosition() const
    {
        return m_position;
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   PointLight::GetColor

      Summary:  Returns the color of the light

      Returns:  const XMFLOAT4&
                  Color of the light
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    const XMFLOAT4& PointLight::GetColor() const
    {
        return m_color;
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   PointLight::Update

      Summary:  Updates the light every frame

      Args:     FLOAT deltaTime
                  Elapsed time
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    void PointLight::Update(_In_ FLOAT deltaTime)
    {
        UNREFERENCED_PARAMETER(deltaTime);
    }

    FLOAT PointLight::GetAttenuationDistance() const
    {
        return m_attenuationDistance;
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   PointLight::GetViewMatrix

      Summary:  Return the view matrix

      Returns:  XMMATRIX
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: PointLight::GetViewMatrix definition (remove the comment)
    --------------------------------------------------------------------*/
    const XMMATRIX& PointLight::GetViewMatrix() const
    {
        return m_view;
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   PointLight::GetProjectionMatrix

      Summary:  Return the projection matrix

      Returns:  XMMATRIX
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: PointLight::GetProjectionMatrix definition (remove the comment)
    --------------------------------------------------------------------*/
    const XMMATRIX& PointLight::GetProjectionMatrix() const
    {
        return m_projection;
    }


    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   PointLight::Initialize

      Summary:  Initialize the projection matrix

      Args:     UINT uWidth
                UINT uHeight

      Modifies: [m_projection]
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: PointLight::Initialize definition (remove the comment)
    --------------------------------------------------------------------*/
    void PointLight::Initialize(_In_ UINT uWidth, _In_ UINT uHeight)
    {
        // Initialize the projection matrix
        m_projection = XMMatrixPerspectiveFovLH(XM_PIDIV4, static_cast<FLOAT>(uWidth) / static_cast<FLOAT>(uHeight), 0.01f, 1000.0f);
    }

   

}