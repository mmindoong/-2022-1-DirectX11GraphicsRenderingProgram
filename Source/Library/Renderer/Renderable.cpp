#include "Renderer/Renderable.h"
#include "Texture/DDSTextureLoader.h"

namespace library
{

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
       Method:   Renderable::Renderable

       Summary:  Constructor

       Args:     const XMFLOAT4* outputColor
                   Default color of the renderable

       Modifies: [m_vertexBuffer, m_indexBuffer, m_constantBuffer,
                  m_textureRV, m_samplerLinear, m_vertexShader,
                  m_pixelShader, m_textureFilePath, m_outputColor,
                  m_world].
     M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    Renderable::Renderable(_In_ const XMFLOAT4& outputColor)
        : m_vertexBuffer(nullptr)
        , m_indexBuffer(nullptr)
        , m_constantBuffer(nullptr)
        , m_aMeshes(std::vector<BasicMeshEntry>())
        , m_aMaterials(std::vector<Material>())
        , m_vertexShader(nullptr)
        , m_pixelShader(nullptr)
        , m_outputColor(outputColor)
        , m_world(XMMatrixIdentity())
        , m_padding()
    {
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderable::initialize

      Summary:  Initializes the buffers and the world matrix

      Args:     ID3D11Device* pDevice
                  The Direct3D device to create the buffers
                ID3D11DeviceContext* pImmediateContext
                  The Direct3D context to set buffers

      Modifies: [m_vertexBuffer, m_indexBuffer, m_constantBuffer, 
                  m_world].

      Returns:  HRESULT
                  Status code
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    HRESULT Renderable::initialize(_In_ ID3D11Device* pDevice, _In_ ID3D11DeviceContext* pImmediateContext)
    {
        HRESULT hr = S_OK;

        // Create vertex buffer
        D3D11_BUFFER_DESC bd = {};
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.ByteWidth = sizeof(SimpleVertex) * GetNumVertices(); //renderable이 호출할 것이 아님.
        bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bd.CPUAccessFlags = 0;

        D3D11_SUBRESOURCE_DATA InitData = {};
        InitData.pSysMem = getVertices();
        hr = pDevice->CreateBuffer(&bd, &InitData, m_vertexBuffer.GetAddressOf());
        if (FAILED(hr))
            return hr;

        // Create index buffer
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.ByteWidth = sizeof(WORD) * GetNumIndices();        // 36 vertices needed for 12 triangles in a triangle list
        bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
        bd.CPUAccessFlags = 0;
        InitData.pSysMem = getIndices();
        hr = pDevice->CreateBuffer(&bd, &InitData, m_indexBuffer.GetAddressOf());
        if (FAILED(hr))
            return hr;
       
        //Create the world matrix constant buffers : CBChangesEveryFrame
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.ByteWidth = sizeof(CBChangesEveryFrame);
        bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        bd.CPUAccessFlags = 0;
        bd.MiscFlags = 0;
        bd.StructureByteStride = 0;
        hr = pDevice->CreateBuffer(&bd, nullptr, m_constantBuffer.GetAddressOf());
        if (FAILED(hr))
            return hr;

        
        return S_OK;
    }


    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderable::SetVertexShader

      Summary:  Sets the vertex shader to be used for this renderable 
                object

      Args:     const std::shared_ptr<VertexShader>& vertexShader
                  Vertex shader to set to

      Modifies: [m_vertexShader].
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    void Renderable::SetVertexShader(_In_ const std::shared_ptr<VertexShader>& vertexShader)
    {
        m_vertexShader = vertexShader;
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderable::SetPixelShader

      Summary:  Sets the pixel shader to be used for this renderable
                object

      Args:     const std::shared_ptr<PixelShader>& pixelShader
                  Pixel shader to set to

      Modifies: [m_pixelShader].
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    void Renderable::SetPixelShader(_In_ const std::shared_ptr<PixelShader>& pixelShader)
    {
        m_pixelShader = pixelShader;
    }


    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderable::GetVertexShader

      Summary:  Returns the vertex shader

      Returns:  ComPtr<ID3D11VertexShader>&
                  Vertex shader. Could be a nullptr
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    ComPtr<ID3D11VertexShader>& Renderable::GetVertexShader()
    {
        return m_vertexShader->GetVertexShader();
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderable::GetPixelShader

      Summary:  Returns the vertex shader

      Returns:  ComPtr<ID3D11PixelShader>&
                  Pixel shader. Could be a nullptr
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    ComPtr<ID3D11PixelShader>& Renderable::GetPixelShader()
    {
        return m_pixelShader->GetPixelShader();
    }


    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderable::GetVertexLayout

      Summary:  Returns the vertex input layout

      Returns:  ComPtr<ID3D11InputLayout>&
                  Vertex input layout
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    ComPtr<ID3D11InputLayout>& Renderable::GetVertexLayout()
    {
        return m_vertexShader->GetVertexLayout();
    }


    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderable::GetVertexBuffer

      Summary:  Returns the vertex buffer

      Returns:  ComPtr<ID3D11Buffer>&
                  Vertex buffer
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    ComPtr<ID3D11Buffer>& Renderable::GetVertexBuffer()
    {
        return m_vertexBuffer;
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderable::GetIndexBuffer

      Summary:  Returns the index buffer

      Returns:  ComPtr<ID3D11Buffer>&
                  Index buffer
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    ComPtr<ID3D11Buffer>& Renderable::GetIndexBuffer()
    {
        return m_indexBuffer;
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderable::GetConstantBuffer

      Summary:  Returns the constant buffer

      Returns:  ComPtr<ID3D11Buffer>&
                  Constant buffer
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    ComPtr<ID3D11Buffer>& Renderable::GetConstantBuffer()
    {
        return m_constantBuffer;
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderable::GetWorldMatrix

      Summary:  Returns the world matrix

      Returns:  const XMMATRIX&
                  World matrix
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    const XMMATRIX& Renderable::GetWorldMatrix() const
    {
        return m_world;
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderable::GetMaterial

      Summary:  Returns a material at given index

      Returns:  const Material&
                  Material at given index
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    const Material& Renderable::GetMaterial(UINT uIndex) const
    {
        //If the index exceeds the size of the material vector, then create an assertion failure and kill the application.
        assert(uIndex < m_aMaterials.size());

        return m_aMaterials[uIndex];
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
     Method:   Renderable::GetMesh

     Summary:  Returns a basic mesh entry at given index

     Returns:  const Renderable::BasicMeshEntry&
                 Basic mesh entry at given index
   M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    const Renderable::BasicMeshEntry& Renderable::GetMesh(UINT uIndex) const
    {
        //If the index exceeds the size of the mesh vector, then create an assertion failure and kill the application.
        assert(uIndex < m_aMeshes.size());

        return m_aMeshes[uIndex];
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderable::GetOutputColor

      Summary:  Returns the output color

      Returns:  const XMFLOAT4&
                  The output color
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    const XMFLOAT4& Renderable::GetOutputColor() const
    {
        return m_outputColor;
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderable::HasTexture

      Summary:  Returns whether the renderable has texture

      Returns:  BOOL
                  Whether the renderable has texture
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    BOOL Renderable::HasTexture() const
    {
        if (!m_aMaterials.empty())
            return TRUE;
        else
            return FALSE;
    }

    void Renderable::RotateX(_In_ FLOAT angle)
    {
        m_world *= XMMatrixRotationZ(-angle);
    }
    void Renderable::RotateY(_In_ FLOAT angle)
    {
        m_world *= XMMatrixRotationY(-angle);
    }
    void Renderable::RotateZ(_In_ FLOAT angle)
    {
        m_world *= XMMatrixRotationZ(-angle);
    }
    void Renderable::RotateRollPitchYaw(_In_ FLOAT roll, _In_ FLOAT pitch, _In_ FLOAT yaw)
    {
        m_world *= XMMatrixRotationRollPitchYaw(pitch, yaw, roll);
    }
    void Renderable::Scale(_In_ FLOAT scaleX, _In_ FLOAT scaleY, _In_ FLOAT scaleZ)
    {
        m_world *= XMMatrixScaling(scaleX, scaleY, scaleZ);
    }
    void Renderable::Translate(_In_ const XMVECTOR& offset)
    {
        m_world *= XMMatrixTranslationFromVector(offset);
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderable::GetNumMeshes

      Summary:  Returns the number of meshes

      Returns:  UINT
                  Number of meshes
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    UINT Renderable::GetNumMeshes() const
    {
        return static_cast<UINT>(m_aMeshes.size());
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
     Method:   Renderable::GetNumMaterials

     Summary:  Returns the number of materials

     Returns:  UINT
                 Number of materials
   M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    UINT Renderable::GetNumMaterials() const
    {
        return static_cast<UINT>(m_aMaterials.size());
    }
}
