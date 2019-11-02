#ifdef VK_USE_PLATFORM_WIN32_KHR

#ifndef __WINDOW_WIN32__
#define __WINDOW_WIN32__

#include "WindowImpl.h"

#include <windowsx.h>  // Mouse

// Convert native Win32 keyboard scancode to cross-platform USB HID code.
const unsigned char WIN32_TO_HID[256] = {
	  0,  0,  0,  0,  0,  0,  0,  0, 42, 43,  0,  0,  0, 40,  0,  0,    // 16
	225,224,226, 72, 57,  0,  0,  0,  0,  0,  0, 41,  0,  0,  0,  0,    // 32
	 44, 75, 78, 77, 74, 80, 82, 79, 81,  0,  0,  0, 70, 73, 76,  0,    // 48
	 39, 30, 31, 32, 33, 34, 35, 36, 37, 38,  0,  0,  0,  0,  0,  0,    // 64
	  0,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18,    // 80
	 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29,  0,  0,  0,  0,  0,    // 96
	 98, 89, 90, 91, 92, 93, 94, 95, 96, 97, 85, 87,  0, 86, 99, 84,    //112
	 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69,104,105,106,107,    //128
	108,109,110,111,112,113,114,115,  0,  0,  0,  0,  0,  0,  0,  0,    //144
	 83, 71,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,    //160
	225,229,224,228,226,230,  0,  0,  0,  0,  0,  0,  0,127,128,129,    //176    L/R shift/ctrl/alt  mute/vol+/vol-
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 51, 46, 54, 45, 55, 56,    //192
	 53,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,    //208
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 47, 49, 48, 52,  0,    //224
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,    //240
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0     //256
};

class WindowWin32 : public WindowImpl
{
private:
	HINSTANCE m_hInstance;

	HWND m_hWnd;

	MultiTouch m_MTouch;  // Multi-Touch device

private:
	void SetTitle(const char* title);

	void SetPosition(uint32_t x, uint32_t y);

	void SetSize(uint32_t w, uint32_t h);

	void CreateSurface(VkInstance instance);

	void GetDesktopResolution(uint32_t& horizontal, uint32_t& vertical) const;

	void Init(const char* title, uint32_t width, uint32_t height, bool isFullScreen);

private:
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
	WindowWin32(const char* title); // creates fullScreen window

	WindowWin32(const char* title, uint32_t width, uint32_t height);

	virtual ~WindowWin32();

public:
	Event GetEvent(bool waitForEvent = false);

	bool CanPresent(VkPhysicalDevice phy, uint32_t queueFamily) const;
};

#endif

void WindowWin32::GetDesktopResolution(uint32_t& horizontal, uint32_t& vertical) const
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

void WindowWin32::Init(const char* title, uint32_t width, uint32_t height, bool tryFullScreen)
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
	winClass.lpszClassName = title;
	winClass.hInstance = m_hInstance;
	winClass.hIconSm = LoadIcon(NULL, IDI_WINLOGO);
	// Register window class:
	ATOM atom = RegisterClassEx(&winClass);
	assert(atom && "Failed to register the window class.");

	uint32_t screenWidth, screenHeight;
	GetDesktopResolution(screenWidth, screenHeight);

	bool fullScreen = false;
	if (tryFullScreen)
	{
		fullScreen = true;

		DEVMODE dmScreenSettings;
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = screenWidth;
		dmScreenSettings.dmPelsHeight = screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		if ((width != (uint32_t)screenWidth) && (height != (uint32_t)screenHeight))
		{
			if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
			{
				MessageBox(NULL, "Fullscreen Mode not supported!\n Switch to window mode?", "Error", MB_OK | MB_ICONERROR);
				fullScreen = false;
			}
		}

	}

	DWORD dwExStyle;
	DWORD dwStyle;

	if (fullScreen)
	{
		dwExStyle = WS_EX_APPWINDOW;
		dwStyle = WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	}
	else
	{
		dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
		dwStyle = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	}

	m_shape.width = uint16_t(fullScreen ? screenWidth : width);
	m_shape.height = uint16_t(fullScreen ? screenHeight : height);
	m_shape.fullscreen = fullScreen;

	RECT windowRect;
	windowRect.left = 0L;
	windowRect.top = 0L;
	windowRect.right = (long)m_shape.width;
	windowRect.bottom = (long)m_shape.height;

	AdjustWindowRectEx(&windowRect, dwStyle, FALSE, dwExStyle);

	m_hWnd = CreateWindowEx(0, title, title, dwStyle | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0, 0, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, NULL, NULL, m_hInstance, NULL);
	assert(m_hWnd && "Failed to create a window.");

	ShowWindow(m_hWnd, SW_SHOW);
	SetForegroundWindow(m_hWnd);
	SetFocus(m_hWnd);

	m_eventQueue.Push(OnResizeEvent(width, height));

	m_eventQueue.Push(OnInitEvent());
}

