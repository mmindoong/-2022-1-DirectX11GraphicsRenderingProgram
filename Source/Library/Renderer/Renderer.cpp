#include "Renderer/Renderer.h"

namespace library
{
    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::Renderer

      Summary:  Constructor

      Modifies: [m_driverType, m_featureLevel, m_d3dDevice, m_d3dDevice1,
                  m_immediateContext, m_immediateContext1, m_swapChain,
                  m_swapChain1, m_renderTargetView, m_vertexShader,
                  m_pixelShader, m_vertexLayout, m_vertexBuffer].
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    Renderer::Renderer()
        : m_driverType(D3D_DRIVER_TYPE_NULL)
        , m_featureLevel(D3D_FEATURE_LEVEL_11_0)
        , m_d3dDevice(nullptr)
        , m_d3dDevice1(nullptr)
        , m_immediateContext(nullptr)
        , m_immediateContext1(nullptr)
        , m_swapChain(nullptr)
        , m_swapChain1(nullptr)
        , m_renderTargetView(nullptr)
        , m_depthStencil(nullptr)
        , m_renderables()
        , m_vertexShaders()
        , m_pixelShaders()
        , m_camera({ 0, 0, 0, 0 })
        , m_projection(XMMatrixIdentity())
        , m_models()
    {
    }

       

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::Initialize

      Summary:  Creates Direct3D device and swap chain

      Args:     HWND hWnd
                  Handle to the window

      Modifies: [m_d3dDevice, m_featureLevel, m_immediateContext,
                  m_d3dDevice1, m_immediateContext1, m_swapChain1,
                  m_swapChain, m_renderTargetView, m_vertexShader,
                  m_vertexLayout, m_pixelShader, m_vertexBuffer].

      Returns:  HRESULT
                  Status code
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    HRESULT Renderer::Initialize(_In_ HWND hWnd)
    {
        HRESULT hr = S_OK;

        RECT rc;
        GetClientRect(hWnd, &rc);

        UINT width = rc.right - rc.left;
        UINT height = rc.bottom - rc.top;


        UINT createDeviceFlags = 0;
#ifdef _DEBUG
        createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

        D3D_DRIVER_TYPE driverTypes[] =
        {
            D3D_DRIVER_TYPE_HARDWARE,
            D3D_DRIVER_TYPE_WARP,
            D3D_DRIVER_TYPE_REFERENCE,
        };
        UINT numDriverTypes = ARRAYSIZE(driverTypes);

        D3D_FEATURE_LEVEL featureLevels[] =
        {
            D3D_FEATURE_LEVEL_11_1,
            D3D_FEATURE_LEVEL_11_0,
            D3D_FEATURE_LEVEL_10_1,
            D3D_FEATURE_LEVEL_10_0,
        };
        UINT numFeatureLevels = ARRAYSIZE(featureLevels);

        for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
        {
            m_driverType = driverTypes[driverTypeIndex];
            hr = D3D11CreateDevice(nullptr,
                m_driverType,
                nullptr,
                createDeviceFlags,
                featureLevels,
                numFeatureLevels,
                D3D11_SDK_VERSION,
                m_d3dDevice.GetAddressOf(),
                &m_featureLevel,
                m_immediateContext.GetAddressOf());

            if (hr == E_INVALIDARG)
            {
                // DirectX 11.0 platforms will not recognize D3D_FEATURE_LEVEL_11_1 so we need to retry without it
                hr = D3D11CreateDevice(nullptr, m_driverType, nullptr, createDeviceFlags, &featureLevels[1], numFeatureLevels - 1,
                    D3D11_SDK_VERSION, m_d3dDevice.GetAddressOf(), &m_featureLevel, m_immediateContext.GetAddressOf());
            }

            if (SUCCEEDED(hr))
                break;
        }
        if (FAILED(hr))
            return hr;

        // Obtain DXGI factory from device (since we used nullptr for pAdapter above)
        ComPtr<IDXGIFactory1> dxgiFactory;
        {
            ComPtr<IDXGIDevice> dxgiDevice;
            ComPtr<IDXGIDevice> dxgiDevice1;
            if (SUCCEEDED(m_d3dDevice.As(&dxgiDevice)))
            {
                ComPtr<IDXGIAdapter> adapter;
                hr = dxgiDevice->GetAdapter(&adapter);
                if (SUCCEEDED(hr))
                {
                    hr = adapter->GetParent(__uuidof(IDXGIFactory1),
                        reinterpret_cast<void**>(dxgiFactory.GetAddressOf()));
                }
            }
        }
        if (FAILED(hr))
            return hr;

        // Create swap chain
        ComPtr<IDXGIFactory2> dxgiFactory2;
        hr = dxgiFactory.As(&dxgiFactory2);

        if (dxgiFactory2)
        {
            // DirectX 11.1 or later
            if (SUCCEEDED(m_d3dDevice.As(&m_d3dDevice1)))
            {
                (void)m_immediateContext.As(&m_immediateContext1);
            }

            DXGI_SWAP_CHAIN_DESC1 sd = {};
            sd.Width = width;
            sd.Height = height;
            sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            sd.SampleDesc.Count = 1;
            sd.SampleDesc.Quality = 0;
            sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            sd.BufferCount = 1;

            hr = dxgiFactory2->CreateSwapChainForHwnd(m_d3dDevice.Get(), hWnd, &sd, nullptr, nullptr, m_swapChain1.GetAddressOf());
            if (SUCCEEDED(hr))
            {
                hr = m_swapChain1.As(&m_swapChain);
            }
        }
        else
        {
            // DirectX 11.0 systems
            DXGI_SWAP_CHAIN_DESC sd = {};
            sd.BufferCount = 1;
            sd.BufferDesc.Width = width;
            sd.BufferDesc.Height = height;
            sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            sd.BufferDesc.RefreshRate.Numerator = 60;
            sd.BufferDesc.RefreshRate.Denominator = 1;
            sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            sd.OutputWindow = hWnd;
            sd.SampleDesc.Count = 1;
            sd.SampleDesc.Quality = 0;
            sd.Windowed = TRUE;

            hr = dxgiFactory->CreateSwapChain(m_d3dDevice.Get(), &sd, m_swapChain.GetAddressOf());
        }

        // Note this tutorial doesn't handle full-screen swapchains so we block the ALT+ENTER shortcut
        dxgiFactory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER);

