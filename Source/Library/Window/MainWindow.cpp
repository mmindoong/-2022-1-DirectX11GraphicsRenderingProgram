#include "Window/MainWindow.h"

namespace library
{
    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   MainWindow::Initialize

      Summary:  Initializes main window

      Args:     HINSTANCE hInstance
                  Handle to the instance
                INT nCmdShow
                    Is a flag that says whether the main application window
                    will be minimized, maximized, or shown normally
                PCWSTR pszWindowName
                    The window name

      Returns:  HRESULT
                  Status code
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/

    HRESULT MainWindow::Initialize(_In_ HINSTANCE hInstance, _In_ INT nCmdShow, _In_ PCWSTR pszWindowName)
    {
        RECT rc = { 0, 0, 800, 600 };
        AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);


        HRESULT hr = initialize(hInstance, nCmdShow, pszWindowName, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr);
        if (FAILED(hr))
        {
            return hr;
        }

        static bool raw_input_initialized = false;
        if (raw_input_initialized == false)
        {
            RAWINPUTDEVICE rid;

            rid.usUsagePage = 0x01; //Mouse
            rid.usUsage = 0x02;
            rid.dwFlags = 0;
            rid.hwndTarget = NULL;

            if (RegisterRawInputDevices(&rid, 1, sizeof(rid)) == FALSE)
            {
                exit(-1);
            }

            raw_input_initialized = true;
        }

        return S_OK;

    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   MainWindow::GetWindowClassName

      Summary:  Returns the name of the window class

      Returns:  PCWSTR
                  Name of the window class
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/

    PCWSTR MainWindow::GetWindowClassName() const { return L"TutorialClass"; }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   MainWindow::HandleMessage

      Summary:  Handles the messages

      Args:     UINT uMessage
                  Message code
                WPARAM wParam
                    Additional data the pertains to the message
                LPARAM lParam
                    Additional data the pertains to the message

      Returns:  LRESULT
                  Integer value that your program returns to Windows
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/

    LRESULT MainWindow::HandleMessage(_In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)
    {
        PAINTSTRUCT ps;
        HDC hdc;

        switch (uMsg)
        {
        case WM_PAINT:
            hdc = BeginPaint(m_hWnd, &ps);
            EndPaint(m_hWnd, &ps);
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        
        case WM_INPUT:
        {

            RECT rc;
            POINT p1, p2;
            RECT rc1;
          
            GetClientRect(m_hWnd, &rc);

            p1.x = rc.left;
            p1.y = rc.top;
            p2.x = rc.right;
            p2.y = rc.bottom;

            ClientToScreen(m_hWnd, &p1);
            ClientToScreen(m_hWnd, &p2);

            GetWindowRect(m_hWnd, &rc1);

            rc.left = p1.x;
            rc.top = rc1.top;
            rc.right = p2.x;
            rc.bottom = p2.y;

            ClipCursor(&rc);

            UINT dwSize = sizeof(RAWINPUT);
            static BYTE lpb[sizeof(RAWINPUT)];

            GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), 
                            RID_INPUT, 
                            lpb, &dwSize, 
                            sizeof(RAWINPUTHEADER));

            if (dwSize > 0)
            {
                RAWINPUT* raw = reinterpret_cast<RAWINPUT*>(lpb);
                if (raw->header.dwType == RIM_TYPEMOUSE)
                {
                    m_mouseRelativeMovement.X = raw->data.mouse.lLastX;
                    m_mouseRelativeMovement.Y = raw->data.mouse.lLastY;
                }
            }
            return DefWindowProc(m_hWnd, uMsg, wParam, lParam); //Need to call DefWindowProc for WM_INPUT messages
        }
        //Keyboard Message
        case WM_KEYDOWN:
        {
            switch (wParam)
            {
                case 0x10: //SHIFT Key
                    m_directions.bDown = true;
                    break;
                case 0x20: //SPACE key
                    m_directions.bUp = true;
                    break;
                case 0x57: //W key
                    m_directions.bFront = true;
                    break;
                case 0x41: //A key
                    m_directions.bLeft = true;
                    break;
                case 0x53: //S key
                    m_directions.bBack = true;
                    break;
                case 0x44: //D key
                    m_directions.bRight = true;
                    break;
            }
            //wParam : 가상 키 코드
            return 0;
        }
        case WM_KEYUP:
        {
            switch (wParam)
            {
            case 0x10: //SHIFT Key
                m_directions.bDown = false;
                break;
            case 0x20: //SPACE key
                m_directions.bUp = false;
                break;
            case 0x57: //W key
                m_directions.bFront = false;
                break;
            case 0x41: //A key
                m_directions.bLeft = false;
                break;
            case 0x53: //S key
                m_directions.bBack = false;
                break;
            case 0x44: //D key
                m_directions.bRight = false;
                break;
           
            }
            return 0;
        }
        default:
            return DefWindowProc(m_hWnd, uMsg, wParam, lParam);
        }

        return 0;
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
     Method:   MainWindow::GetDirections

     Summary:  Returns the keyboard direction input

     Returns:  const DirectionsInput&
                 Keyboard direction input
   M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    const DirectionsInput& MainWindow::GetDirections() const
    {
        return m_directions;
    }

   /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
     Method:   MainWindow::GetMouseRelativeMovement

     Summary:  Returns the mouse relative movement

     Returns:  const MouseRelativeMovement&
                 Mouse relative movement
   M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    const MouseRelativeMovement& MainWindow::GetMouseRelativeMovement() const
    {
        return m_mouseRelativeMovement;
    }

   /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
     Method:   MainWindow::ResetMouseMovement

     Summary:  Reset the mouse relative movement to zero
   M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    void MainWindow::ResetMouseMovement()
    {
        m_mouseRelativeMovement.X = 0;
        m_mouseRelativeMovement.Y = 0;
    }
}
