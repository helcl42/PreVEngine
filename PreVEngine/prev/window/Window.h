#ifndef __WINDOW_H__
#define __WINDOW_H__

#include "IWindow.h"
#include "WindowCommon.h"
#include "WindowEvents.h"

#include "impl/WindowImpl.h"

#include "../event/EventHandler.h"
#include "../input/keyboard/KeyboardEvents.h"
#include "../input/mouse/MouseEvents.h"
#include "../input/touch/TouchEvents.h"

#include <memory>

namespace prev::window {
class Window final : public IWindow {
public:
    Window(const WindowCreateInfo& createInfo);

    virtual ~Window() = default;

public:
    impl::Surface& GetSurface(VkInstance instance) override;

    bool CanPresent(VkPhysicalDevice gpu, uint32_t queueFamily) const override;

    impl::Position GetPosition() const override;

    impl::Size GetSize() const override;

    bool IsKeyPressed(const prev::input::keyboard::KeyCode key) const override;

    bool IsMouseButtonPressed(const impl::ButtonType btn) const override;

    impl::Position GetMousePosition() const override;

    bool HasFocus() const override;

    bool IsMouseLocked() const override;

    bool IsMouseCursorVisible() const override;

    void SetTitle(const std::string& title) override;

    void SetPosition(const impl::Position& position) override;

    void SetSize(const impl::Size& size) override;

    void ShowKeyboard(bool enabled) override;

    void SetMouseLocked(bool locked) override;

    void SetMouseCursorVisible(bool visible) override;

    void Close() override;

public:
    impl::Event GetEvent(bool waitForEvent = false) override; // Return a single event from the queue (Alternative to using ProcessEvents.)

    bool ProcessEvents(bool waitForEvent = false) override; // Poll events, and call event handlers. Returns false if window is closing.

public:
    void operator()(const prev::input::mouse::MouseLockRequest& mouseLock);

    void operator()(const prev::input::mouse::MouseCursorVisibilityRequest& cursorVisibility);

private:
    bool ProcessEvent(const impl::Event& e);

private:
    std::unique_ptr<impl::WindowImpl> m_windowImpl{};

private:
    prev::event::EventHandler<Window, prev::input::mouse::MouseLockRequest> m_mouseLockHandler{ *this };

    prev::event::EventHandler<Window, prev::input::mouse::MouseCursorVisibilityRequest> m_mouseCursorVisibilityHandler{ *this };
};

} // namespace prev::window

#endif