#ifdef VK_USE_PLATFORM_XCB_KHR

#ifndef __XCB_WINDOW_IMPL_H__
#define __XCB_WINDOW_IMPL_H__

#include "../WindowImpl.h"

// #include <xcb/xcb.h>              // XCB only
// #include <X11/Xlib.h>             // XLib only
#include <X11/Xlib-xcb.h> // Xlib + XCB
#include <X11/cursorfont.h>
#include <xkbcommon/xkbcommon.h> // Keyboard

namespace prev::window::impl::xcb {
class XcbWindowImpl final : public WindowImpl {
public:
    XcbWindowImpl(const prev::core::instance::Instance& instance, const WindowInfo& windowInfo);

    ~XcbWindowImpl();

public:
    bool PollEvent(bool waitForEvent, Event& outEvent) override;

    void SetTitle(const std::string& title) override;

    void SetPosition(int32_t x, int32_t y) override;

    void SetSize(uint32_t w, uint32_t h) override;

    void SetMouseCursorVisible(bool visible) override;

    Surface& CreateSurface() override;

private:
    bool InitTouch(); // Returns false if no touch-device was found.

    void ShutDownTouch();

    void ProcessXEvent(xcb_generic_event_t* xEvent); // Convert x_event to Window event

private:
    Display* m_display{}; // for XLib

    xcb_connection_t* m_xcbConnection{}; // for XCB

    xcb_screen_t* m_xcbScreen{};

    xcb_window_t m_xcbWindow{};

    xcb_intern_atom_reply_t* m_atomWmDeleteWindow{};

    //---xkb Keyboard---
    xkb_context* m_keyboardContext{}; // context for xkbcommon keyboard input

    xkb_keymap* m_keyboardKeymap{};

    xkb_state* m_keyboardState{};
    //------------------

    //---Touch Device---
    MultiTouch m_MTouch{};

    int m_xiOpcode{};

    int m_xiDevId{};
    //------------------
};
} // namespace prev::window::impl::xcb

#endif

#endif // VK_USE_PLATFORM_XCB_KHR
