#include "Window.h"
#include "InputConvertor.h"
#include "impl/WindowImplFactory.h"

namespace prev::window {
Window::Window(const prev::core::instance::Instance& instance, const WindowCreateInfo& createInfo)
{
    m_windowImpl = impl::WindowImplFactory{}.Create(instance, impl::WindowInfo{ createInfo.title, { createInfo.left, createInfo.top }, { createInfo.width, createInfo.height }, createInfo.fullScreen });
}

impl::Surface& Window::GetSurface()
{
    m_windowImpl->CreateSurface();
    return *m_windowImpl;
}

impl::Position Window::GetPosition() const
{
    const auto& shape{ m_windowImpl->GetInfo() };
    return shape.position;
}

impl::Size Window::GetSize() const
{
    const auto& shape{ m_windowImpl->GetInfo() };
    return shape.size;
}

bool Window::IsKeyPressed(const prev::input::keyboard::KeyCode key) const
{
    return m_windowImpl->IsKeyPressed(key);
}

bool Window::IsMouseButtonPressed(const impl::ButtonType btn) const
{
    return m_windowImpl->IsMouseButtonPressed(btn);
}

impl::Position Window::GetMousePosition() const
{
    return m_windowImpl->GetMousePosition();
}

bool Window::HasFocus() const
{
    return m_windowImpl->HasFocus();
}

bool Window::IsMouseLocked() const
{
    return m_windowImpl->IsMouseLocked();
}

bool Window::IsMouseCursorVisible() const
{
    return m_windowImpl->IsMouseCursorVisible();
}

void Window::SetTitle(const std::string& title)
{
    m_windowImpl->SetTitle(title);
}

void Window::SetPosition(const impl::Position& position)
{
    m_windowImpl->SetPosition(position.x, position.y);
}

void Window::SetSize(const impl::Size& size)
{
    m_windowImpl->SetSize(size.width, size.height);
}

void Window::ShowKeyboard(bool enabled) // On Android, show the soft-keyboard.
{
    m_windowImpl->SetTextInput(enabled);
}

void Window::SetMouseLocked(bool locked)
{
    m_windowImpl->SetMouseLocked(locked);
}

void Window::SetMouseCursorVisible(bool visible)
{
    m_windowImpl->SetMouseCursorVisible(visible);
}

void Window::Close()
{
    m_windowImpl->Close();
}

impl::Event Window::GetEvent(bool waitForEvent)
{
    return m_windowImpl->GetEvent(waitForEvent);
}

bool Window::ProcessEvents(bool waitForEvent)
{
    impl::Event e = m_windowImpl->GetEvent(waitForEvent);
    while (e.tag != impl::Event::EventType::NONE) {
        if (!ProcessEvent(e)) {
            return false;
        }
        e = m_windowImpl->GetEvent();
    }
    return m_windowImpl->IsRunning();
}

void Window::operator()(const prev::input::mouse::MouseLockRequest& mouseLock)
{
    SetMouseLocked(mouseLock.lock);
}

void Window::operator()(const prev::input::mouse::MouseCursorVisibilityRequest& cursorVisibility)
{
    SetMouseCursorVisible(cursorVisibility.visible);
}

bool Window::ProcessEvent(const impl::Event& e)
{
    switch (e.tag) {
    case impl::Event::EventType::MOUSE:
        prev::event::EventChannel::Post(prev::input::mouse::MouseEvent{ InputConvertor::GetMouseActionType(e.mouse.action), InputConvertor::GetMouseButtonType(e.mouse.btn), glm::vec2(e.mouse.x, e.mouse.y), glm::vec2(e.mouse.w, e.mouse.h) });
        break;
    case impl::Event::EventType::MOUSE_SCROLL:
        prev::event::EventChannel::Post(prev::input::mouse::MouseScrollEvent{ e.scroll.delta, glm::vec2(e.scroll.x, e.scroll.y) });
        break;
    case impl::Event::EventType::KEY:
        prev::event::EventChannel::Post(prev::input::keyboard::KeyEvent{ InputConvertor::GetKeyActionType(e.key.action), e.key.keyCode });
        break;
    case impl::Event::EventType::TEXT:
        prev::event::EventChannel::Post(prev::input::keyboard::TextEvent{ e.text.str });
        break;
    case impl::Event::EventType::MOVE:
        prev::event::EventChannel::Post(WindowMovedEvent{ this, glm::vec2(e.move.x, e.move.y) });
        break;
    case impl::Event::EventType::RESIZE:
        prev::event::EventChannel::Post(WindowResizeEvent{ this, e.resize.width, e.resize.height });
        break;
    case impl::Event::EventType::FOCUS:
        prev::event::EventChannel::Post(WindowFocusChangeEvent{ this, e.focus.hasFocus });
        break;
    case impl::Event::EventType::TOUCH:
        prev::event::EventChannel::Post(prev::input::touch::TouchEvent{ InputConvertor::GetTouchActionType(e.touch.action), e.touch.id, glm::vec2(e.touch.x, e.touch.y), glm::vec2(e.touch.w, e.touch.h) });
        break;
    case impl::Event::EventType::INIT:
        prev::event::EventChannel::Post(WindowCreatedEvent{ this });
        break;
    case impl::Event::EventType::CHANGE:
        prev::event::EventChannel::Post(WindowChangeEvent{ this });
        break;
    case impl::Event::EventType::CLOSE:
        prev::event::EventChannel::Post(WindowDestroyedEvent{ this });
        return false;
    default:
        break;
    }
    return true;
}
} // namespace prev::window