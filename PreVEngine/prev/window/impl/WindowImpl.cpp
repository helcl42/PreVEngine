#include "WindowImpl.h"

#include "../../core/Core.h"

namespace prev::window::impl {
WindowImpl::WindowImpl(const prev::core::instance::Instance& instance)
    : m_instance{ instance }
    , m_running{false }
    , m_hasTextInput{ false }
    , m_hasFocus{ false }
    , m_mouseLocked{ false }
    , m_mouseCursorVisible{ true }
{
}

WindowImpl::~WindowImpl()
{
    DestroySurface();
}

bool WindowImpl::IsKeyPressed(const prev::input::keyboard::KeyCode key) const
{
    const auto keyIndex{ static_cast<uint32_t>(key) };
    return m_keyboardKeysState[keyIndex];
}

bool WindowImpl::IsMouseButtonPressed(const ButtonType btn) const
{
    const auto buttonIndex{ static_cast<uint32_t>(btn) };
    return m_mouseButtonsState[buttonIndex];
}

Position WindowImpl::GetMousePosition() const
{
    return m_mousePosition;
}

bool WindowImpl::HasFocus() const
{
    return m_hasFocus;
}

const WindowInfo& WindowImpl::GetInfo() const
{
    return m_info;
}

bool WindowImpl::IsRunning() const
{
    return m_running;
}

bool WindowImpl::IsMouseLocked() const
{
    return m_mouseLocked;
}

void WindowImpl::SetMouseLocked(bool locked)
{
    m_mouseLocked = locked;
}

bool WindowImpl::IsMouseCursorVisible() const
{
    return m_mouseCursorVisible;
}

void WindowImpl::DestroySurface()
{
    if (m_vkSurface != VK_NULL_HANDLE) {
        vkDestroySurfaceKHR(m_instance, m_vkSurface, nullptr);
        m_vkSurface = VK_NULL_HANDLE;
    }
}

void WindowImpl::SetTextInput(bool enabled)
{
    m_hasTextInput = enabled;
}

bool WindowImpl::HasTextInput() const
{
    return m_hasTextInput;
}

void WindowImpl::Close()
{
    m_eventQueue.Push(OnCloseEvent());
}

Event WindowImpl::OnMouseEvent(ActionType action, int32_t x, int32_t y, ButtonType btn)
{
    m_mousePosition = { x, y };
    if (action != ActionType::MOVE) {
        uint32_t buttonIndex = static_cast<uint32_t>(btn);
        m_mouseButtonsState[buttonIndex] = (action == ActionType::DOWN); // Keep track of button state
    }

    Event e{ Event::EventType::MOUSE };
    e.mouse.action = action;
    e.mouse.x = x;
    e.mouse.y = y;
    e.mouse.btn = btn;
    e.mouse.w = m_info.size.width;
    e.mouse.h = m_info.size.height;
    return e;
}

Event WindowImpl::OnMouseScrollEvent(int32_t delta, int32_t x, int32_t y)
{
    m_mousePosition = { x, y };

    Event e{ Event::EventType::MOUSE_SCROLL };
    e.scroll.delta = delta;
    e.scroll.x = x;
    e.scroll.y = y;
    return e;
}

Event WindowImpl::OnKeyEvent(ActionType action, uint8_t key)
{
    m_keyboardKeysState[key] = (action == ActionType::DOWN);

    Event e{ Event::EventType::KEY };
    e.key = { action, (prev::input::keyboard::KeyCode)key };
    return e;
}

Event WindowImpl::OnTextEvent(const char* str)
{
    Event e{ Event::EventType::TEXT };
    e.text.str = str;
    return e;
}

Event WindowImpl::OnMoveEvent(int32_t x, int32_t y)
{
    m_info.position = { x, y };

    Event e{ Event::EventType::MOVE };
    e.move = { x, y };
    return e;
}

Event WindowImpl::OnResizeEvent(uint32_t width, uint32_t height)
{
    m_info.size = { width, height };

    Event e{ Event::EventType::RESIZE };
    e.resize = { width, height };
    return e;
}

Event WindowImpl::OnFocusEvent(bool hasFocus)
{
    m_hasFocus = hasFocus;

    Event e{ Event::EventType::FOCUS };
    e.focus.hasFocus = hasFocus;
    return e;
}

Event WindowImpl::OnInitEvent()
{
    m_running = true;
    return { Event::EventType::INIT };
}

Event WindowImpl::OnCloseEvent()
{
    m_running = false;
    return { Event::EventType::CLOSE };
}

Event WindowImpl::OnChangeEvent()
{
    return { Event::EventType::CHANGE };
}
} // namespace prev::window::impl