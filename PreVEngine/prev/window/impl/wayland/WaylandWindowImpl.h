#ifdef VK_USE_PLATFORM_WAYLAND_KHR

#ifndef __WAYLAND_WINDOW_IMPL_H__
#define __WAYLAND_WINDOW_IMPL_H__

#include "../WindowImpl.h"

#include <wayland-client.h>

#include <xdg-shell.h>

namespace prev::window::impl::wayland {
class WaylandWindowImpl final : public WindowImpl {
public:
    WaylandWindowImpl(const prev::core::instance::Instance& instance, const WindowInfo& windowInfo);

    ~WaylandWindowImpl();

public:
    bool PollEvent(bool waitForEvent, Event& outEvent) override;

    void SetTitle(const std::string& title) override;

    void SetPosition(int32_t x, int32_t y) override;

    void SetSize(uint32_t w, uint32_t h) override;

    void SetMouseCursorVisible(bool visible) override;

    Surface& CreateSurface() override;

private:
    // registry
    static void OnRegistryAdd(void* data, wl_registry* registry, uint32_t id, const char* interface, uint32_t version);

    static void OnRegistryRemove(void* data, wl_registry* registry, uint32_t name);

    // shell surface
    static void OnConfigure(void* data, xdg_surface* shellSurface, uint32_t serial);

    // top level
    static void OnToplevelConfigure(void* data, struct xdg_toplevel* toplevel, int32_t width, int32_t height, struct wl_array* states);

    static void OnToplevelClose(void* data, struct xdg_toplevel* toplevel);

    // pointer
    static void OnPointerEnter(void* data, wl_pointer* pointer, uint32_t serial, wl_surface* surface, wl_fixed_t sx, wl_fixed_t sy);

    static void OnPointerLeave(void* data, wl_pointer* pointer, uint32_t serial, wl_surface* surface);

    static void OnPointerMotion(void* data, wl_pointer* pointer, uint32_t time, wl_fixed_t sx, wl_fixed_t sy);

    static void OnPointerButton(void* data, wl_pointer* pointer, uint32_t serial, uint32_t time, uint32_t button, uint32_t state);

    static void OnPointerAxis(void* data, wl_pointer* pointer, uint32_t time, uint32_t axis, wl_fixed_t value);

    // keyboard
    static void OnKeyboardKeymap(void* data, wl_keyboard* keyboard, uint32_t format, int fd, uint32_t size);

    static void OnKeyboardEnter(void* data, wl_keyboard* keyboard, uint32_t serial, wl_surface* surface, wl_array* keys);

    static void OnKeyboardLeave(void* data, wl_keyboard* keyboard, uint32_t serial, wl_surface* surface);

    static void OnKeyboardKey(void* data, wl_keyboard* keyboard, uint32_t serial, uint32_t time, uint32_t key, uint32_t state);

    static void OnKeyboardModifiers(void* data, wl_keyboard* keyboard, uint32_t serial, uint32_t modsDepressed, uint32_t modsLatched, uint32_t modsLocked, uint32_t group);

    // touch
    static void OnTouchDown(void* data, wl_touch* wl_touch, uint32_t serial, uint32_t time, wl_surface* surface, int32_t id, wl_fixed_t x, wl_fixed_t y);

    static void OnTouchUp(void* data, wl_touch* wl_touch, uint32_t serial, uint32_t time, int32_t id);

    static void OnTouchMotion(void* data, wl_touch* wl_touch, uint32_t time, int32_t id, wl_fixed_t x, wl_fixed_t y);

    static void OnTouchFrame(void* data, wl_touch* wl_touch);

    static void OnTouchCancel(void* data, wl_touch* wl_touch);

    static void OnTouchShape(void* data, wl_touch* wl_touch, int32_t id, wl_fixed_t major, wl_fixed_t minor);

    static void OnTouchOrientation(void* data, wl_touch* wl_touch, int32_t id, wl_fixed_t orientation);

    // seat
    static void OnSeatCapabilities(void* data, wl_seat* seat, uint32_t caps);

private:
    wl_display* m_display{};

    wl_registry* m_registry{};

    wl_compositor* m_compositor{};

    wl_surface* m_surface{};

    xdg_wm_base* m_shell{};

    xdg_surface* m_shellSurface{};

    xdg_toplevel* m_topLevel{};

    wl_seat* m_seat{};

    wl_pointer* m_pointer{};

    wl_keyboard* m_keyboard{};

    wl_touch* m_touch{};

private:
    const int32_t ResolutionScaleFactor{ 256 }; // TODO - where does this scaler come from ??

    Position m_lastMousePosition{};

    MultiTouch m_MTouch{};

private:
    static const wl_registry_listener registryListener;

    static const xdg_surface_listener shellSurfaceListener;

    static const xdg_toplevel_listener toplevelListener;

    static const wl_pointer_listener pointerListener;

    static const wl_keyboard_listener keyboardListener;

    static const wl_touch_listener touchListener;

    static const wl_seat_listener seatListener;
};
} // namespace prev::window::impl::wayland

#endif

#endif // VK_USE_PLATFORM_WAYLAND_KHR
