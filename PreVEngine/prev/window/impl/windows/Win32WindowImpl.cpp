#include "Win32WindowImpl.h"

#ifdef VK_USE_PLATFORM_WIN32_KHR

#include "../../../common/Logger.h"

#include <windowsx.h> // Mouse

namespace prev::window::impl::win32 {
namespace {
    // Convert native Win32 keyboard scancode to cross-platform USB HID code.
    const unsigned char WIN32_TO_HID[256] = {
        0, 0, 0, 0, 0, 0, 0, 0, 42, 43, 0, 0, 0, 40, 0, 0, // 16
        225, 224, 226, 72, 57, 0, 0, 0, 0, 0, 0, 41, 0, 0, 0, 0, // 32
        44, 75, 78, 77, 74, 80, 82, 79, 81, 0, 0, 0, 70, 73, 76, 0, // 48
        39, 30, 31, 32, 33, 34, 35, 36, 37, 38, 0, 0, 0, 0, 0, 0, // 64
        0, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, // 80
        19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 0, 0, 0, 0, 0, // 96
        98, 89, 90, 91, 92, 93, 94, 95, 96, 97, 85, 87, 0, 86, 99, 84, // 112
        58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 104, 105, 106, 107, // 128
        108, 109, 110, 111, 112, 113, 114, 115, 0, 0, 0, 0, 0, 0, 0, 0, // 144
        83, 71, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 160
        225, 229, 224, 228, 226, 230, 0, 0, 0, 0, 0, 0, 0, 127, 128, 129, // 176    L/R shift/ctrl/alt  mute/vol+/vol-
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 51, 46, 54, 45, 55, 56, // 192
        53, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 208
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 47, 49, 48, 52, 0, // 224
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 240
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 // 256
    };

    void GetDesktopResolution(uint32_t& horizontal, uint32_t& vertical)
    {
        RECT desktop;
        // Get a handle to the desktop window
        const HWND hDesktop = GetDesktopWindow();
        // Get the size of screen to the variable desktop
        GetWindowRect(hDesktop, &desktop);

        // The top left corner will have coordinates (0,0)
        // and the bottom right corner will have coordinates
        // (horizontal, vertical)
        horizontal = static_cast<uint32_t>(desktop.right);
        vertical = static_cast<uint32_t>(desktop.bottom);
    }

    DWORD GetStyleFlags(bool fullScreen)
    {
        DWORD styleFlags;
        if (fullScreen) {
            styleFlags = WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
        } else {
            styleFlags = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
        }
        return styleFlags;
    }

    DWORD GetStyleExFlags(bool fullScreen)
    {
        DWORD exStyleFlags;
        if (fullScreen) {
            exStyleFlags = WS_EX_APPWINDOW;
        } else {
            exStyleFlags = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
        }
        return exStyleFlags;
    }

