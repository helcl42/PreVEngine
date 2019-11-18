#include "Window.h"
#include "WindowAndroid.h"
#include "WindowWin32.h"
#include "WindowXcb.h"

namespace PreVEngine
{
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

	Surface& Window::GetSurface(VkInstance instance)
	{
		m_windowImpl->CreateSurface(instance);

		return *m_windowImpl;
	}

	bool Window::CanPresent(VkPhysicalDevice gpu, uint32_t queueFamily) const
	{
		return m_windowImpl->CanPresent(gpu, queueFamily);
	}

	Position Window::GetPosition() const
	{
		const auto& shape = m_windowImpl->GetShape();

		return Position{ shape.x, shape.y };
	}

	Size Window::GetSize() const
	{
		const auto& shape = m_windowImpl->GetShape();

		return Size{ shape.width, shape.height };
	}

	bool Window::IsKeyPressed(const KeyCode key) const
	{
		return m_windowImpl->IsKeyPressed(key);
	}

	bool Window::IsMouseButtonPressed(const ButtonType btn) const
	{
		return m_windowImpl->IsMouseButtonPressed(btn);
	}

	Position Window::GetMousePosition() const
	{
		return m_windowImpl->GetMousePosition();
	}

	bool Window::HasFocus() const
	{
		return m_windowImpl->HasFocus();
	}

	void Window::SetTitle(const char* title)
	{
		m_windowImpl->SetTitle(title);
	}

	void Window::SetPosition(const Position& position)
	{
		m_windowImpl->SetPosition(position.x, position.y);
	}

	void Window::SetSize(const Size& size)
	{
		m_windowImpl->SetSize(size.width, size.height);
	}

	void Window::ShowKeyboard(bool enabled) // On Android, show the soft-keyboard.
	{
		m_windowImpl->SetTextInput(enabled);
	}

	void Window::Close()
	{
		m_windowImpl->Close();
	}

	Event Window::GetEvent(bool waitForEvent)
	{
		return m_windowImpl->GetEvent(waitForEvent);
	}

	bool Window::ProcessEvents(bool waitForEvent)
	{
		Event e = m_windowImpl->GetEvent(waitForEvent);
		while (e.tag != Event::NONE)
		{
			// Calling the event handlers
			switch (e.tag)
			{
				case Event::MOUSE:
					OnMouseEvent(e.mouse.action, e.mouse.x, e.mouse.y, e.mouse.btn);
					break;
				case Event::MOUSE_SCROLL:
					OnMouseScrollEvent(e.scroll.delta, e.scroll.x, e.scroll.y);
				case Event::KEY:
					OnKeyEvent(e.key.action, e.key.keycode);
					break;
				case Event::TEXT:
					OnTextEvent(e.text.str);
					break;
				case Event::MOVE:
					OnMoveEvent(e.move.x, e.move.y);
					break;
				case Event::RESIZE:
					OnResizeEvent(e.resize.width, e.resize.height);
					break;
				case Event::FOCUS:
					OnFocusEvent(e.focus.hasFocus);
					break;
				case Event::TOUCH:
					OnTouchEvent(e.touch.action, e.touch.x, e.touch.y, e.touch.id);
					break;
				case Event::INIT:
					OnInitEvent();
					break;
				case Event::CLOSE:
					OnCloseEvent();
					return false;
				default:
					break;
			}

			e = m_windowImpl->GetEvent();
		}

		return m_windowImpl->IsRunning();
	}
}