WindowWin32::WindowWin32(const char* title)
{
	Init(title, 640, 480, true);
}

WindowWin32::WindowWin32(const char* title, uint32_t width, uint32_t height)
{
	Init(title, width, height, false);
}

WindowWin32::~WindowWin32()
{
	DestroyWindow(m_hWnd);
}

void WindowWin32::SetTitle(const char* title)
{
	SetWindowText(m_hWnd, title);
}

void WindowWin32::SetPosition(uint32_t x, uint32_t y)
{
	SetWindowPos(m_hWnd, NULL, x, y, 0, 0, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE);
	
	if (x != m_shape.x || y != m_shape.y)
	{
		m_eventQueue.Push(OnMoveEvent(x, y));  // Trigger window moved event
	}
}

void WindowWin32::SetSize(uint32_t w, uint32_t h)
{
	if (m_shape.fullscreen)
	{
		return;
	}

	RECT wr = { 0, 0, (LONG)w, (LONG)h };
	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);  // Add border size to create desired client area size

	int total_width = wr.right - wr.left;
	int total_height = wr.bottom - wr.top;
	SetWindowPos(m_hWnd, NULL, 0, 0, total_width, total_height, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOMOVE);

	if ((w != m_shape.width) || (h != m_shape.height))
	{
		m_eventQueue.Push(OnResizeEvent(w, h));  // Trigger resize event
	}
}

void WindowWin32::CreateSurface(VkInstance instance)
{
	if (m_vkSurface) return;

	m_vkInstance = instance;

	VkWin32SurfaceCreateInfoKHR win32CreateInfo;
	win32CreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	win32CreateInfo.pNext = nullptr;
	win32CreateInfo.flags = 0;
	win32CreateInfo.hinstance = m_hInstance;
	win32CreateInfo.hwnd = m_hWnd;
	VKERRCHECK(vkCreateWin32SurfaceKHR(instance, &win32CreateInfo, nullptr, &m_vkSurface));

	LOGI("Vulkan Surface created\n");
}

#define WM_RESHAPE (WM_USER + 0)
#define WM_ACTIVE  (WM_USER + 1)