    RECT ComputeWindowRect(uint32_t clientAreaWidth, uint32_t clientAreaHeight, bool fullScreen)
    {
        const auto dwStyle{ GetStyleFlags(fullScreen) };
        const auto dwExStyle{ GetStyleExFlags(fullScreen) };

        RECT windowRect{ 0, 0, static_cast<long>(clientAreaWidth), static_cast<long>(clientAreaHeight) };
        AdjustWindowRectEx(&windowRect, dwStyle, FALSE, dwExStyle);
        return windowRect;
    }
} // namespace

Win32WindowImpl::Win32WindowImpl(const prev::core::instance::Instance& instance, const WindowInfo& windowInfo)
    : WindowImpl(instance)
{
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    LOGI("Creating Win32 Window...\n");

    m_hInstance = GetModuleHandle(NULL);

    // Initialize the window class structure:
    WNDCLASSEX winClass;
    winClass.cbSize = sizeof(WNDCLASSEX);
    winClass.style = CS_HREDRAW | CS_VREDRAW;
    winClass.lpfnWndProc = WndProc;
    winClass.cbClsExtra = 0;
    winClass.cbWndExtra = 0;
    winClass.hInstance = m_hInstance;
    winClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    winClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    winClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    winClass.lpszMenuName = NULL;
    winClass.lpszClassName = windowInfo.title.c_str();
    winClass.hInstance = m_hInstance;
    winClass.hIconSm = LoadIcon(NULL, IDI_WINLOGO);
    // Register window class:
    ATOM atom = RegisterClassEx(&winClass);
    assert(atom && "Failed to register the window class.");

    uint32_t screenWidth, screenHeight;
    GetDesktopResolution(screenWidth, screenHeight);

    bool fullScreen = false;
    if (windowInfo.fullScreen) {
        DEVMODE dmScreenSettings;
        memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
        dmScreenSettings.dmSize = sizeof(dmScreenSettings);
        dmScreenSettings.dmPelsWidth = screenWidth;
        dmScreenSettings.dmPelsHeight = screenHeight;
        dmScreenSettings.dmBitsPerPel = 32;
        dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

        if (windowInfo.size.width != static_cast<uint32_t>(screenWidth) && windowInfo.size.height != static_cast<uint32_t>(screenHeight)) {
            if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) == DISP_CHANGE_SUCCESSFUL) {
                fullScreen = true;
            } else {
                MessageBox(NULL, "Fullscreen Mode not supported!\n Switch to window mode?", "Error", MB_OK | MB_ICONERROR);
            }
        }
    }

    m_info.title = windowInfo.title;
    m_info.fullScreen = fullScreen;
    if (fullScreen) {
        m_info.position = {};
        m_info.size = { screenWidth, screenHeight };
    } else {
        m_info.position = { windowInfo.position.x, windowInfo.position.y };
        m_info.size = { windowInfo.size.width, windowInfo.size.height };
    }

    const auto windowRect{ ComputeWindowRect(m_info.size.width, m_info.size.height, fullScreen) };
    const auto dwStyle{ GetStyleFlags(fullScreen) };

    m_hWnd = CreateWindowEx(0, m_info.title.c_str(), m_info.title.c_str(), dwStyle, m_info.position.x, m_info.position.y, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, NULL, NULL, m_hInstance, NULL);
    assert(m_hWnd && "Failed to create a window.");

    ShowWindow(m_hWnd, SW_SHOW);
    SetForegroundWindow(m_hWnd);
    SetFocus(m_hWnd);

    m_eventQueue.Push(OnInitEvent());
}

Win32WindowImpl::~Win32WindowImpl()
{
    DestroyWindow(m_hWnd);
}

void Win32WindowImpl::SetTitle(const std::string& title)
{
    SetWindowText(m_hWnd, title.c_str());
}

void Win32WindowImpl::SetPosition(int32_t x, int32_t y)
{
    if (m_info.fullScreen) {
        return;
    }

    if (m_info.position == Position{ x, y }) {
        return;
    }

    SetWindowPos(m_hWnd, NULL, x, y, 0, 0, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE);

    m_eventQueue.Push(OnMoveEvent(x, y)); // Trigger window moved event
}

void Win32WindowImpl::SetSize(uint32_t w, uint32_t h)
{
    if (m_info.fullScreen) {
        return;
    }

    if (m_info.size == Size{ w, h }) {
        return;
    }

    const auto windowRect{ ComputeWindowRect(w, h, false) };
    SetWindowPos(m_hWnd, NULL, 0, 0, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOMOVE);

    m_eventQueue.Push(OnResizeEvent(w, h)); // Trigger resize event
}

void Win32WindowImpl::SetMouseCursorVisible(bool visible)
{
    m_mouseCursorVisible = visible;

    if (visible) {
        ShowCursor(true);
    } else {
        while (ShowCursor(false) >= 0)
            ;
    }
}

Surface& Win32WindowImpl::CreateSurface()
{
    if (m_vkSurface == VK_NULL_HANDLE) {
        VkWin32SurfaceCreateInfoKHR win32CreateInfo;
        win32CreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        win32CreateInfo.pNext = nullptr;
        win32CreateInfo.flags = 0;
        win32CreateInfo.hinstance = m_hInstance;
        win32CreateInfo.hwnd = m_hWnd;
        VKERRCHECK(vkCreateWin32SurfaceKHR(m_instance, &win32CreateInfo, nullptr, &m_vkSurface));

        LOGI("Win32 - Vulkan Surface created\n");
    }
    return *this;
}

#define WM_RESHAPE (WM_USER + 0)
#define WM_ACTIVE (WM_USER + 1)

