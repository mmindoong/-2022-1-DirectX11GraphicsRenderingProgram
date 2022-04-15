/*+===================================================================
  File:      MAIN.CPP

  Summary:   This application demonstrates creating a Direct3D 11 device

  Origin:    http://msdn.microsoft.com/en-us/library/windows/apps/ff729718.aspx

  Originally created by Microsoft Corporation under MIT License
  � 2022 Kyung Hee University
===================================================================+*/

#include "Common.h"

#include <memory>

#include "Game/Game.h"
#include "Cube/BigCube.h"
#include "Cube/SmallCube.h"
#include "Cube//CustomCube.h"

/*F+F+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  Function: wWinMain

  Summary:  Entry point to the program. Initializes everything and
            goes into a message processing loop. Idle time is used to
            render the scene.

  Args:     HINSTANCE hInstance
              Handle to an instance.
            HINSTANCE hPrevInstance
              Has no meaning.
            LPWSTR lpCmdLine
              Contains the command-line arguments as a Unicode
              string
            INT nCmdShow
              Flag that says whether the main application window
              will be minimized, maximized, or shown normally

  Returns:  INT
              Status code.
-----------------------------------------------------------------F-F*/
INT WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ INT nCmdShow)
{

#ifdef _DEBUG
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    std::unique_ptr<library::Game> game = std::make_unique<library::Game>(L"Game Graphics Programming Assignmnet 01 : Flying Camera");

    std::shared_ptr<library::VertexShader> vertexShader = std::make_shared<library::VertexShader>(L"Shaders/Shaders.fxh", "VS", "vs_5_0");
    if (FAILED(game->GetRenderer()->AddVertexShader(L"MainShader", vertexShader))) 
    {
        return 0;
    }
    
    std::shared_ptr<library::PixelShader> pixelShader = std::make_shared<library::PixelShader>(L"Shaders/Shaders.fxh", "PS", "ps_5_0");
    if (FAILED(game->GetRenderer()->AddPixelShader(L"MainShader", pixelShader)))
    {
        return 0;
    }

    std::filesystem::path p("./seafloor.dds");
    std::filesystem::path p1("./brick.dds");
    std::shared_ptr<BigCube> bigCube = std::make_shared<BigCube>(p);
    if (FAILED(game->GetRenderer()->AddRenderable(L"cube1", bigCube)))
    {
        return E_FAIL;
    }

    std::shared_ptr<SmallCube> smallCube = std::make_shared<SmallCube>(p1);
    if (FAILED(game->GetRenderer()->AddRenderable(L"cube2", smallCube)))
    {
        return E_FAIL;
    }
    std::shared_ptr<CustomCube> customCube = std::make_shared<CustomCube>(p1);
    if (FAILED(game->GetRenderer()->AddRenderable(L"cube3", customCube)))
    {
        return E_FAIL;
    }

    if (FAILED(game->GetRenderer()->SetVertexShaderOfRenderable(L"cube1", L"MainShader")))
    {
        return E_FAIL;
    }
   
    if (FAILED(game->GetRenderer()->SetVertexShaderOfRenderable(L"cube2", L"MainShader")))
    {
        return E_FAIL;
    }
    if (FAILED(game->GetRenderer()->SetVertexShaderOfRenderable(L"cube3", L"MainShader")))
    {
        return E_FAIL;
    }
    if (FAILED(game->GetRenderer()->SetPixelShaderOfRenderable(L"cube1", L"MainShader")))
    {
        return E_FAIL;
    }
    if (FAILED(game->GetRenderer()->SetPixelShaderOfRenderable(L"cube2", L"MainShader")))
    {
        return E_FAIL;
    }
    if (FAILED(game->GetRenderer()->SetPixelShaderOfRenderable(L"cube3", L"MainShader")))
    {
        return E_FAIL;
    }

    if (FAILED(game->Initialize(hInstance, nCmdShow)))
    {
        return 0;
    }

    return game->Run();
    
}