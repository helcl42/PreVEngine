#include "WindowImpl.h"

namespace PreVEngine {
WindowImpl::WindowImpl()
    : m_isRunning(false)
    , m_hasTextInput(false)
    , m_hasFocus(false)
    , m_mouseLocked(false)
    , m_mouseCursorVisible(true)
{
}

WindowImpl::~WindowImpl()
{
    if (m_vkSurface != VK_NULL_HANDLE) {
        vkDestroySurfaceKHR(m_vkInstance, m_vkSurface, nullptr);
        m_vkSurface = VK_NULL_HANDLE;
    }
}

Event WindowImpl::OnMouseEvent(ActionType action, int16_t x, int16_t y, ButtonType btn)
{
    m_mousePosition = { x, y };
    if (action != ActionType::MOVE) {
        uint32_t buttonIndex = static_cast<uint32_t>(btn);
        m_mouseButtonsState[buttonIndex] = (action == ActionType::DOWN); // Keep track of button state
    }

    Event e = { Event::EventType::MOUSE, { action, x, y, btn, (int16_t)m_shape.width, (int16_t)m_shape.height } };
    return e;
}

Event WindowImpl::OnMouseScrollEvent(int16_t delta, int16_t x, int16_t y)
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
    e.key = { action, (KeyCode)key };
    return e;
}

Event WindowImpl::OnTextEvent(const char* str)
{
    Event e{ Event::EventType::TEXT };
    e.text.str = str;
    return e;
}

Event WindowImpl::OnMoveEvent(int16_t x, int16_t y)
{
    m_shape.x = x;
    m_shape.y = y;

    Event e{ Event::EventType::MOVE };
    e.move = { x, y };
    return e;
}

Event WindowImpl::OnResizeEvent(uint16_t width, uint16_t height)
{
    m_shape.width = width;
    m_shape.height = height;

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
    m_isRunning = true;
    return { Event::EventType::INIT };
}

Event WindowImpl::OnCloseEvent()
{
    m_isRunning = false;
    return { Event::EventType::CLOSE };
}

Event WindowImpl::OnChangeEvent()
{
    if (m_vkSurface != VK_NULL_HANDLE) {
        vkDestroySurfaceKHR(m_vkInstance, m_vkSurface, nullptr);
        m_vkSurface = VK_NULL_HANDLE;
    }

    CreateSurface(m_vkInstance);

    return { Event::EventType::CHANGE };
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

const WindowShape& WindowImpl::GetShape() const
{
    return m_shape;
}

bool WindowImpl::IsRunning() const
{
    return m_isRunning;
}

bool WindowImpl::IsKeyPressed(const KeyCode key) const
{
    uint32_t keyIndex = static_cast<uint32_t>(key);
    return m_keyboardKeysState[keyIndex];
}

bool WindowImpl::IsMouseButtonPressed(const ButtonType btn) const
{
    uint32_t buttonIndex = static_cast<uint32_t>(btn);
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

Surface::Surface()
{
}

Surface::~Surface()
{
}

Surface::operator VkSurfaceKHR() const // Use *this as a VkSurfaceKHR
{
    return m_vkSurface;
}

bool Surface::CanPresent(VkPhysicalDevice gpu, uint32_t queueFamily) const
{
    VkBool32 canPresent = false;
    VKERRCHECK(vkGetPhysicalDeviceSurfaceSupportKHR(gpu, queueFamily, m_vkSurface, &canPresent));
    return !!canPresent;
}
} // namespace PreVEngine