Event Win32WindowImpl::GetEvent(bool waitForEvent)
{
    if (!m_eventQueue.IsEmpty()) {
        return m_eventQueue.Pop();
    }

    MSG msg = {};
    if (waitForEvent) {
        m_running = (GetMessage(&msg, NULL, 16, 0) > 0); // Blocking mode
    } else {
        m_running = (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) > 0); // Non-blocking mode
    }

    TranslateMessage(&msg);
    int32_t x{ GET_X_LPARAM(msg.lParam) };
    int32_t y{ GET_Y_LPARAM(msg.lParam) };

    if (m_hasFocus && m_mouseLocked) {
        const uint32_t widhtHalf{ m_info.size.width / 2 };
        const uint32_t heightHalf{ m_info.size.height / 2 };

        POINT pt;
        pt.x = widhtHalf;
        pt.y = heightHalf;
        ClientToScreen(m_hWnd, &pt);
        SetCursorPos(pt.x, pt.y);
        x -= widhtHalf;
        y -= heightHalf;
    }

    //--Convert Shift / Ctrl / Alt key messages to LeftShift / RightShift / LeftCtrl / RightCtrl / LeftAlt / RightAlt--
    if (msg.message == WM_KEYDOWN || msg.message == WM_KEYUP) {
        if (msg.wParam == VK_CONTROL) {
            msg.wParam = (msg.lParam & (1 << 24)) ? VK_RCONTROL : VK_LCONTROL;
        }

        if (msg.wParam == VK_SHIFT) {
            if (!!(GetKeyState(VK_LSHIFT) & 128) != IsKeyPressed(prev::input::keyboard::KeyCode::KEY_LeftShift)) {
                PostMessage(m_hWnd, msg.message, VK_LSHIFT, 0);
            }

            if (!!(GetKeyState(VK_RSHIFT) & 128) != IsKeyPressed(prev::input::keyboard::KeyCode::KEY_RightShift)) {
                PostMessage(m_hWnd, msg.message, VK_RSHIFT, 0);
            }

            m_eventQueue.Push({ Event::EventType::NONE });
        }
    } else if (msg.message == WM_SYSKEYDOWN || msg.message == WM_SYSKEYUP) {
        if (msg.wParam == VK_MENU) {
            msg.wParam = (msg.lParam & (1 << 24)) ? VK_RMENU : VK_LMENU;
        }
    }
    //-----------------------------------------------------------------------------------------------------------------
    ButtonType bestBtn = ButtonType(IsMouseButtonPressed(ButtonType::LEFT) ? 1 : IsMouseButtonPressed(ButtonType::MIDDLE) ? 2
            : IsMouseButtonPressed(ButtonType::RIGHT)                                                                     ? 3
                                                                                                                          : 0);
    switch (msg.message) {
    //--Mouse events--
    case WM_MOUSEMOVE:
        m_eventQueue.Push(OnMouseEvent(ActionType::MOVE, x, y, bestBtn));
        break;
    case WM_LBUTTONDOWN:
        m_eventQueue.Push(OnMouseEvent(ActionType::DOWN, x, y, ButtonType::LEFT));
        break;
    case WM_MBUTTONDOWN:
        m_eventQueue.Push(OnMouseEvent(ActionType::DOWN, x, y, ButtonType::MIDDLE));
        break;
    case WM_RBUTTONDOWN:
        m_eventQueue.Push(OnMouseEvent(ActionType::DOWN, x, y, ButtonType::RIGHT));
        break;
    case WM_LBUTTONUP:
        m_eventQueue.Push(OnMouseEvent(ActionType::UP, x, y, ButtonType::LEFT));
        break;
    case WM_MBUTTONUP:
        m_eventQueue.Push(OnMouseEvent(ActionType::UP, x, y, ButtonType::MIDDLE));
        break;
    case WM_RBUTTONUP:
        m_eventQueue.Push(OnMouseEvent(ActionType::UP, x, y, ButtonType::RIGHT));
        break;

        //--Mouse wheel events--
    case WM_MOUSEWHEEL: {
        const int32_t wheel{ GET_WHEEL_DELTA_WPARAM(msg.wParam) / 120 };
        m_eventQueue.Push(OnMouseScrollEvent(wheel, x, y));
        break;
    }

    //--Keyboard events--
    case WM_KEYDOWN:
        m_eventQueue.Push(OnKeyEvent(ActionType::DOWN, WIN32_TO_HID[msg.wParam]));
        break;
    case WM_KEYUP:
        m_eventQueue.Push(OnKeyEvent(ActionType::UP, WIN32_TO_HID[msg.wParam]));
        break;
    case WM_SYSKEYDOWN: {
        MSG discard;
        GetMessage(&discard, NULL, 0, 0); // Alt-key triggers a WM_MOUSEMOVE message... Discard it.
        m_eventQueue.Push(OnKeyEvent(ActionType::DOWN, WIN32_TO_HID[msg.wParam]));
        break;
    } // +alt key
    case WM_SYSKEYUP:
        m_eventQueue.Push(OnKeyEvent(ActionType::UP, WIN32_TO_HID[msg.wParam])); // +alt key
        break;
    //--Char event--
    case WM_CHAR: {
        strncpy_s(m_textBuffer, (const char*)&msg.wParam, 4);
        m_eventQueue.Push(OnTextEvent(m_textBuffer));
        break;
    } // return UTF8 code of key pressed

    //--Window events--
    case WM_ACTIVE:
        m_eventQueue.Push(OnFocusEvent(msg.wParam != WA_INACTIVE));
        break;

    case WM_RESHAPE: {
        if (!m_hasFocus) {
            PostMessage(m_hWnd, WM_RESHAPE, msg.wParam, msg.lParam); // Repost this event to the queue
            m_eventQueue.Push(OnFocusEvent(true)); // Activate window before reshape
            break;
        }

        RECT r;
        GetClientRect(m_hWnd, &r);
        uint32_t w{ static_cast<uint32_t>(r.right - r.left) };
        uint32_t h{ static_cast<uint32_t>(r.bottom - r.top) };
        if (m_info.size != Size{ w, h }) {
            m_eventQueue.Push(OnResizeEvent(w, h)); // window resized
            break;
        }

        GetWindowRect(m_hWnd, &r);
        int32_t x{ static_cast<int32_t>(r.left) };
        int32_t y{ static_cast<int32_t>(r.top) };
        if (m_info.position != Position{ x, y }) {
            m_eventQueue.Push(OnMoveEvent(x, y)); // window moved
            break;
        }
        break;
    }
    case WM_CLOSE:
        m_eventQueue.Push(OnCloseEvent());
        break;

#ifdef ENABLE_MULTITOUCH
    case WM_POINTERUPDATE:
    case WM_POINTERDOWN:
    case WM_POINTERUP: {
        POINTER_INFO pointerInfo;
        if (GetPointerInfo(GET_POINTERID_WPARAM(msg.wParam), &pointerInfo)) {
            uint32_t id = pointerInfo.pointerId;
            POINT pt = pointerInfo.ptPixelLocation;
            ScreenToClient(m_hWnd, &pt);
            switch (msg.message) {
            case WM_POINTERDOWN:
                m_eventQueue.Push(m_MTouch.OnEventById(ActionType::DOWN, static_cast<float>(pt.x), static_cast<float>(pt.y), 0, id, static_cast<float>(m_info.size.width), static_cast<float>(m_info.size.height))); // touch down event
                break;
            case WM_POINTERUPDATE:
                m_eventQueue.Push(m_MTouch.OnEventById(ActionType::MOVE, static_cast<float>(pt.x), static_cast<float>(pt.y), id, id, static_cast<float>(m_info.size.width), static_cast<float>(m_info.size.height))); // touch move event
                break;
            case WM_POINTERUP:
                m_eventQueue.Push(m_MTouch.OnEventById(ActionType::UP, static_cast<float>(pt.x), static_cast<float>(pt.y), id, 0, static_cast<float>(m_info.size.width), static_cast<float>(m_info.size.height))); // touch up event
                break;
            }
        }
    }
#endif
    }
    DispatchMessage(&msg);

    if (!m_eventQueue.IsEmpty()) {
        return m_eventQueue.Pop();
    }
    return { Event::EventType::NONE };
}

LRESULT CALLBACK Win32WindowImpl::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg) {
    case WM_CLOSE:
        PostMessage(hWnd, WM_CLOSE, 0, 0); // for OnCloseEvent
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_PAINT:
        return 0;
    case WM_GETMINMAXINFO:
        return 0;
    case WM_EXITSIZEMOVE:
        PostMessage(hWnd, WM_RESHAPE, 0, 0);
        break;
    case WM_ACTIVATE:
        PostMessage(hWnd, WM_ACTIVE, wParam, lParam);
        break;
    default:
        break;
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
} // namespace prev::window::impl::win32

#endif