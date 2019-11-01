#include "Window.h"
#include "WindowAndroid.h"
#include "WindowWin32.h"
#include "WindowXcb.h"

Window::Window(const char* title)
{
	InitWindow(title, 640, 480, true);
}

Window::Window(const char* title, const uint32_t width, const uint32_t height)
{
	InitWindow(title, width, height, false);
}

Window::~Window()
{
	delete (m_windowImpl);
}

void Window::InitWindow(const char* title, const uint32_t width, const uint32_t height, bool tryFullscreen)
{
#ifdef VK_USE_PLATFORM_XCB_KHR
	LOGI("PLATFORM: XCB\n");
	if (tryFullscreen)
	{
		m_windowImpl = new WindowXcb(title);
	}
	else
	{
		m_windowImpl = new WindowXcb(title, width, height);
	}
#elif VK_USE_PLATFORM_WIN32_KHR
	LOGI("PLATFORM: WIN32\n");
	if (tryFullscreen)
	{
		m_windowImpl = new WindowWin32(title);
	}
	else
	{
		m_windowImpl = new WindowWin32(title, width, height);
	}
#elif VK_USE_PLATFORM_ANDROID_KHR
	LOGI("PLATFORM: ANDROID\n");
	m_windowImpl = new WindowAndroid(title, width, height);
#else 
#error NOT IMPLEMENTED PLATFORM
#endif
	// TODO:
	//    #ifdef VK_USE_PLATFORM_XLIB_KHR
	//    #ifdef VK_USE_PLATFORM_MIR_KHR
	//    #ifdef VK_USE_PLATFORM_WAYLAND_KHR
}

CSurface& Window::GetSurface(VkInstance instance)
{
	m_windowImpl->CreateSurface(instance);

	return *m_windowImpl;
}

bool Window::CanPresent(VkPhysicalDevice gpu, uint32_t queue_family) const
{
	return m_windowImpl->CanPresent(gpu, queue_family);
}

void Window::GetPosition(int16_t& x, int16_t& y) const
{
	const auto& shape = m_windowImpl->GetShape();
	
	x = shape.x; 
	y = shape.y;
}

void Window::GetSize(int16_t& width, int16_t& height) const
{
	const auto& shape = m_windowImpl->GetShape();

	width = shape.width; 
	height = shape.height;
}

bool Window::GetKeyState(const eKeycode key) const
{
	return m_windowImpl->KeyState(key);
}

bool Window::GetBtnState(const eMouseButton btn) const
{
	return m_windowImpl->BtnState(btn);
}

void Window::GetMousePos(int16_t& x, int16_t& y) const
{
	m_windowImpl->MousePos(x, y);
}

bool Window::HasFocus() const
{
	return m_windowImpl->HasFocus();
}

void Window::SetTitle(const char* title)
{
	m_windowImpl->SetTitle(title);
}

void Window::SetWinPos(uint16_t x, uint16_t y)
{
	m_windowImpl->SetWinPos(x, y);
}

void Window::SetWinSize(uint16_t w, uint16_t h)
{
	m_windowImpl->SetWinSize(w, h);
}

void Window::ShowKeyboard(bool enabled) // On Android, show the soft-keyboard.
{
	m_windowImpl->SetTextInput(enabled);
}

void Window::Close()
{
	m_windowImpl->Close();
}

EventType Window::GetEvent(bool waitForEvent)
{
	return m_windowImpl->GetEvent(waitForEvent);
}

bool Window::ProcessEvents(bool waitForEvent)
{
	EventType e = m_windowImpl->GetEvent(waitForEvent);
	while (e.tag != EventType::NONE)
	{
		// Calling the event handlers
		switch (e.tag)
		{
			case EventType::MOUSE:
				OnMouseEvent(e.mouse.action, e.mouse.x, e.mouse.y, e.mouse.btn);
				break;
			case EventType::MOUSE_SCROLL:
				OnMouseScrollEvent(e.scroll.delta, e.scroll.x, e.scroll.y);
			case EventType::KEY: 
				OnKeyEvent(e.key.action, e.key.keycode);                        
				break;
			case EventType::TEXT: 
				OnTextEvent(e.text.str);                                         
				break;
			case EventType::MOVE: 
				OnMoveEvent(e.move.x, e.move.y);                                 
				break;
			case EventType::RESIZE: 
				OnResizeEvent(e.resize.width, e.resize.height);                    
				break;
			case EventType::FOCUS: 
				OnFocusEvent(e.focus.hasFocus);                                  
				break;
			case EventType::TOUCH: 
				OnTouchEvent(e.touch.action, e.touch.x, e.touch.y, e.touch.id);   
				break;
			case EventType::INIT:
 				OnInitEvent();
				break;
			case EventType::CLOSE: 
				OnCloseEvent(); 
				return false;
			default: 
				break;
		}

		e = m_windowImpl->GetEvent();
	}

	return m_windowImpl->IsRunning();
}
