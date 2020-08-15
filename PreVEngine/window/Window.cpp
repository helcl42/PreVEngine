#include "Window.h"
#include "WindowImpl.h"
#include "android//WindowAndroid.h"
#include "linux/WindowXcb.h"
#include "windows/WindowWin32.h"

namespace prev {
AbstractWindow::AbstractWindow(const char* title)
{
    InitWindow(title, 640, 480, true);
}

AbstractWindow::AbstractWindow(const char* title, const uint32_t width, const uint32_t height)
{
    InitWindow(title, width, height, false);
}

void AbstractWindow::InitWindow(const char* title, const uint32_t width, const uint32_t height, bool tryFullscreen)
{
#ifdef VK_USE_PLATFORM_XCB_KHR
    LOGI("PLATFORM: XCB\n");
    if (tryFullscreen) {
        m_windowImpl = std::make_shared<WindowXcb>(title);
    } else {
        m_windowImpl = std::make_shared<WindowXcb>(title, width, height);
    }
#elif VK_USE_PLATFORM_WIN32_KHR
    LOGI("PLATFORM: WIN32\n");
    if (tryFullscreen) {
        m_windowImpl = std::make_shared<WindowWin32>(title);
    } else {
        m_windowImpl = std::make_shared<WindowWin32>(title, width, height);
    }
#elif VK_USE_PLATFORM_ANDROID_KHR
    LOGI("PLATFORM: ANDROID\n");
    m_windowImpl = std::make_shared<WindowAndroid>(title, width, height);
#else
#error NOT IMPLEMENTED PLATFORM
#endif
    // TODO:
    //    #ifdef VK_USE_PLATFORM_XLIB_KHR
    //    #ifdef VK_USE_PLATFORM_MIR_KHR
    //    #ifdef VK_USE_PLATFORM_WAYLAND_KHR
}

Surface& AbstractWindow::GetSurface(VkInstance instance)
{
    m_windowImpl->CreateSurface(instance);

    return *m_windowImpl;
}

bool AbstractWindow::CanPresent(VkPhysicalDevice gpu, uint32_t queueFamily) const
{
    return m_windowImpl->CanPresent(gpu, queueFamily);
}

Position AbstractWindow::GetPosition() const
{
    const auto& shape = m_windowImpl->GetShape();

    return Position{ shape.x, shape.y };
}

Size AbstractWindow::GetSize() const
{
    const auto& shape = m_windowImpl->GetShape();

    return Size{ shape.width, shape.height };
}

bool AbstractWindow::IsKeyPressed(const KeyCode key) const
{
    return m_windowImpl->IsKeyPressed(key);
}

bool AbstractWindow::IsMouseButtonPressed(const ButtonType btn) const
{
    return m_windowImpl->IsMouseButtonPressed(btn);
}

Position AbstractWindow::GetMousePosition() const
{
    return m_windowImpl->GetMousePosition();
}

bool AbstractWindow::HasFocus() const
{
    return m_windowImpl->HasFocus();
}

bool AbstractWindow::IsMouseLocked() const
{
    return m_windowImpl->IsMouseLocked();
}

bool AbstractWindow::IsMouseCursorVisible() const
{
    return m_windowImpl->IsMouseCursorVisible();
}

void AbstractWindow::SetTitle(const char* title)
{
    m_windowImpl->SetTitle(title);
}

void AbstractWindow::SetPosition(const Position& position)
{
    m_windowImpl->SetPosition(position.x, position.y);
}

void AbstractWindow::SetSize(const Size& size)
{
    m_windowImpl->SetSize(size.width, size.height);
}

void AbstractWindow::ShowKeyboard(bool enabled) // On Android, show the soft-keyboard.
{
    m_windowImpl->SetTextInput(enabled);
}

void AbstractWindow::SetMouseLocked(bool locked)
{
    m_windowImpl->SetMouseLocked(locked);
}

void AbstractWindow::SetMouseCursorVisible(bool visible)
{
    m_windowImpl->SetMouseCursorVisible(visible);
}

void AbstractWindow::Close()
{
    m_windowImpl->Close();
}

Event AbstractWindow::GetEvent(bool waitForEvent)
{
    return m_windowImpl->GetEvent(waitForEvent);
}

bool AbstractWindow::ProcessEvents(bool waitForEvent)
{
    Event e = m_windowImpl->GetEvent(waitForEvent);
    while (e.tag != Event::EventType::NONE) {
        // Calling the event handlers
        switch (e.tag) {
        case Event::EventType::MOUSE:
            OnMouseEvent(e.mouse.action, e.mouse.x, e.mouse.y, e.mouse.btn, e.mouse.w, e.mouse.h);
            break;
        case Event::EventType::MOUSE_SCROLL:
            OnMouseScrollEvent(e.scroll.delta, e.scroll.x, e.scroll.y);
            break;
        case Event::EventType::KEY:
            OnKeyEvent(e.key.action, e.key.keycode);
            break;
        case Event::EventType::TEXT:
            OnTextEvent(e.text.str);
            break;
        case Event::EventType::MOVE:
            OnMoveEvent(e.move.x, e.move.y);
            break;
        case Event::EventType::RESIZE:
            OnResizeEvent(e.resize.width, e.resize.height);
            break;
        case Event::EventType::FOCUS:
            OnFocusEvent(e.focus.hasFocus);
            break;
        case Event::EventType::TOUCH:
            OnTouchEvent(e.touch.action, e.touch.x, e.touch.y, e.touch.id, e.touch.w, e.touch.h);
            break;
        case Event::EventType::INIT:
            OnInitEvent();
            break;
        case Event::EventType::CHANGE:
            OnChangeEvent();
            break;
        case Event::EventType::CLOSE:
            OnCloseEvent();
            return false;
        default:
            break;
        }

        e = m_windowImpl->GetEvent();
    }

    return m_windowImpl->IsRunning();
}
} // namespace prev