Event WindowWin32::GetEvent(bool waitForEvent)
{
	// Event event;
	if (!m_eventQueue.IsEmpty())
	{
		return *m_eventQueue.Pop();
	}

	MSG msg = {};
	if (waitForEvent)
	{
		m_isRunning = (GetMessage(&msg, NULL, 16, 0) > 0);             // Blocking mode
	}
	else
	{
		m_isRunning = (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) > 0);  // Non-blocking mode
	}

	if (m_isRunning)
	{
		TranslateMessage(&msg);
		int16_t x = GET_X_LPARAM(msg.lParam);
		int16_t y = GET_Y_LPARAM(msg.lParam);

		//--Convert Shift / Ctrl / Alt key messages to LeftShift / RightShift / LeftCtrl / RightCtrl / LeftAlt / RightAlt--
		if (msg.message == WM_KEYDOWN || msg.message == WM_KEYUP)
		{
			if (msg.wParam == VK_CONTROL)
			{
				msg.wParam = (msg.lParam & (1 << 24)) ? VK_RCONTROL : VK_LCONTROL;
			}

			if (msg.wParam == VK_SHIFT)
			{
				if (!!(GetKeyState(VK_LSHIFT) & 128) != IsKeyPressed(KEY_LeftShift))
				{
					PostMessage(m_hWnd, msg.message, VK_LSHIFT, 0);
				}

				if (!!(GetKeyState(VK_RSHIFT) & 128) != IsKeyPressed(KEY_RightShift))
				{
					PostMessage(m_hWnd, msg.message, VK_RSHIFT, 0);
				}
				
				return { Event::NONE };
			}
		}
		else if (msg.message == WM_SYSKEYDOWN || msg.message == WM_SYSKEYUP)
		{
			if (msg.wParam == VK_MENU)
			{
				msg.wParam = (msg.lParam & (1 << 24)) ? VK_RMENU : VK_LMENU;
			}
		}
		//-----------------------------------------------------------------------------------------------------------------

		static char buf[4] = {};
		MouseButtonType bestBtn = MouseButtonType(IsMouseButtonPressed(MouseButtonType::LEFT) ? 1 : IsMouseButtonPressed(MouseButtonType::MIDDLE) ? 2 : IsMouseButtonPressed(MouseButtonType::RIGHT) ? 3 : 0);

		switch (msg.message)
		{
			//--Mouse events--
			case WM_MOUSEMOVE: 
				return OnMouseEvent(ActionType::MOVE, x, y, bestBtn);
			case WM_LBUTTONDOWN: 
				return OnMouseEvent(ActionType::DOWN, x, y, MouseButtonType::LEFT);
			case WM_MBUTTONDOWN: 
				return OnMouseEvent(ActionType::DOWN, x, y, MouseButtonType::MIDDLE);
			case WM_RBUTTONDOWN: 
				return OnMouseEvent(ActionType::DOWN, x, y, MouseButtonType::RIGHT);
			case WM_LBUTTONUP: 
				return OnMouseEvent(ActionType::UP, x, y, MouseButtonType::LEFT);
			case WM_MBUTTONUP: 
				return  OnMouseEvent(ActionType::UP, x, y, MouseButtonType::MIDDLE);
			case WM_RBUTTONUP: 
				return OnMouseEvent(ActionType::UP, x, y, MouseButtonType::RIGHT);

			//--Mouse wheel events--
			case WM_MOUSEWHEEL:
			{
				uint16_t wheel = GET_WHEEL_DELTA_WPARAM(msg.wParam) / 120;
				return OnMouseScrollEvent(wheel, x, y);
			}

			//--Keyboard events--
			case WM_KEYDOWN: 
				return OnKeyEvent(ActionType::DOWN, WIN32_TO_HID[msg.wParam]);
			case WM_KEYUP: 
				return OnKeyEvent(ActionType::UP, WIN32_TO_HID[msg.wParam]);
			case WM_SYSKEYDOWN:
			{
				MSG discard; GetMessage(&discard, NULL, 0, 0);     // Alt-key triggers a WM_MOUSEMOVE message... Discard it.
				return OnKeyEvent(ActionType::DOWN, WIN32_TO_HID[msg.wParam]);
			} // +alt key
			case WM_SYSKEYUP: 
				return OnKeyEvent(ActionType::UP, WIN32_TO_HID[msg.wParam]);   // +alt key

			//--Char event--
			case WM_CHAR:
			{
				strncpy_s(buf, (const char*)&msg.wParam, 4);
				return OnTextEvent(buf);
			}  // return UTF8 code of key pressed

			//--Window events--
			case WM_ACTIVE:
			{
				return OnFocusEvent(msg.wParam != WA_INACTIVE);
			}

			case WM_RESHAPE:
			{
				if (!m_hasFocus)
				{
					PostMessage(m_hWnd, WM_RESHAPE, msg.wParam, msg.lParam);  // Repost this event to the queue
					return OnFocusEvent(true);                                // Activate window before reshape
				}

				RECT r;
				GetClientRect(m_hWnd, &r);
				uint16_t w = (uint16_t)(r.right - r.left);
				uint16_t h = (uint16_t)(r.bottom - r.top);
				if (w != m_shape.width || h != m_shape.height)
				{
					return OnResizeEvent(w, h);  // window resized
				}

				GetWindowRect(m_hWnd, &r);
				int16_t x = (int16_t)r.left;
				int16_t y = (int16_t)r.top;
				if (x != m_shape.x || y != m_shape.y)
				{
					return OnMoveEvent(x, y);  // window moved
				}
			}
			case WM_CLOSE:
			{
				LOGI("WM_CLOSE\n");  
				return OnCloseEvent();
			}
#ifdef ENABLE_MULTITOUCH
			case WM_POINTERUPDATE:
			case WM_POINTERDOWN:
			case WM_POINTERUP:
			{
				POINTER_INFO pointerInfo;
				if (GetPointerInfo(GET_POINTERID_WPARAM(msg.wParam), &pointerInfo))
				{
					uint  id = pointerInfo.pointerId;
					POINT pt = pointerInfo.ptPixelLocation;
					ScreenToClient(hWnd, &pt);
					switch (msg.message)
					{
						case WM_POINTERDOWN: 
							return m_MTouch.OnEventById(DOWN, x, y, 0, id);  // touch down event
						case WM_POINTERUPDATE: 
							return m_MTouch.OnEventById(MOVE, x, y, id, id);  // touch move event
						case WM_POINTERUP: 
							return m_MTouch.OnEventById(UP, x, y, id, 0);  // touch up event
					}
				}
			}
#endif
		}
		DispatchMessage(&msg);
	}
	return { Event::NONE };
}

LRESULT CALLBACK WindowWin32::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_CLOSE:
			PostMessage(hWnd, WM_CLOSE, 0, 0);  // for OnCloseEvent
			return 0;
		case WM_DESTROY:
			LOGI("WM_DESTROY\n");
			PostQuitMessage(0);
			return 0;
		case WM_PAINT:
			return 0;
		case WM_GETMINMAXINFO:
			return 0;
		case WM_EXITSIZEMOVE:
		{
			PostMessage(hWnd, WM_RESHAPE, 0, 0);          
			break;
		}
		case WM_ACTIVATE:
		{
			PostMessage(hWnd, WM_ACTIVE, wParam, lParam); 
			break;
		}
		default: break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

// Return true if this window can present the given queue type
bool WindowWin32::CanPresent(VkPhysicalDevice gpu, uint32_t queue_family) const
{
	return vkGetPhysicalDeviceWin32PresentationSupportKHR(gpu, queue_family) == VK_TRUE;
}

#endif  // VK_USE_PLATFORM_WIN32_KHR

