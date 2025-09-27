#include "WaylandWindowImpl.h"

#ifdef VK_USE_PLATFORM_WAYLAND_KHR

#include "../../../common/Logger.h"
#include "../../../util/VkUtils.h"

#include <linux/input.h>

#include <stdexcept>

namespace prev::window::impl::wayland {

// Convert native libinput keyboard key-code to cross-platform USB HID code.
const uint8_t INPUT_TO_HID[256] = {
    0, 41, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 45, 46, 42, 43,
    20, 26, 8, 21, 23, 28, 24, 12, 18, 19, 47, 48, 40, 224, 4, 22,
    7, 9, 10, 11, 13, 14, 15, 51, 52, 53, 225, 49, 29, 27, 6, 25,
    5, 17, 16, 54, 55, 56, 229, 85, 226, 44, 57, 58, 59, 60, 61, 62,
    63, 64, 65, 66, 67, 83, 71, 95, 96, 97, 86, 92, 93, 94, 87, 89,
    90, 91, 98, 99, 0, 0, 100, 68, 69, 0, 0, 0, 0, 0, 0, 0, 88,
    228, 84, 70, 230, 0, 74, 82, 75, 80, 79, 77, 81, 78, 73, 76,
    0, 127, 128, 129, 0, 103, 0, 72, 0, 0, 0, 0, 0, 227, 231, 118,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 104, 105, 106, 107, 108, 109, 110, 111, 112,
    113, 114, 115, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

const wl_registry_listener WaylandWindowImpl::registryListener = {
    WaylandWindowImpl::OnRegistryAdd,
    WaylandWindowImpl::OnRegistryRemove,
};

const wl_seat_listener WaylandWindowImpl::seatListener = {
    WaylandWindowImpl::OnSeatCapabilities,
};

const xdg_surface_listener WaylandWindowImpl::shellSurfaceListener = {
    WaylandWindowImpl::OnConfigure,
};

const wl_pointer_listener WaylandWindowImpl::pointerListener = {
    WaylandWindowImpl::OnPointerEnter,
    WaylandWindowImpl::OnPointerLeave,
    WaylandWindowImpl::OnPointerMotion,
    WaylandWindowImpl::OnPointerButton,
    WaylandWindowImpl::OnPointerAxis,
};

const wl_keyboard_listener WaylandWindowImpl::keyboardListener = {
    WaylandWindowImpl::OnKeyboardKeymap,
    WaylandWindowImpl::OnKeyboardEnter,
    WaylandWindowImpl::OnKeyboardLeave,
    WaylandWindowImpl::OnKeyboardKey,
    WaylandWindowImpl::OnKeyboardModifiers,
};

const wl_touch_listener WaylandWindowImpl::touchListener = {
    WaylandWindowImpl::OnTouchDown,
    WaylandWindowImpl::OnTouchUp,
    WaylandWindowImpl::OnTouchMotion,
    WaylandWindowImpl::OnTouchFrame,
    WaylandWindowImpl::OnTouchCancel,
    WaylandWindowImpl::OnTouchShape,
    WaylandWindowImpl::OnTouchOrientation,
};

const xdg_toplevel_listener WaylandWindowImpl::toplevelListener = {
    WaylandWindowImpl::OnToplevelConfigure,
    WaylandWindowImpl::OnToplevelClose,
};

WaylandWindowImpl::WaylandWindowImpl(const prev::core::instance::Instance& instance, const WindowInfo& windowInfo)
    : WindowImpl(instance)
{
    LOGI("Creating Wayland Window...");

    m_info = windowInfo;

    m_display = wl_display_connect(nullptr);
    if (!m_display) {
        throw std::runtime_error("Could not connect to the display server.");
    }

    m_registry = wl_display_get_registry(m_display);
    if (!m_registry) {
        throw std::runtime_error("Could not get registry.");
    }

    wl_registry_add_listener(m_registry, &WaylandWindowImpl::registryListener, this);
    wl_display_roundtrip(m_display);

    if (!m_compositor) {
        throw std::runtime_error("Could not bind compositor.");
    }

    m_surface = wl_compositor_create_surface(m_compositor);
    if (!m_surface) {
        throw std::runtime_error("Could not create surface.");
    }

    if (!m_shell) {
        throw std::runtime_error("Could not bind shell.");
    }

    m_shellSurface = xdg_wm_base_get_xdg_surface(m_shell, m_surface);
    if (!m_shellSurface) {
        throw std::runtime_error("Could not get xdg surface.");
    }
    xdg_surface_add_listener(m_shellSurface, &shellSurfaceListener, this);

    m_topLevel = xdg_surface_get_toplevel(m_shellSurface);
    if (!m_topLevel) {
        throw std::runtime_error("Could not get xdg top level.");
    }
    xdg_toplevel_add_listener(m_topLevel, &toplevelListener, this);

    wl_surface_commit(m_surface);
    wl_display_roundtrip(m_display);
    wl_surface_commit(m_surface);

    SetTitle(windowInfo.title);

    m_eventQueue.Push(OnInitEvent());
    m_eventQueue.Push(OnResizeEvent(m_info.size.width, m_info.size.height));
    m_eventQueue.Push(OnFocusEvent(true));
}

WaylandWindowImpl::~WaylandWindowImpl()
{
    if (m_touch) {
        wl_touch_destroy(m_touch);
    }

    if (m_keyboard) {
        wl_keyboard_destroy(m_keyboard);
    }

    if (m_pointer) {
        wl_pointer_destroy(m_pointer);
    }

    if (m_seat) {
        wl_seat_destroy(m_seat);
    }

    if (m_topLevel) {
        xdg_toplevel_destroy(m_topLevel);
    }

    DestroySurface();

    if (m_shellSurface) {
        xdg_surface_destroy(m_shellSurface);
    }

    if (m_surface) {
        wl_surface_destroy(m_surface);
    }

    if (m_shell) {
        xdg_wm_base_destroy(m_shell);
    }

    if (m_compositor) {
        wl_compositor_destroy(m_compositor);
    }

    if (m_registry) {
        wl_registry_destroy(m_registry);
    }

    if (m_display) {
        wl_display_disconnect(m_display);
    }
}

bool WaylandWindowImpl::PollEvent(bool waitForEvent, Event& outEvent)
{
    wl_display_roundtrip(m_display);

    if (!m_eventQueue.IsEmpty()) {
        outEvent = m_eventQueue.Pop(); // Pop message from message queue buffer
        return true;
    }
    return false;
}

void WaylandWindowImpl::SetTitle(const std::string& title)
{
    xdg_toplevel_set_title(m_topLevel, title.c_str());
    xdg_toplevel_set_app_id(m_topLevel, title.c_str());

    wl_display_roundtrip(m_display);
}

void WaylandWindowImpl::SetPosition(int32_t x, int32_t y)
{
}

void WaylandWindowImpl::SetSize(uint32_t w, uint32_t h)
{
}

void WaylandWindowImpl::SetMouseCursorVisible(bool visible)
{
}

Surface& WaylandWindowImpl::CreateSurface()
{
    if (m_vkSurface == VK_NULL_HANDLE) {
        VkWaylandSurfaceCreateInfoKHR waylandSurfaceCreateInfo{ prev::util::vk::CreateStruct<VkWaylandSurfaceCreateInfoKHR>(VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR) };
        waylandSurfaceCreateInfo.flags = 0;
        waylandSurfaceCreateInfo.display = m_display;
        waylandSurfaceCreateInfo.surface = m_surface;

        VKERRCHECK(vkCreateWaylandSurfaceKHR(m_instance, &waylandSurfaceCreateInfo, VK_NULL_HANDLE, &m_vkSurface));
    }
    return *this;
}

// general
void WaylandWindowImpl::OnRegistryAdd(void* data, wl_registry* registry, uint32_t id, const char* interface, uint32_t version)
{
    // pickup wayland objects when they appear
    WaylandWindowImpl* impl = static_cast<WaylandWindowImpl*>(data);
    if (strcmp(interface, wl_compositor_interface.name) == 0) {
        impl->m_compositor = static_cast<wl_compositor*>(wl_registry_bind(registry, id, &wl_compositor_interface, 1));
    } else if (strcmp(interface, xdg_wm_base_interface.name) == 0) {
        impl->m_shell = static_cast<xdg_wm_base*>(wl_registry_bind(registry, id, &xdg_wm_base_interface, 1));
    } else if (strcmp(interface, wl_seat_interface.name) == 0) {
        impl->m_seat = static_cast<wl_seat*>(wl_registry_bind(registry, id, &wl_seat_interface, 1));
        wl_seat_add_listener(impl->m_seat, &seatListener, impl);
    }
}

void WaylandWindowImpl::OnRegistryRemove(void* data, wl_registry* registry, uint32_t name)
{
}

// shell surface
void WaylandWindowImpl::OnConfigure(void* data, xdg_surface* shellSurface, uint32_t serial)
{
    xdg_surface_ack_configure(shellSurface, serial);
}

// top level
void WaylandWindowImpl::OnToplevelConfigure(void* data, xdg_toplevel* toplevel, int32_t width, int32_t height, wl_array* states)
{
    WaylandWindowImpl* impl = static_cast<WaylandWindowImpl*>(data);

    const Size newSize{ static_cast<uint32_t>(width), static_cast<uint32_t>(height) };
    if (newSize != Size{ 0, 0 } && impl->m_info.size != newSize) {
        impl->m_info.size = newSize;
        impl->m_eventQueue.Push(impl->OnResizeEvent(impl->m_info.size.width, impl->m_info.size.height));
    }
}

void WaylandWindowImpl::OnToplevelClose(void* data, xdg_toplevel* toplevel)
{
    WaylandWindowImpl* impl = static_cast<WaylandWindowImpl*>(data);

    impl->m_eventQueue.Push(impl->OnCloseEvent());
}

// pointer
void WaylandWindowImpl::OnPointerEnter(void* data, wl_pointer* pointer, uint32_t serial, wl_surface* surface, wl_fixed_t sx, wl_fixed_t sy)
{
    WaylandWindowImpl* impl = static_cast<WaylandWindowImpl*>(data);

    impl->m_lastMousePosition = { sx, sy };
}

void WaylandWindowImpl::OnPointerLeave(void* data, wl_pointer* pointer, uint32_t serial, wl_surface* surface)
{
    WaylandWindowImpl* impl = static_cast<WaylandWindowImpl*>(data);

    impl->m_lastMousePosition = { 0, 0 };
}

void WaylandWindowImpl::OnPointerMotion(void* data, wl_pointer* pointer, uint32_t time, wl_fixed_t sx, wl_fixed_t sy)
{
    WaylandWindowImpl* impl = static_cast<WaylandWindowImpl*>(data);

    int32_t x{};
    int32_t y{};
    if (impl->m_hasFocus && impl->m_mouseLocked) {
        x = impl->m_lastMousePosition.x - sx;
        y = impl->m_lastMousePosition.y - sy;
    } else {
        x = sx;
        y = sy;
    }

    x /= impl->ResolutionScaleFactor;
    y /= impl->ResolutionScaleFactor;

    const auto btn{ impl->IsMouseButtonPressed(ButtonType::MIDDLE) ? ButtonType::MIDDLE : (impl->IsMouseButtonPressed(ButtonType::RIGHT) ? ButtonType::RIGHT : ButtonType::LEFT) };

    impl->m_eventQueue.Push(impl->OnMouseEvent(ActionType::MOVE, x, y, btn));
    impl->m_lastMousePosition = { sx, sy };
}

void WaylandWindowImpl::OnPointerButton(void* data, wl_pointer* pointer, uint32_t serial, uint32_t time, uint32_t button, uint32_t state)
{
    WaylandWindowImpl* impl = static_cast<WaylandWindowImpl*>(data);

    const auto st{ state == WL_POINTER_BUTTON_STATE_PRESSED ? ActionType::DOWN : ActionType::UP };
    const auto pos{ impl->GetMousePosition() };
    const auto btn{ button == BTN_MIDDLE ? ButtonType::MIDDLE : (button == BTN_RIGHT ? ButtonType::RIGHT : ButtonType::LEFT) };

    impl->m_eventQueue.Push(impl->OnMouseEvent(st, pos.x, pos.y, btn));
}

void WaylandWindowImpl::OnPointerAxis(void* data, wl_pointer* pointer, uint32_t time, uint32_t axis, wl_fixed_t value)
{
    WaylandWindowImpl* impl = static_cast<WaylandWindowImpl*>(data);

    const auto pos{ impl->GetMousePosition() };
    const auto delta{ value / impl->ResolutionScaleFactor }; // TODO - magic number scalcce

    impl->m_eventQueue.Push(impl->OnMouseScrollEvent(delta, pos.x, pos.y));
}

// keyboard
void WaylandWindowImpl::OnKeyboardKeymap(void* data, wl_keyboard* keyboard, uint32_t format, int fd, uint32_t size)
{
}

void WaylandWindowImpl::OnKeyboardEnter(void* data, wl_keyboard* keyboard, uint32_t serial, wl_surface* surface, wl_array* keys)
{
    WaylandWindowImpl* impl = static_cast<WaylandWindowImpl*>(data);

    // TODO - we use keyboard events to detect window focus because top level does not handle it
    impl->m_eventQueue.Push(impl->OnFocusEvent(true));
}

void WaylandWindowImpl::OnKeyboardLeave(void* data, wl_keyboard* keyboard, uint32_t serial, wl_surface* surface)
{
    WaylandWindowImpl* impl = static_cast<WaylandWindowImpl*>(data);

    // TODO - we use keyboard events to detect window focus because top level does not handle it
    impl->m_eventQueue.Push(impl->OnFocusEvent(false));
}

void WaylandWindowImpl::OnKeyboardKey(void* data, wl_keyboard* keyboard, uint32_t serial, uint32_t time, uint32_t key, uint32_t state)
{
    WaylandWindowImpl* impl = static_cast<WaylandWindowImpl*>(data);

    const auto keyCode{ INPUT_TO_HID[key] };
    const auto st{ state == WL_KEYBOARD_KEY_STATE_PRESSED ? ActionType::DOWN : ActionType::UP };

    impl->m_eventQueue.Push(impl->OnKeyEvent(st, keyCode));
}

void WaylandWindowImpl::OnKeyboardModifiers(void* data, wl_keyboard* keyboard, uint32_t serial, uint32_t modsDepressed, uint32_t modsLatched, uint32_t modsLocked, uint32_t group)
{
}

// touch
void WaylandWindowImpl::OnTouchDown(void* data, wl_touch* wl_touch, uint32_t serial, uint32_t time, wl_surface* surface, int32_t id, wl_fixed_t x, wl_fixed_t y)
{
    WaylandWindowImpl* impl = static_cast<WaylandWindowImpl*>(data);

    const Size windowSize{ impl->m_info.size };
    const Position normalizedCoord{ x / impl->ResolutionScaleFactor, y / impl->ResolutionScaleFactor };
    const uint8_t fingerId{ static_cast<uint8_t>(id) };

    impl->m_eventQueue.Push(impl->m_MTouch.OnEvent(ActionType::DOWN, normalizedCoord.x, normalizedCoord.y, fingerId, windowSize.width, windowSize.height));
}

void WaylandWindowImpl::OnTouchUp(void* data, wl_touch* wl_touch, uint32_t serial, uint32_t time, int32_t id)
{
    WaylandWindowImpl* impl = static_cast<WaylandWindowImpl*>(data);

    const auto pointer{ impl->m_MTouch.GetPointer(id) };

    const Size windowSize{ impl->m_info.size };
    const Position normalizedCoord{ static_cast<int32_t>(pointer.x), static_cast<int32_t>(pointer.y) };
    const uint8_t fingerId{ static_cast<uint8_t>(id) };

    impl->m_eventQueue.Push(impl->m_MTouch.OnEvent(ActionType::UP, normalizedCoord.x, normalizedCoord.y, fingerId, windowSize.width, windowSize.height));
}

void WaylandWindowImpl::OnTouchMotion(void* data, wl_touch* wl_touch, uint32_t time, int32_t id, wl_fixed_t x, wl_fixed_t y)
{
    WaylandWindowImpl* impl = static_cast<WaylandWindowImpl*>(data);

    const Size windowSize{ impl->m_info.size };
    const Position normalizedCoord{ x / impl->ResolutionScaleFactor, y / impl->ResolutionScaleFactor };
    const uint8_t fingerId{ static_cast<uint8_t>(id) };

    impl->m_eventQueue.Push(impl->m_MTouch.OnEvent(ActionType::MOVE, normalizedCoord.x, normalizedCoord.y, fingerId, windowSize.width, windowSize.height));
}

void WaylandWindowImpl::OnTouchFrame(void* data, wl_touch* wl_touch)
{
}

void WaylandWindowImpl::OnTouchCancel(void* data, wl_touch* wl_touch)
{
    WaylandWindowImpl* impl = static_cast<WaylandWindowImpl*>(data);

    impl->m_MTouch.Reset();
}

void WaylandWindowImpl::OnTouchShape(void* data, wl_touch* wl_touch, int32_t id, wl_fixed_t major, wl_fixed_t minor)
{
}

void WaylandWindowImpl::OnTouchOrientation(void* data, wl_touch* wl_touch, int32_t id, wl_fixed_t orientation)
{
}

// seat
void WaylandWindowImpl::OnSeatCapabilities(void* data, wl_seat* seat, uint32_t caps)
{
    WaylandWindowImpl* impl = static_cast<WaylandWindowImpl*>(data);

    const bool hasMouse{ !!(caps & WL_SEAT_CAPABILITY_POINTER) };
    if (hasMouse && !impl->m_pointer) {
        impl->m_pointer = wl_seat_get_pointer(seat);
        wl_pointer_add_listener(impl->m_pointer, &pointerListener, impl);
    } else if (!hasMouse && impl->m_pointer) {
        wl_pointer_destroy(impl->m_pointer);
        impl->m_pointer = nullptr;
    }

    const bool hasKeyboard{ !!(caps & WL_SEAT_CAPABILITY_KEYBOARD) };
    if (hasKeyboard && !impl->m_keyboard) {
        impl->m_keyboard = wl_seat_get_keyboard(seat);
        wl_keyboard_add_listener(impl->m_keyboard, &keyboardListener, impl);
    } else if (!hasKeyboard && impl->m_keyboard) {
        wl_keyboard_destroy(impl->m_keyboard);
        impl->m_keyboard = nullptr;
    }

    const bool hasTouch{ !!(caps & WL_SEAT_CAPABILITY_TOUCH) };
    if (hasTouch && !impl->m_touch) {
        impl->m_touch = wl_seat_get_touch(seat);
        wl_touch_add_listener(impl->m_touch, &touchListener, impl);
    } else if (!hasTouch && impl->m_touch) {
        wl_touch_destroy(impl->m_touch);
        impl->m_touch = nullptr;
    }
}

} // namespace prev::window::impl::wayland

#endif