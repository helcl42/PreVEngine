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
    CSurface& GetSurface(VkInstance instance);							// Returns Vulkan Surface (VkSurfaceKHR).
    
	bool CanPresent(VkPhysicalDevice gpu, uint32_t queue_family) const; // Returns true if this window can present the given queue type.

public:
    //--State query functions--
    void GetPosition(int16_t& x, int16_t& y) const;			// Get the window's x,y position, relative to top-left

    void GetSize(int16_t& width, int16_t& height) const;	// Get the window's width and height

    bool GetKeyState(const eKeycode key) const;             // Returns true if specified key is pressed. (see keycodes.h)

    bool GetBtnState(const eMouseButton btn) const;         // Returns true if specified mouse button is pressed (button 1-3)

    void GetMousePos(int16_t& x, int16_t& y) const;         // Get mouse (x,y) coordinate within window client area

    bool HasFocus() const;

public:
    //--Control functions--
    void SetTitle(const char* title);                   // Set window title

    void SetWinPos (uint16_t x, uint16_t y);            // Set window position

    void SetWinSize(uint16_t w, uint16_t h);            // Set window client-area size (Excludes title bar and borders.)

    void ShowKeyboard(bool enabled);                    // on Android, show the soft-keyboard.

    void Close();                                       // Close the window

public:
    //--Event loop--
    EventType GetEvent(bool wait_for_event = false);  // Return a single event from the queue (Alternative to using ProcessEvents.)

    bool ProcessEvents(bool wait_for_event = false);  // Poll events, and call event handlers. Returns false if window is closing.

public:
    //-- Virtual Functions as event handlers --
    virtual void OnMouseEvent(eAction action, int16_t x, int16_t y, uint8_t btn) {}  // Callback for mouse events

	virtual void OnMouseScrollEvent(int16_t delta, int16_t x, int16_t y) {}			 // Callback for mouse scroll

    virtual void OnKeyEvent(eAction action, eKeycode keycode) {}                     // Callback for keyboard events (keycodes)

    virtual void OnTextEvent(const char *str) {}                                     // Callback for text typed events (text)

    virtual void OnMoveEvent(int16_t x, int16_t y) {}                                // Callback for window move events

    virtual void OnResizeEvent(uint16_t width, uint16_t height) {}                   // Callback for window resize events

    virtual void OnFocusEvent(bool hasFocus) {}                                      // Callback for window gain/lose focus events

    virtual void OnTouchEvent(eAction action, float x, float y, uint8_t id) {}       // Callback for Multi-touch events
    
	virtual void OnInitEvent() {}													 // Callback for window init event

	virtual void OnCloseEvent() {}                                                   // Callback for window closing event
};

#endif