        if (FAILED(hr))
            return hr;

        // Create a render target view
        ComPtr<ID3D11Texture2D> pBackBuffer;
        hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(pBackBuffer.GetAddressOf()));
        if (FAILED(hr))
            return hr;

        hr = m_d3dDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, m_renderTargetView.GetAddressOf());
        if (FAILED(hr))
            return hr;

        m_immediateContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), nullptr);

        // Setup the viewport
        D3D11_VIEWPORT vp;
        vp.Width = (FLOAT)width;
        vp.Height = (FLOAT)height;
        vp.MinDepth = 0.0f;
        vp.MaxDepth = 1.0f;
        vp.TopLeftX = 0;
        vp.TopLeftY = 0;
        m_immediateContext->RSSetViewports(1, &vp);


        // Create depth stencil texture and the depth stencil view
        // Create depth stencil texture
        D3D11_TEXTURE2D_DESC descDepth = {};
        descDepth.Width = width;
        descDepth.Height = height;
        descDepth.MipLevels = 1;
        descDepth.ArraySize = 1;
        descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        descDepth.SampleDesc.Count = 1; //The default sampler mode, with no anti - aliasing, has a count of 1 and a quality level of 0.
        descDepth.SampleDesc.Quality = 0;
        descDepth.Usage = D3D11_USAGE_DEFAULT; //A resource that requires read and write access by the GPU.
        descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
        descDepth.CPUAccessFlags = 0;
        descDepth.MiscFlags = 0;
        hr = m_d3dDevice->CreateTexture2D(&descDepth, nullptr, m_depthStencil.GetAddressOf());
        if (FAILED(hr))
        {
            return hr;
        }

        // Create depth stencil view
        D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
        descDSV.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
        descDSV.Texture2D.MipSlice = 0;
        hr = m_d3dDevice->CreateDepthStencilView(
            m_depthStencil.Get(), // Depth stencil texture
            &descDSV, // Depth stencil desc
            m_depthStencilView.GetAddressOf()
            );  // [out] Depth stencil view
        if (FAILED(hr))
        {
            return hr;
        }

        // Bind the depth stencil view
        m_immediateContext->OMSetRenderTargets(
            1,          // One rendertarget view
            m_renderTargetView.GetAddressOf(),      // Render target view, created earlier
            m_depthStencilView.Get()
            );     // Depth stencil view for the render target

        // Initialize view matrix and the projection matrix
        // Initialize the view matrix
        m_camera.Initialize(m_d3dDevice.Get());
        m_camera.GetView();

        // Initialize the projection matrix
        m_projection = XMMatrixPerspectiveFovLH(XM_PIDIV2, width / (FLOAT)height, 0.01f, 100.0f);

        // Create the projection matrix constant buffers : CBChangeOnResize
        D3D11_BUFFER_DESC bd = {};
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.ByteWidth = sizeof(CBChangeOnResize);
        bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        bd.CPUAccessFlags = 0;
        bd.MiscFlags = 0;
        bd.StructureByteStride = 0;
        hr = m_d3dDevice->CreateBuffer(&bd, nullptr, m_cbChangeOnResize.GetAddressOf());
        if (FAILED(hr))
            return hr;

        CBChangeOnResize cbChangeOnResize;
        cbChangeOnResize.Projection = XMMatrixTranspose(m_projection);
        m_immediateContext->UpdateSubresource(m_cbChangeOnResize.Get(), 0, nullptr, &cbChangeOnResize, 0, 0);

        //Initialize the CBLights constant buffer
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.ByteWidth = sizeof(CBLights);
        bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        bd.CPUAccessFlags = 0;
        bd.MiscFlags = 0;
        bd.StructureByteStride = 0;
        hr = m_d3dDevice->CreateBuffer(&bd, nullptr, m_cbLights.GetAddressOf());
        if (FAILED(hr))
            return hr;

        // Set primitive topology
        m_immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


        // Initialize the shaders, then the renderables
        for (auto it = m_renderables.begin(); it != m_renderables.end(); it++)
        {
            hr = it->second->Initialize(m_d3dDevice.Get(), m_immediateContext.Get());
            if (FAILED(hr))
                return hr;
        }
        for (auto model = m_models.begin(); model != m_models.end(); model++)
        {
            hr = model->second->Initialize(m_d3dDevice.Get(), m_immediateContext.Get());
            if (FAILED(hr))
                return hr;
        }

        for (auto voxel = m_scenes.begin(); voxel != m_scenes.end(); voxel++)
        {
            for (int i = 0; i < voxel->second->GetVoxels().size(); i++)
            {
                voxel->second->GetVoxels()[i]->Initialize(m_d3dDevice.Get(), m_immediateContext.Get());
            }
        }
        for (auto it = m_vertexShaders.begin(); it != m_vertexShaders.end(); it++)
        {
            hr = it->second->Initialize(m_d3dDevice.Get());
            if (FAILED(hr))
                return hr;
        }
        for (auto it = m_pixelShaders.begin(); it != m_pixelShaders.end(); it++)
        {
            hr = it->second->Initialize(m_d3dDevice.Get());
            if (FAILED(hr))
                return hr;
        }

        return S_OK;
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::AddRenderable

      Summary:  Add a renderable object and initialize the object

      Args:     PCWSTR pszRenderableName
                  Key of the renderable object
                const std::shared_ptr<Renderable>& renderable
                  Unique pointer to the renderable object

      Modifies: [m_renderables].

      Returns:  HRESULT
                  Status code.
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    HRESULT Renderer::AddRenderable(_In_ PCWSTR pszRenderableName, _In_ const std::shared_ptr<Renderable>& renderable)
    {
        if (m_renderables.contains(pszRenderableName))
        {
            return E_FAIL;
        }
        else
        {
            //std::unordered_map<std::wstring, std::shared_ptr<Renderable>>
            m_renderables.insert(std::pair<PCWSTR, std::shared_ptr<Renderable>>(pszRenderableName, renderable));
            return S_OK;
        }
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::AddPointLight

      Summary:  Add a point light

      Args:     size_t index
                  Index of the point light
                const std::shared_ptr<PointLight>& pointLight
                  Shared pointer to the point light object

      Modifies: [m_aPointLights].

      Returns:  HRESULT
                  Status code.
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    HRESULT Renderer::AddPointLight(_In_ size_t index, _In_ const std::shared_ptr<PointLight>& pPointLight)
    {
        //When the index exceeds the size of possible lights, it returns E_FAIL
        if (index > NUM_LIGHTS)
        {
            return E_FAIL;
        }
        //else, add the light to designated index
        else
        {
            m_aPointLights[index]= pPointLight;
        }
        return S_OK;
    }

    HRESULT Renderer::AddScene(_In_ PCWSTR pszSceneName, const std::filesystem::path& sceneFilePath)
    {
        std::shared_ptr<Scene> scene = std::make_shared<Scene>(sceneFilePath);
        if (m_scenes.contains(pszSceneName))
        {
            return E_FAIL;
        }
        else
        {
            m_scenes.insert(std::pair<PCWSTR, std::shared_ptr<Scene>>(pszSceneName, scene));
            return S_OK;
        }
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::AddModel

      Summary:  Add a model object

      Args:     PCWSTR pszModelName
                  Key of the model object
                const std::shared_ptr<Model>& pModel
                  Shared pointer to the model object

      Modifies: [m_models].

      Returns:  HRESULT
                  Status code.
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    HRESULT Renderer::AddModel(_In_ PCWSTR pszModelName, _In_ const std::shared_ptr<Model>& pModel)
    {
        if (m_models.contains(pszModelName))
        {
            return E_FAIL;
        }
        else
        {
            m_models.insert(std::pair<PCWSTR, std::shared_ptr<Model>>(pszModelName, pModel));
            return S_OK;
        }
    }



    HRESULT Renderer::SetMainScene(_In_ PCWSTR pszSceneName)
    {
        //해당 name의 scene을 메인으로 두고, 이후에 renderer에서 MainScene의 voxel을 렌더링함.
        if (m_scenes.contains(pszSceneName))
        {
           m_mainScene = m_scenes.at(pszSceneName);
           return S_OK;
        }
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::AddVertexShader

      Summary:  Add the vertex shader into the renderer

      Args:     PCWSTR pszVertexShaderName
                  Key of the vertex shader
                const std::shared_ptr<VertexShader>&
                  Vertex shader to add

      Modifies: [m_vertexShaders].

      Returns:  HRESULT
                  Status code
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    HRESULT Renderer::AddVertexShader(_In_ PCWSTR pszVertexShaderName, _In_ const std::shared_ptr<VertexShader>& vertexShader)
    {
        if (m_vertexShaders.contains(pszVertexShaderName))
        {
            return E_FAIL;
        }
        else
        {
            m_vertexShaders.insert(std::pair<PCWSTR, std::shared_ptr<VertexShader>>(pszVertexShaderName, vertexShader));
            return S_OK;
        }
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::AddPixelShader

      Summary:  Add the pixel shader into the renderer

      Args:     PCWSTR pszPixelShaderName
                  Key of the pixel shader
                const std::shared_ptr<PixelShader>&
                  Pixel shader to add

      Modifies: [m_pixelShaders].

      Returns:  HRESULT
                  Status code
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    HRESULT Renderer::AddPixelShader(_In_ PCWSTR pszPixelShaderName, _In_ const std::shared_ptr<PixelShader>& pixelShader)
    {
        if (m_pixelShaders.contains(pszPixelShaderName))
        {
            return E_FAIL;
        }
        else
        {
            m_pixelShaders.insert(std::pair<PCWSTR, std::shared_ptr<PixelShader>>(pszPixelShaderName, pixelShader));
            return S_OK;
        }
    }


    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::HandleInput

      Summary:  Add the pixel shader into the renderer and initialize it

      Args:     const DirectionsInput& directions
                  Data structure containing keyboard input data
                const MouseRelativeMovement& mouseRelativeMovement
                  Data structure containing mouse relative input data

      Modifies: [m_camera].
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    void Renderer::HandleInput(_In_ const DirectionsInput& directions, _In_ const MouseRelativeMovement& mouseRelativeMovement, _In_ FLOAT deltaTime)
    {
        // The only object to handle input is the camera object
        m_camera.HandleInput(directions, mouseRelativeMovement, deltaTime);

    }


    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::Update

      Summary:  Update the renderables each frame

      Args:     FLOAT deltaTime
                  Time difference of a frame
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    void Renderer::Update(_In_ FLOAT deltaTime)
    {
        for (auto it = m_renderables.begin(); it != m_renderables.end(); it++)
        {
            it->second->Update(deltaTime);
        }
        for (auto it = m_models.begin(); it != m_models.end(); it++)
        {
            it->second->Update(deltaTime);
        }
        
        for (int i = 0; i < NUM_LIGHTS; i++)
        {
            m_aPointLights[i]->Update(deltaTime);
        }
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::Render

      Summary:  Render the frame
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    void Renderer::Render()
    {
        // Clear the backbuffer
        m_immediateContext->ClearRenderTargetView(m_renderTargetView.Get(), Colors::MidnightBlue);

        // Clear the depth buffer to 1.0 (maximum depth)
        m_immediateContext->ClearDepthStencilView(
            m_depthStencilView.Get(),
            D3D11_CLEAR_DEPTH,
            1.0f,
            0);
       
        // Update the Camera Constant buffer
        CBChangeOnCameraMovement cbChangeOnCameraMovement;
        cbChangeOnCameraMovement.View = XMMatrixTranspose(m_camera.GetView());
        XMStoreFloat4(&cbChangeOnCameraMovement.CameraPosition, m_camera.GetEye());
        m_immediateContext->UpdateSubresource(m_camera.GetConstantBuffer().Get(), 0, nullptr, &cbChangeOnCameraMovement, 0, 0);
        m_immediateContext->VSSetConstantBuffers(0u, 1u, m_camera.GetConstantBuffer().GetAddressOf());
        m_immediateContext->PSSetConstantBuffers(0u, 1u, m_camera.GetConstantBuffer().GetAddressOf());

        // Update the Lights Constant buffer 
        CBLights cbLights = {};
        for (UINT i = 0u; i < NUM_LIGHTS; i++)
        {
            cbLights.LightPositions[i] = m_aPointLights[i]->GetPosition();
            cbLights.LightColors[i] = m_aPointLights[i]->GetColor();
            m_immediateContext->UpdateSubresource(m_cbLights.Get(), 0u, nullptr, &cbLights, 0u, 0u);
            m_immediateContext->VSSetConstantBuffers(3u, 1u, m_cbLights.GetAddressOf());
            m_immediateContext->PSSetConstantBuffers(3u, 1u, m_cbLights.GetAddressOf());
        }
    
        // For each renderables Set the vertex buffer, index buffer, input layout
        // For each renderables Update constant buffer
        // For each renderables Render the triangles
        for (auto it = m_renderables.begin(); it != m_renderables.end(); it++)
        {
            UINT stride = sizeof(SimpleVertex);
            UINT offset = 0;
            m_immediateContext->IASetVertexBuffers(0, 1, it->second->GetVertexBuffer().GetAddressOf(), &stride, &offset);
            m_immediateContext->IASetIndexBuffer(it->second->GetIndexBuffer().Get(), DXGI_FORMAT_R16_UINT, 0);
            m_immediateContext->IASetInputLayout(it->second->GetVertexLayout().Get());

            //Set the VS and PS shaders
            m_immediateContext->VSSetShader(it->second->GetVertexShader().Get(), nullptr, 0);
            m_immediateContext->PSSetShader(it->second->GetPixelShader().Get(), nullptr, 0);

            // Set the Projection Constant buffer
            m_immediateContext->VSSetConstantBuffers(1u, 1u, m_cbChangeOnResize.GetAddressOf());

            // Update variables that change once per frame
            CBChangesEveryFrame cbChangesEveryFrame;
            cbChangesEveryFrame.World = XMMatrixTranspose(it->second->GetWorldMatrix());
            cbChangesEveryFrame.OutputColor = it->second->GetOutputColor();
            m_immediateContext->UpdateSubresource(it->second->GetConstantBuffer().Get(), 0, nullptr, &cbChangesEveryFrame, 0, 0);
            m_immediateContext->VSSetConstantBuffers(2u, 1u, it->second->GetConstantBuffer().GetAddressOf());
            m_immediateContext->PSSetConstantBuffers(2u, 1u, it->second->GetConstantBuffer().GetAddressOf());

            if (it->second->HasTexture())
            {
                for (UINT i = 0u; i < it->second->GetNumMeshes(); ++i)
                {
                    UINT materialIndex = it->second->GetMesh(i).uMaterialIndex;

                    m_immediateContext->PSSetShaderResources(0, 1, it->second->GetMaterial(materialIndex).pDiffuse->GetTextureResourceView().GetAddressOf());
                    m_immediateContext->PSSetSamplers(0, 1, it->second->GetMaterial(materialIndex).pDiffuse->GetSamplerState().GetAddressOf());

                    m_immediateContext->DrawIndexed(it->second->GetMesh(i).uNumIndices, it->second->GetMesh(i).uBaseIndex, it->second->GetMesh(i).uBaseVertex);
                }
            }
            else
            {
                m_immediateContext->DrawIndexed(it->second->GetNumIndices(), 0, 0);     
            }
        }
        
        // render the voxels of the main scene
        // 1. For each voxels Set the vertex buffer, index buffer, instance buffer
        // 2. For each voxels Set the input layout
        // 3. For each voxels Update constant buffer
        // 4. For each voxels Set the Shaders and their input(constant buffer etc)
        // 5.  DrawIndexedInstanced
        for (auto voxel = m_scenes.begin(); voxel != m_scenes.end(); voxel++)
        {
            for (int it = 0; it < voxel->second->GetVoxels().size(); it++)
            {
                // 1. The instance buffer is just a second vertex buffer containing different infromation 
                // so it is set on the device at the same time using the same call as the vertex buffer
                // So stride, offset, buffer we now send an arrary of strides, offsets, and buffers to the IASetVertexBuffers call.

                UINT offset = 0;

                //Set the buffer strides
                UINT stride = sizeof(SimpleVertex);
                UINT stride1 = sizeof(InstanceData);

                //Set both the vertex buffer and the instance buffer on the device context in the same call
                //Set the vertex buffer to active in the input assembler so it can be rendered.
                m_immediateContext->IASetVertexBuffers(0, 1, voxel->second->GetVoxels()[it]->GetVertexBuffer().GetAddressOf(), &stride, &offset);
                m_immediateContext->IASetVertexBuffers(1, 1, voxel->second->GetVoxels()[it]->GetInstanceBuffer().GetAddressOf(), &stride1, &offset);

                // Set the index buffer
                m_immediateContext->IASetIndexBuffer(voxel->second->GetVoxels()[it]->GetIndexBuffer().Get(), DXGI_FORMAT_R16_UINT, 0);
                //2. Set the vertex input layout
                m_immediateContext->IASetInputLayout(voxel->second->GetVoxels()[it]->GetVertexLayout().Get());

                //3. Update variables that change once per frame
                CBChangesEveryFrame cbChangesEveryFrame;
                cbChangesEveryFrame.World = XMMatrixTranspose(voxel->second->GetVoxels()[it]->GetWorldMatrix());
                cbChangesEveryFrame.OutputColor = voxel->second->GetVoxels()[it]->GetOutputColor();
                m_immediateContext->UpdateSubresource(voxel->second->GetVoxels()[it]->GetConstantBuffer().Get(), 0, nullptr, &cbChangesEveryFrame, 0, 0);

                //4. Set the vertex and pixel shaders that will be used to render voxels
                //Set the new VS and PS shaders
                m_immediateContext->VSSetShader(voxel->second->GetVoxels()[it]->GetVertexShader().Get(),nullptr, 0);
                m_immediateContext->PSSetShader(voxel->second->GetVoxels()[it]->GetPixelShader().Get(), nullptr, 0);

                //Set the ConstantBuffers
                m_immediateContext->VSSetConstantBuffers(1u, 1u, m_cbChangeOnResize.GetAddressOf());
                m_immediateContext->VSSetConstantBuffers(2u, 1u, voxel->second->GetVoxels()[it]->GetConstantBuffer().GetAddressOf());

                m_immediateContext->PSSetConstantBuffers(2u, 1u, voxel->second->GetVoxels()[it]->GetConstantBuffer().GetAddressOf());
                m_immediateContext->DrawIndexedInstanced(voxel->second->GetVoxels()[it]->GetNumIndices(), voxel->second->GetVoxels()[it]->GetNumInstances(), 0u, 0, 0u);
            }
        }
       
        // For each renderables Set the vertex buffer, index buffer, input layout
        // For each renderables Update constant buffer
        // For each renderables Render the triangles
        for (auto it = m_models.begin(); it != m_models.end(); it++)
        {
            UINT aStrides[2] = { static_cast<UINT>(sizeof(SimpleVertex)),
                                static_cast<UINT>(sizeof(AnimationData)) };
            UINT aOffsets[2] = { 0u, 0u };

            ComPtr<ID3D11Buffer> aBuffers[2]
            {
                it->second->GetVertexBuffer().Get(),
                it->second->GetAnimationBuffer().Get()
            };
            m_immediateContext->IASetVertexBuffers(0u, 2u, aBuffers->GetAddressOf(), aStrides, aOffsets);
            m_immediateContext->IASetIndexBuffer(it->second->GetIndexBuffer().Get(), DXGI_FORMAT_R16_UINT, 0);
            m_immediateContext->IASetInputLayout(it->second->GetVertexLayout().Get());

            m_immediateContext->VSSetShader(it->second->GetVertexShader().Get(), nullptr, 0);
            m_immediateContext->PSSetShader(it->second->GetPixelShader().Get(), nullptr, 0);

            m_immediateContext->VSSetConstantBuffers(1u, 1u, m_cbChangeOnResize.GetAddressOf());

            // Update variables that change once per frame
            CBChangesEveryFrame cbChangesEveryFrame;
            cbChangesEveryFrame.World = XMMatrixTranspose(it->second->GetWorldMatrix());
            cbChangesEveryFrame.OutputColor = it->second->GetOutputColor();
            m_immediateContext->UpdateSubresource(it->second->GetConstantBuffer().Get(), 0, nullptr, &cbChangesEveryFrame, 0, 0);
            m_immediateContext->VSSetConstantBuffers(2u, 1u, it->second->GetConstantBuffer().GetAddressOf());
            m_immediateContext->PSSetConstantBuffers(2u, 1u, it->second->GetConstantBuffer().GetAddressOf());

            // Update Skinning constatnt buffer
            // Set the bone transformations in skinning constant buffer
            // Constant Buffer에 마지막에 XMMatrixTranspose 시켜주기!!
            CBSkinning cbSkinning;
            for (UINT i = 0u; i < it->second->GetBoneTransforms().size(); i++)
            {
                cbSkinning.BoneTransforms[i] = XMMatrixTranspose(it->second->GetBoneTransforms()[i]);
            }
            m_immediateContext->UpdateSubresource(it->second->GetSkinningConstantBuffer().Get(), 0u, nullptr, &cbSkinning, 0u, 0u);
            m_immediateContext->VSSetConstantBuffers(4u, 1u, it->second->GetSkinningConstantBuffer().GetAddressOf());
            m_immediateContext->PSSetConstantBuffers(4u, 1u, it->second->GetSkinningConstantBuffer().GetAddressOf());

            if (it->second->HasTexture())
            {
                for (UINT i = 0u; i < it->second->GetNumMeshes(); ++i)
                {
                    UINT materialIndex = it->second->GetMesh(i).uMaterialIndex;

                    m_immediateContext->PSSetShaderResources(0, 1, it->second->GetMaterial(materialIndex).pDiffuse->GetTextureResourceView().GetAddressOf());
                    m_immediateContext->PSSetSamplers(0, 1, it->second->GetMaterial(materialIndex).pDiffuse->GetSamplerState().GetAddressOf());
                    m_immediateContext->DrawIndexed(it->second->GetMesh(i).uNumIndices, it->second->GetMesh(i).uBaseIndex, it->second->GetMesh(i).uBaseVertex);
                }
            }
            else
            {
                m_immediateContext->DrawIndexed(it->second->GetNumIndices(), 0, 0);
            }
        }

        // Present our back buffer to our front buffer
        m_swapChain->Present(0, 0);
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::SetVertexShaderOfRenderable

      Summary:  Sets the vertex shader for a renderable

      Args:     PCWSTR pszRenderableName
                  Key of the renderable
                PCWSTR pszVertexShaderName
                  Key of the vertex shader

      Modifies: [m_renderables].

      Returns:  HRESULT
                  Status code
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    HRESULT Renderer::SetVertexShaderOfRenderable(_In_ PCWSTR pszRenderableName, _In_ PCWSTR pszVertexShaderName)
    {
        if (m_renderables.contains(pszRenderableName))
        {
            m_renderables.at(pszRenderableName)->SetVertexShader(m_vertexShaders.at(pszVertexShaderName));
            return S_OK;
        }
        else
        {
            return E_FAIL;
        }
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::SetPixelShaderOfRenderable

      Summary:  Sets the pixel shader for a renderable

      Args:     PCWSTR pszRenderableName
                  Key of the renderable
                PCWSTR pszPixelShaderName
                  Key of the pixel shader

      Modifies: [m_renderables].

      Returns:  HRESULT
                  Status code
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    HRESULT Renderer::SetPixelShaderOfRenderable(_In_ PCWSTR pszRenderableName, _In_ PCWSTR pszPixelShaderName)
    {
        if (m_renderables.contains(pszRenderableName))
        {
            m_renderables.at(pszRenderableName)->SetPixelShader(m_pixelShaders.at(pszPixelShaderName));
            return S_OK;
        }
        else
        {
            return E_FAIL;
        }
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::GetDriverType

      Summary:  Returns the Direct3D driver type

      Returns:  D3D_DRIVER_TYPE
                  The Direct3D driver type used
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    D3D_DRIVER_TYPE Renderer::GetDriverType() const
    {
        return m_driverType;
    }

    HRESULT Renderer::SetVertexShaderOfScene(_In_ PCWSTR pszSceneName, _In_ PCWSTR pszVertexShaderName)
    {
        if (m_scenes.contains(pszSceneName))
        {
            for (UINT i = 0u; i < m_scenes[pszSceneName]->GetVoxels().size(); ++i)
            {
                m_scenes[pszSceneName]->GetVoxels()[i]->SetVertexShader(m_vertexShaders[pszVertexShaderName]);
            }
            return S_OK;
        }
        else
        {
            return E_FAIL;
        }
    }

    HRESULT Renderer::SetPixelShaderOfScene(_In_ PCWSTR pszSceneName, _In_ PCWSTR pszPixelShaderName)
    {
        if (m_scenes.contains(pszSceneName))
        {
            for (UINT i = 0u; i < m_scenes[pszSceneName]->GetVoxels().size(); ++i)
            {
                m_scenes[pszSceneName]->GetVoxels()[i]->SetPixelShader(m_pixelShaders[pszPixelShaderName]);
            }
            return S_OK;
        }
        else
        {
            return E_FAIL;
        }
    }

    HRESULT Renderer::SetVertexShaderOfModel(_In_ PCWSTR pszModelName, _In_ PCWSTR pszVertexShaderName)
    {
        if (m_models.contains(pszModelName))
        {
            m_models.at(pszModelName)->SetVertexShader(m_vertexShaders.at(pszVertexShaderName));
            return S_OK;
        }
        else
        {
            return E_FAIL;
        }
    }
    HRESULT Renderer::SetPixelShaderOfModel(_In_ PCWSTR pszModelName, _In_ PCWSTR pszPixelShaderName)
    {
        if (m_models.contains(pszModelName))
        {
            m_models.at(pszModelName)->SetPixelShader(m_pixelShaders.at(pszPixelShaderName));
            return S_OK;
        }
        else
        {
            return E_FAIL;
        }
    }
}