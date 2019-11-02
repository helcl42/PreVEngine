#ifndef __WINDOW_H__
#define __WINDOW_H__

#ifdef ANDROID
#include <native.h>
#endif

#include "WindowImpl.h"

class Window 
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

    bool IsMouseButtonPressed(const MouseButtonType btn) const;

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
	virtual void OnMouseEvent(ActionType action, int16_t x, int16_t y, MouseButtonType btn) {}

	virtual void OnMouseScrollEvent(int16_t delta, int16_t x, int16_t y) {}

    virtual void OnKeyEvent(ActionType action, KeyCode keycode) {}

    virtual void OnTextEvent(const char *str) {}

    virtual void OnMoveEvent(int16_t x, int16_t y) {}

    virtual void OnResizeEvent(uint16_t width, uint16_t height) {}

    virtual void OnFocusEvent(bool hasFocus) {}

    virtual void OnTouchEvent(ActionType action, float x, float y, uint8_t id) {}
    
	virtual void OnInitEvent() {}

	virtual void OnCloseEvent() {}
};

#endif
