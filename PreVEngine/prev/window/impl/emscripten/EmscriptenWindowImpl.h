#ifdef __EMSCRIPTEN__

#ifndef __EMSCRIPTEN_WINDOW_IMPL_H__
#define __EMSCRIPTEN_WINDOW_IMPL_H__

#include "../WindowImpl.h"

#include <emscripten.h>
#include <emscripten/html5.h>

namespace prev::window::impl::emscripten {
class EmscriptenWindowImpl final : public WindowImpl {
public:
    EmscriptenWindowImpl(const WindowInfo& windowInfo);

    ~EmscriptenWindowImpl();

public:
    bool PollEvent(bool waitForEvent, Event& outEvent) override;

    void SetTitle(const std::string& title) override;

    void SetPosition(int32_t x, int32_t y) override;

    void SetSize(uint32_t w, uint32_t h) override;

    void SetMouseCursorVisible(bool visible) override;

    GfxPlatformWindowHandle GetNativeWindowHandle() const override;

private:
    static EM_BOOL MouseCallback(int eventType, const EmscriptenMouseEvent* mouseEvent, void* userData);

    static EM_BOOL KeyCallback(int eventType, const EmscriptenKeyboardEvent* keyEvent, void* userData);

    static EM_BOOL ResizeCallback(int eventType, const EmscriptenUiEvent* uiEvent, void* userData);

    static EM_BOOL FocusCallback(int eventType, const EmscriptenFocusEvent* focusEvent, void* userData);

    static EM_BOOL FullscreenChangeCallback(int eventType, const EmscriptenFullscreenChangeEvent* fullscreenEvent, void* userData);

    static EM_BOOL TouchCallback(int eventType, const EmscriptenTouchEvent* touchEvent, void* userData);

private:
    std::string m_canvasSelector{ "#canvas" };

    MultiTouch m_MTouch{};
};
} // namespace prev::window::impl::emscripten

#endif // !__EMSCRIPTEN_WINDOW_IMPL_H__

#endif // __EMSCRIPTEN__
