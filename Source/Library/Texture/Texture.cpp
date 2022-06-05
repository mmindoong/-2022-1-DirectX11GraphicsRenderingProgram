#include "Texture.h"

#include "Texture/WICTextureLoader.h"
#include "Texture/DDSTextureLoader.h"

namespace library
{
    ComPtr<ID3D11SamplerState> Texture::s_samplers[static_cast<size_t>(eTextureSamplerType::COUNT)];
    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Texture::Texture

      Summary:  Constructor

      Args:     const std::filesystem::path& textureFilePath
                  Path to the texture to use

      Modifies: [m_filePath, m_textureRV, m_samplerLinear].
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Texture::Texture definition (remove the comment)
    --------------------------------------------------------------------*/
    Texture::Texture(_In_ const std::filesystem::path& filePath, _In_opt_ eTextureSamplerType textureSamplerType)
        : m_filePath(filePath)
        , m_textureSamplerType(textureSamplerType)
        , m_textureRV(nullptr)
    {
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Texture::Initialize

      Summary:  Initializes the texture

      Args:     ID3D11Device* pDevice
                  The Direct3D device to create the buffers
                ID3D11DeviceContext* pImmediateContext
                  The Direct3D context to set buffers

      Modifies: [m_textureRV, m_samplerLinear].
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Texture::Initialize definition (remove the comment)
    --------------------------------------------------------------------*/
    HRESULT Texture::Initialize(_In_ ID3D11Device* pDevice, _In_ ID3D11DeviceContext* pImmediateContext)
    {
        //it will load the image file to the texture resource view
        HRESULT hr = S_OK;

        hr = CreateWICTextureFromFile(pDevice, pImmediateContext, m_filePath.c_str(), nullptr, m_textureRV.GetAddressOf());
        if (FAILED(hr))
        {
            hr = CreateDDSTextureFromFile(pDevice, m_filePath.c_str(), nullptr, m_textureRV.GetAddressOf());
            if (FAILED(hr))
            {
                OutputDebugString(L"Can't load texture from \"");
                OutputDebugString(m_filePath.c_str());
                OutputDebugString(L"\n");
                return hr;
            }
        }

        // Create the Trilinear Wrap
        if (!s_samplers[static_cast<size_t>(eTextureSamplerType::TRILINEAR_WRAP)].Get()) //Check whether the sampler type objects are nullptr
        //If they are nullptr, that means this is the first call to the method, thus initialize the sampler type objects
        {
            D3D11_SAMPLER_DESC samplerStateDesc;
            ZeroMemory(&samplerStateDesc, sizeof(samplerStateDesc));
            samplerStateDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
            samplerStateDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
            samplerStateDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
            samplerStateDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
            samplerStateDesc.MinLOD = -FLT_MAX;
            samplerStateDesc.MaxLOD = FLT_MAX;
            samplerStateDesc.MipLODBias = 0.0f;
            samplerStateDesc.MaxAnisotropy = 1;
            samplerStateDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;

            hr = pDevice->CreateSamplerState(&samplerStateDesc, s_samplers[static_cast<size_t>(eTextureSamplerType::TRILINEAR_WRAP)].GetAddressOf());
            if (FAILED(hr))
            {
                return hr;
            }

        }

        // Create the Trilinear Clamp
        if (!s_samplers[static_cast<size_t>(eTextureSamplerType::TRILINEAR_CLAMP)].Get()) //Check whether the sampler type objects are nullptr
        //If they are nullptr, that means this is the first call to the method, thus initialize the sampler type objects
        {
            D3D11_SAMPLER_DESC samplerStateDesc;
            ZeroMemory(&samplerStateDesc, sizeof(samplerStateDesc));
            samplerStateDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
            samplerStateDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
            samplerStateDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
            samplerStateDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
            samplerStateDesc.MinLOD = -FLT_MAX;
            samplerStateDesc.MaxLOD = FLT_MAX;
            samplerStateDesc.MipLODBias = 0.0f;
            samplerStateDesc.MaxAnisotropy = 1;
            samplerStateDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;

            hr = pDevice->CreateSamplerState(&samplerStateDesc, s_samplers[static_cast<size_t>(eTextureSamplerType::TRILINEAR_CLAMP)].GetAddressOf());
            if (FAILED(hr))
            {
                return hr;
            }
        }
        if (FAILED(hr))
        {
            return hr;
        }

        return S_OK;


    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Texture::GetTextureResourceView

      Summary:  Constructor

      Returns:  ComPtr<ID3D11ShaderResourceView>&
                  Shader resource view
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    ComPtr<ID3D11ShaderResourceView>& Texture::GetTextureResourceView()
    {
        return m_textureRV;
    }


    eTextureSamplerType Texture::GetSamplerType() const
    {
        return m_textureSamplerType;
    }

}