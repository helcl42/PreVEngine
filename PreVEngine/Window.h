#ifndef __WINDOW_H__
#define __WINDOW_H__

#ifdef ANDROID
#include <native.h>
#endif

#include "Common.h"
#include "WindowImpl.h"

namespace PreVEngine
{
	class IWindow
	{
	public:
		virtual Surface& GetSurface(VkInstance instance) = 0;

		virtual bool CanPresent(VkPhysicalDevice gpu, uint32_t queueFamily) const = 0;

		virtual Position GetPosition() const = 0;

		virtual Size GetSize() const = 0;

		virtual bool IsKeyPressed(const KeyCode key) const = 0;

		virtual bool IsMouseButtonPressed(const ButtonType btn) const = 0;

		virtual Position GetMousePosition() const = 0;

		virtual bool HasFocus() const = 0;

		virtual void SetTitle(const char* title) = 0;

		virtual void SetPosition(const Position& position) = 0;

		virtual void SetSize(const Size& size) = 0;

		virtual void ShowKeyboard(bool enabled) = 0;

		virtual void Close() = 0;

	public:
		virtual ~IWindow() = default;
	};

	class Window : public IWindow
	{
	private:
		WindowImpl* m_windowImpl;

	private:
		void InitWindow(const char* title, const uint32_t width, const uint32_t height, bool tryFullscreen);

	public:
		Window(const char* title);

		Window(const char* title, const uint32_t width, const uint32_t height);

		virtual ~Window();

	public:
		Surface& GetSurface(VkInstance instance);

		bool CanPresent(VkPhysicalDevice gpu, uint32_t queueFamily) const;

	public:
		Position GetPosition() const;

		Size GetSize() const;

		bool IsKeyPressed(const KeyCode key) const;

		bool IsMouseButtonPressed(const ButtonType btn) const;

		Position GetMousePosition() const;

		bool HasFocus() const;

	public:
		void SetTitle(const char* title);

		void SetPosition(const Position& position);

		void SetSize(const Size& size);

		void ShowKeyboard(bool enabled);

		void Close();

	public:
		Event GetEvent(bool waitForEvent = false);  // Return a single event from the queue (Alternative to using ProcessEvents.)

		bool ProcessEvents(bool waitForEvent = false);  // Poll events, and call event handlers. Returns false if window is closing.

	public:
		virtual void OnInitEvent()
		{
		}

		virtual void OnCloseEvent()
		{
		}

		virtual void OnResizeEvent(uint16_t width, uint16_t height)
		{
		}

		virtual void OnMoveEvent(int16_t x, int16_t y)
		{
		}

		virtual void OnFocusEvent(bool hasFocus)
		{
		}

		virtual void OnMouseEvent(ActionType action, int16_t x, int16_t y, ButtonType btn)
		{
		}

		virtual void OnMouseScrollEvent(int16_t delta, int16_t x, int16_t y)
		{
		}

		virtual void OnKeyEvent(ActionType action, KeyCode keycode)
		{
		}

		virtual void OnTextEvent(const char *str)
		{
		}

		virtual void OnTouchEvent(ActionType action, float x, float y, uint8_t id)
		{
		}
	};
}

#endif
