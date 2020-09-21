#ifndef __IWINDOW_H__
#define __IWINDOW_H__

#include "../common/Common.h"
#include "../core/instance/Validation.h"
#include "../input/keyboard/KeyCodes.h"

#include "impl/Surface.h"
#include "impl/WindowIImplCommon.h"

namespace prev::window {
class IWindow {
public:
    virtual impl::Surface& GetSurface(VkInstance instance) = 0;

    virtual bool CanPresent(VkPhysicalDevice gpu, uint32_t queueFamily) const = 0;

    virtual impl::Position GetPosition() const = 0;

    virtual impl::Size GetSize() const = 0;

    virtual bool IsKeyPressed(const prev::input::keyboard::KeyCode key) const = 0;

    virtual bool IsMouseButtonPressed(const impl::ButtonType btn) const = 0;

    virtual bool IsMouseLocked() const = 0;

    virtual bool IsMouseCursorVisible() const = 0;

    virtual impl::Position GetMousePosition() const = 0;

    virtual bool HasFocus() const = 0;

    virtual void SetTitle(const char* title) = 0;

    virtual void SetPosition(const impl::Position& position) = 0;

    virtual void SetSize(const impl::Size& size) = 0;

    virtual void ShowKeyboard(bool enabled) = 0;

    virtual void SetMouseLocked(bool locked) = 0;

    virtual void SetMouseCursorVisible(bool visible) = 0;

    virtual void Close() = 0;

    virtual impl::Event GetEvent(bool waitForEvent = false) = 0;

    virtual bool ProcessEvents(bool waitForEvent = false) = 0;

public:
    virtual ~IWindow() = default;
};
} // namespace prev::window

#endif