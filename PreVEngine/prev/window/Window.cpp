#include "Window.h"

namespace prev::window {
Window::Window(const char* title)
    : AbstractWindow(title)
{
}

Window::Window(const char* title, const uint32_t width, const uint32_t height)
    : AbstractWindow(title, width, height)
{
}

void Window::OnInitEvent()
{
    prev::event::EventChannel::Broadcast(WindowCreatedEvent{ this });
}

void Window::OnCloseEvent()
{
    prev::event::EventChannel::Broadcast(WindowDestroyedEvent{ this });
}

void Window::OnChangeEvent()
{
    prev::event::EventChannel::Broadcast(WindowChangeEvent{ this });
}

void Window::OnResizeEvent(uint16_t width, uint16_t height)
{
    prev::event::EventChannel::Broadcast(WindowResizeEvent{ this, width, height });
}

void Window::OnMoveEvent(int16_t x, int16_t y)
{
    prev::event::EventChannel::Broadcast(WindowMovedEvent{ this, glm::vec2(x, y) });
}

void Window::OnFocusEvent(bool hasFocus)
{
    prev::event::EventChannel::Broadcast(WindowFocusChangeEvent{ this, hasFocus });
}

void Window::OnKeyEvent(impl::ActionType action, prev::input::keyboard::KeyCode keyCode)
{
    prev::event::EventChannel::Broadcast(prev::input::keyboard::KeyEvent{ InputConvertor::GetKeyActionType(action), keyCode });
}

void Window::OnMouseEvent(impl::ActionType action, int16_t x, int16_t y, impl::ButtonType button, int16_t w, int16_t h)
{
    prev::event::EventChannel::Broadcast(prev::input::mouse::MouseEvent{ InputConvertor::GetMouseActionType(action), InputConvertor::GetMouseButtonType(button), glm::vec2(x, y), glm::vec2(w, h) });
}

void Window::OnMouseScrollEvent(int16_t delta, int16_t x, int16_t y)
{
    prev::event::EventChannel::Broadcast(prev::input::mouse::MouseScrollEvent{ delta, glm::vec2(x, y) });
}

void Window::OnTouchEvent(impl::ActionType action, float x, float y, uint8_t pointerId, float w, float h)
{
    prev::event::EventChannel::Broadcast(prev::input::touch::TouchEvent{ InputConvertor::GetTouchActionType(action), pointerId, glm::vec2(x, y), glm::vec2(w, h) });
}

void Window::OnTextEvent(const char* str)
{
    prev::event::EventChannel::Broadcast(prev::input::keyboard::TextEvent{ str });
}

void Window::operator()(const prev::input::mouse::MouseLockRequest& mouseLock)
{
    SetMouseLocked(mouseLock.lock);
}

void Window::operator()(const prev::input::mouse::MouseCursorVisibilityRequest& cursorVisibility)
{
    SetMouseCursorVisible(cursorVisibility.visible);
}

} // namespace prev::window