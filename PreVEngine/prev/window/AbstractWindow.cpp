#include "AbstractWindow.h"

#include "impl/WindowImplFactory.h"

namespace prev::window {
AbstractWindow::AbstractWindow(const WindowCreateInfo& createInfo)
{
    m_windowImpl = impl::WindowImplFactory{}.Create(impl::WindowInfo{ createInfo.title, { createInfo.left, createInfo.top }, { createInfo.width, createInfo.height }, createInfo.fullScreen });
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
    const auto& shape{ m_windowImpl->GetInfo() };
    return shape.position;
}

impl::Size AbstractWindow::GetSize() const
{
    const auto& shape{ m_windowImpl->GetInfo() };
    return shape.size;
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

void AbstractWindow::SetTitle(const std::string& title)
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