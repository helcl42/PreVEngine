#include "AbstractWindow.h"
#include "impl/android//WindowAndroid.h"
#include "impl/linux/WindowXcb.h"
#include "impl/windows/WindowWin32.h"

// change const char* to const std::string& 
// create factory for WindowImpl
// remove Abstract Window vs. WIndow -> no need to inherit here !!!

namespace prev::window {
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
        m_windowImpl = std::make_shared<prev::window::impl::xcb::WindowXcb>(title);
    } else {
        m_windowImpl = std::make_shared<prev::window::impl::xcb::WindowXcb>(title, width, height);
    }
#elif VK_USE_PLATFORM_WIN32_KHR
    LOGI("PLATFORM: WIN32\n");
    if (tryFullscreen) {
        m_windowImpl = std::make_shared<prev::window::impl::windows::WindowWin32>(title);
    } else {
        m_windowImpl = std::make_shared<prev::window::impl::windows::WindowWin32>(title, width, height);
    }
#elif VK_USE_PLATFORM_ANDROID_KHR
    LOGI("PLATFORM: ANDROID\n");
    m_windowImpl = std::make_shared<prev::window::impl::android::WindowAndroid>(title, width, height);
#else
#error NOT IMPLEMENTED PLATFORM
#endif
    // TODO:
    //    #ifdef VK_USE_PLATFORM_XLIB_KHR
    //    #ifdef VK_USE_PLATFORM_MIR_KHR
    //    #ifdef VK_USE_PLATFORM_WAYLAND_KHR
}

impl::Surface& AbstractWindow::GetSurface(VkInstance instance)
{
    m_windowImpl->CreateSurface(instance);

    return *m_windowImpl;
}

bool AbstractWindow::CanPresent(VkPhysicalDevice gpu, uint32_t queueFamily) const
{
    return m_windowImpl->CanPresent(gpu, queueFamily);
}

impl::Position AbstractWindow::GetPosition() const
{
    const auto& shape = m_windowImpl->GetShape();

    return impl::Position{ shape.x, shape.y };
}

impl::Size AbstractWindow::GetSize() const
{
    const auto& shape = m_windowImpl->GetShape();

    return impl::Size{ shape.width, shape.height };
}

bool AbstractWindow::IsKeyPressed(const prev::input::keyboard::KeyCode key) const
{
    return m_windowImpl->IsKeyPressed(key);
}

bool AbstractWindow::IsMouseButtonPressed(const impl::ButtonType btn) const
{
    return m_windowImpl->IsMouseButtonPressed(btn);
}

impl::Position AbstractWindow::GetMousePosition() const
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

void AbstractWindow::SetPosition(const impl::Position& position)
{
    m_windowImpl->SetPosition(position.x, position.y);
}

void AbstractWindow::SetSize(const impl::Size& size)
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

impl::Event AbstractWindow::GetEvent(bool waitForEvent)
{
    return m_windowImpl->GetEvent(waitForEvent);
}

bool AbstractWindow::ProcessEvents(bool waitForEvent)
{
    impl::Event e = m_windowImpl->GetEvent(waitForEvent);
    while (e.tag != impl::Event::EventType::NONE) {
        // Calling the event handlers
        switch (e.tag) {
        case impl::Event::EventType::MOUSE:
            OnMouseEvent(e.mouse.action, e.mouse.x, e.mouse.y, e.mouse.btn, e.mouse.w, e.mouse.h);
            break;
        case impl::Event::EventType::MOUSE_SCROLL:
            OnMouseScrollEvent(e.scroll.delta, e.scroll.x, e.scroll.y);
            break;
        case impl::Event::EventType::KEY:
            OnKeyEvent(e.key.action, e.key.keycode);
            break;
        case impl::Event::EventType::TEXT:
            OnTextEvent(e.text.str);
            break;
        case impl::Event::EventType::MOVE:
            OnMoveEvent(e.move.x, e.move.y);
            break;
        case impl::Event::EventType::RESIZE:
            OnResizeEvent(e.resize.width, e.resize.height);
            break;
        case impl::Event::EventType::FOCUS:
            OnFocusEvent(e.focus.hasFocus);
            break;
        case impl::Event::EventType::TOUCH:
            OnTouchEvent(e.touch.action, e.touch.x, e.touch.y, e.touch.id, e.touch.w, e.touch.h);
            break;
        case impl::Event::EventType::INIT:
            OnInitEvent();
            break;
        case impl::Event::EventType::CHANGE:
            OnChangeEvent();
            break;
        case impl::Event::EventType::CLOSE:
            OnCloseEvent();
            return false;
        default:
            break;
        }

        e = m_windowImpl->GetEvent();
    }

    return m_windowImpl->IsRunning();
}
} // namespace prev::window