#ifdef VK_USE_PLATFORM_XCB_KHR

#ifndef __WINDOW_XCB__
#define __WINDOW_XCB__

//-------------------------------------------------
#include "../WindowImpl.h"
//#include <xcb/xcb.h>            // XCB only
//#include <X11/Xlib.h>           // XLib only
#include <X11/Xlib-xcb.h> // Xlib + XCB
#include <X11/cursorfont.h>
#include <xkbcommon/xkbcommon.h> // Keyboard
//-------------------------------------------------

namespace prev::window::linux {
class WindowXcb final : public WindowImpl {
public:
    WindowXcb(const char* title);

    WindowXcb(const char* title, uint32_t width, uint32_t height);

    ~WindowXcb();

public:
    Event GetEvent(bool wait_for_event = false);

    bool CanPresent(VkPhysicalDevice phy, uint32_t queue_family) const; // check if this window can present this queue type

private:
    void SetTitle(const char* title);

    void SetPosition(uint32_t x, uint32_t y);

    void SetSize(uint32_t w, uint32_t h);

    void SetMouseCursorVisible(bool visible);

    bool CreateSurface(VkInstance instance);

    bool InitTouch(); // Returns false if no touch-device was found.

    Event TranslateEvent(xcb_generic_event_t* x_event); // Convert x_event to Window event

    void Init(const char* title, const uint32_t width, const uint32_t height, bool tryFullscreen);

private:
    Display* m_display; // for XLib

    xcb_connection_t* m_xcbConnection; // for XCB

    xcb_screen_t* m_xcbScreen;

    xcb_window_t m_xcbWindow;

    xcb_intern_atom_reply_t* m_atomWmDeleteWindow;

    //---xkb Keyboard---
    xkb_context* m_keyboardContext; // context for xkbcommon keyboard input

    xkb_keymap* m_keyboardKeymap;

    xkb_state* m_keyboardState;
    //------------------

    //---Touch Device---
    MultiTouch m_MTouch;

    int m_xiOpcode; // 131

    int m_xiDevId; // 2
    //------------------
};

#endif
}

#endif // VK_USE_PLATFORM_XCB_KHR
