#include "Window.h"

#include "../event/EventChannel.h"

namespace prev::window {
Window::Window(const WindowCreateInfo& createInfo)
    : AbstractWindow(createInfo)
{
}

void Window::OnInitEvent()
{
    prev::event::EventChannel::Post(WindowCreatedEvent{ this });
}

void Window::OnCloseEvent()
{
    prev::event::EventChannel::Post(WindowDestroyedEvent{ this });
}

void Window::OnChangeEvent()
{
    prev::event::EventChannel::Post(WindowChangeEvent{ this });
}

void Window::OnResizeEvent(uint32_t width, uint32_t height)
{
    prev::event::EventChannel::Post(WindowResizeEvent{ this, width, height });
}

void Window::OnMoveEvent(int32_t x, int32_t y)
{
    prev::event::EventChannel::Post(WindowMovedEvent{ this, glm::vec2(x, y) });
}

void Window::OnFocusEvent(bool hasFocus)
{
    prev::event::EventChannel::Post(WindowFocusChangeEvent{ this, hasFocus });
}

void Window::OnKeyEvent(impl::ActionType action, prev::input::keyboard::KeyCode keyCode)
{
    prev::event::EventChannel::Post(prev::input::keyboard::KeyEvent{ InputConvertor::GetKeyActionType(action), keyCode });
}

void Window::OnMouseEvent(impl::ActionType action, int32_t x, int32_t y, impl::ButtonType button, uint32_t w, uint32_t h)
{
    prev::event::EventChannel::Post(prev::input::mouse::MouseEvent{ InputConvertor::GetMouseActionType(action), InputConvertor::GetMouseButtonType(button), glm::vec2(x, y), glm::vec2(w, h) });
}

void Window::OnMouseScrollEvent(int32_t delta, int32_t x, int32_t y)
{
    prev::event::EventChannel::Post(prev::input::mouse::MouseScrollEvent{ delta, glm::vec2(x, y) });
}

void Window::OnTouchEvent(impl::ActionType action, float x, float y, uint8_t pointerId, float w, float h)
{
    prev::event::EventChannel::Post(prev::input::touch::TouchEvent{ InputConvertor::GetTouchActionType(action), pointerId, glm::vec2(x, y), glm::vec2(w, h) });
}

void Window::OnTextEvent(const char* str)
{
    prev::event::EventChannel::Post(prev::input::keyboard::TextEvent{ str });
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