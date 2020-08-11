#ifndef __IWINDOW_H__
#define __IWINDOW_H__

#include "../common/Common.h"
#include "../core/instance/Validation.h"
#include "../input/keyboard/KeyCodes.h"

#include "WindowCommon.h"
#include "Surface.h"

namespace PreVEngine {
class IWindow {
public:
    virtual Surface& GetSurface(VkInstance instance) = 0;

    virtual bool CanPresent(VkPhysicalDevice gpu, uint32_t queueFamily) const = 0;

    virtual Position GetPosition() const = 0;

    virtual Size GetSize() const = 0;

    virtual bool IsKeyPressed(const KeyCode key) const = 0;

    virtual bool IsMouseButtonPressed(const ButtonType btn) const = 0;

    virtual bool IsMouseLocked() const = 0;

    virtual bool IsMouseCursorVisible() const = 0;

    virtual Position GetMousePosition() const = 0;

    virtual bool HasFocus() const = 0;

    virtual void SetTitle(const char* title) = 0;

    virtual void SetPosition(const Position& position) = 0;

    virtual void SetSize(const Size& size) = 0;

    virtual void ShowKeyboard(bool enabled) = 0;

    virtual void SetMouseLocked(bool locked) = 0;

    virtual void SetMouseCursorVisible(bool visible) = 0;

    virtual void Close() = 0;

    virtual Event GetEvent(bool waitForEvent = false) = 0;
     
    virtual bool ProcessEvents(bool waitForEvent = false) = 0;

public:
    virtual ~IWindow() = default;
};
} // namespace PreVEngine

#endif