#include "EmscriptenWindowImpl.h"

#ifdef __EMSCRIPTEN__

#include <emscripten.h>
#include <emscripten/html5.h>

namespace prev::window::impl::emscripten {
namespace {
    // Convert DOM KeyboardEvent.keyCode (which uses Windows virtual key codes) to USB HID codes.
    const uint8_t DOM_TO_HID[256] = {
        0, 0, 0, 0, 0, 0, 0, 0, 42, 43, 0, 0, 0, 40, 0, 0, // 16
        225, 224, 226, 72, 57, 0, 0, 0, 0, 0, 0, 41, 0, 0, 0, 0, // 32
        44, 75, 78, 77, 74, 80, 82, 79, 81, 0, 0, 0, 70, 73, 76, 0, // 48
        39, 30, 31, 32, 33, 34, 35, 36, 37, 38, 0, 0, 0, 0, 0, 0, // 64
        0, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, // 80
        19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 0, 0, 0, 0, 0, // 96
        98, 89, 90, 91, 92, 93, 94, 95, 96, 97, 85, 87, 0, 86, 99, 84, // 112
        58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 104, 105, 106, 107, // 128
        108, 109, 110, 111, 112, 113, 114, 115, 0, 0, 0, 0, 0, 0, 0, 0, // 144
        83, 71, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 160
        225, 229, 224, 228, 226, 230, 0, 0, 0, 0, 0, 0, 0, 127, 128, 129, // 176
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 51, 46, 54, 45, 55, 56, // 192
        53, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 208
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 47, 49, 48, 52, 0, // 224
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 240
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 // 256
    };

    ButtonType ToButtonType(unsigned short button)
    {
        switch (button) {
        case 0:
            return ButtonType::LEFT;
        case 1:
            return ButtonType::MIDDLE;
        case 2:
            return ButtonType::RIGHT;
        default:
            return ButtonType::NONE;
        }
    }
} // namespace
EmscriptenWindowImpl::EmscriptenWindowImpl(const WindowInfo& windowInfo)
    : WindowImpl()
{
    m_info = windowInfo;
    m_running = true;

    // Set canvas size
    emscripten_set_canvas_element_size(m_canvasSelector.c_str(), m_info.size.width, m_info.size.height);

    SetTitle(windowInfo.title);

    // Register HTML5 event callbacks
    emscripten_set_mousemove_callback(m_canvasSelector.c_str(), this, true, MouseCallback);
    emscripten_set_mousedown_callback(m_canvasSelector.c_str(), this, true, MouseCallback);
    emscripten_set_mouseup_callback(m_canvasSelector.c_str(), this, true, MouseCallback);
    emscripten_set_keydown_callback(EMSCRIPTEN_EVENT_TARGET_DOCUMENT, this, true, KeyCallback);
    emscripten_set_keyup_callback(EMSCRIPTEN_EVENT_TARGET_DOCUMENT, this, true, KeyCallback);
    emscripten_set_resize_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, this, true, ResizeCallback);
    emscripten_set_blur_callback(m_canvasSelector.c_str(), this, true, FocusCallback);
    emscripten_set_focus_callback(m_canvasSelector.c_str(), this, true, FocusCallback);
    emscripten_set_fullscreenchange_callback(EMSCRIPTEN_EVENT_TARGET_DOCUMENT, this, true, FullscreenChangeCallback);
    emscripten_set_touchstart_callback(m_canvasSelector.c_str(), this, true, TouchCallback);
    emscripten_set_touchmove_callback(m_canvasSelector.c_str(), this, true, TouchCallback);
    emscripten_set_touchend_callback(m_canvasSelector.c_str(), this, true, TouchCallback);
    emscripten_set_touchcancel_callback(m_canvasSelector.c_str(), this, true, TouchCallback);

    // Push initial events
    m_eventQueue.Push(OnInitEvent());
    m_eventQueue.Push(OnResizeEvent(m_info.size.width, m_info.size.height));
    m_eventQueue.Push(OnFocusEvent(true));
}

EmscriptenWindowImpl::~EmscriptenWindowImpl()
{
}

bool EmscriptenWindowImpl::PollEvent(bool waitForEvent, Event& outEvent)
{
    if (!m_eventQueue.IsEmpty()) {
        outEvent = m_eventQueue.Pop();
        return true;
    }
    return false;
}

void EmscriptenWindowImpl::SetTitle(const std::string& title)
{
    m_info.title = title;
    emscripten_set_window_title(title.c_str());
}

void EmscriptenWindowImpl::SetPosition(int32_t x, int32_t y)
{
    m_info.position = { x, y };
}

void EmscriptenWindowImpl::SetSize(uint32_t w, uint32_t h)
{
    m_info.size = { w, h };
    emscripten_set_canvas_element_size(m_canvasSelector.c_str(), w, h);
}

void EmscriptenWindowImpl::SetMouseCursorVisible(bool visible)
{
    m_mouseCursorVisible = visible;
    if (!visible) {
        emscripten_request_pointerlock(m_canvasSelector.c_str(), true);
    } else {
        emscripten_exit_pointerlock();
    }
}

GfxPlatformWindowHandle EmscriptenWindowImpl::GetNativeWindowHandle() const
{
    return gfxPlatformWindowHandleFromEmscripten(m_canvasSelector.c_str());
}

EM_BOOL EmscriptenWindowImpl::MouseCallback(int eventType, const EmscriptenMouseEvent* mouseEvent, void* userData)
{
    auto* self = static_cast<EmscriptenWindowImpl*>(userData);

    const ButtonType btn = ToButtonType(mouseEvent->button);

    int32_t x, y;
    if (self->m_hasFocus && self->m_mouseLocked) {
        x = static_cast<int32_t>(mouseEvent->movementX);
        y = static_cast<int32_t>(mouseEvent->movementY);
    } else {
        x = static_cast<int32_t>(mouseEvent->targetX);
        y = static_cast<int32_t>(mouseEvent->targetY);
    }

    switch (eventType) {
    case EMSCRIPTEN_EVENT_MOUSEMOVE:
        self->m_eventQueue.Push(self->OnMouseEvent(ActionType::MOVE, x, y, ButtonType::NONE));
        break;
    case EMSCRIPTEN_EVENT_MOUSEDOWN:
        self->m_eventQueue.Push(self->OnMouseEvent(ActionType::DOWN, x, y, btn));
        break;
    case EMSCRIPTEN_EVENT_MOUSEUP:
        self->m_eventQueue.Push(self->OnMouseEvent(ActionType::UP, x, y, btn));
        break;
    }
    return EM_TRUE;
}

EM_BOOL EmscriptenWindowImpl::KeyCallback(int eventType, const EmscriptenKeyboardEvent* keyEvent, void* userData)
{
    auto* self = static_cast<EmscriptenWindowImpl*>(userData);

    const uint8_t keyCode = DOM_TO_HID[keyEvent->keyCode & 0xFF];

    switch (eventType) {
    case EMSCRIPTEN_EVENT_KEYDOWN:
        self->m_eventQueue.Push(self->OnKeyEvent(ActionType::DOWN, keyCode));
        break;
    case EMSCRIPTEN_EVENT_KEYUP:
        self->m_eventQueue.Push(self->OnKeyEvent(ActionType::UP, keyCode));
        break;
    }
    return EM_TRUE;
}

EM_BOOL EmscriptenWindowImpl::ResizeCallback(int eventType, const EmscriptenUiEvent* uiEvent, void* userData)
{
    auto* self = static_cast<EmscriptenWindowImpl*>(userData);
    self->m_info.size = { static_cast<uint32_t>(uiEvent->windowInnerWidth), static_cast<uint32_t>(uiEvent->windowInnerHeight) };
    self->m_eventQueue.Push(self->OnResizeEvent(self->m_info.size.width, self->m_info.size.height));
    return EM_TRUE;
}

EM_BOOL EmscriptenWindowImpl::FocusCallback(int eventType, const EmscriptenFocusEvent* focusEvent, void* userData)
{
    auto* self = static_cast<EmscriptenWindowImpl*>(userData);
    const bool hasFocus = (eventType == EMSCRIPTEN_EVENT_FOCUS);
    self->m_eventQueue.Push(self->OnFocusEvent(hasFocus));
    return EM_TRUE;
}

EM_BOOL EmscriptenWindowImpl::FullscreenChangeCallback(int eventType, const EmscriptenFullscreenChangeEvent* fullscreenEvent, void* userData)
{
    auto* self = static_cast<EmscriptenWindowImpl*>(userData);
    self->m_info.fullScreen = fullscreenEvent->isFullscreen;
    if (fullscreenEvent->isFullscreen) {
        self->m_info.size = { static_cast<uint32_t>(fullscreenEvent->screenWidth), static_cast<uint32_t>(fullscreenEvent->screenHeight) };
    } else {
        int w, h;
        emscripten_get_canvas_element_size(self->m_canvasSelector.c_str(), &w, &h);
        self->m_info.size = { static_cast<uint32_t>(w), static_cast<uint32_t>(h) };
    }
    self->m_eventQueue.Push(self->OnFocusEvent(true));
    self->m_eventQueue.Push(self->OnResizeEvent(self->m_info.size.width, self->m_info.size.height));
    return EM_TRUE;
}

EM_BOOL EmscriptenWindowImpl::TouchCallback(int eventType, const EmscriptenTouchEvent* touchEvent, void* userData)
{
    auto* self = static_cast<EmscriptenWindowImpl*>(userData);

    const float w = static_cast<float>(self->m_info.size.width);
    const float h = static_cast<float>(self->m_info.size.height);

    switch (eventType) {
    case EMSCRIPTEN_EVENT_TOUCHSTART:
        for (int i = 0; i < touchEvent->numTouches; ++i) {
            const auto& touch = touchEvent->touches[i];
            if (touch.isChanged) {
                self->m_eventQueue.Push(self->m_MTouch.OnEvent(ActionType::DOWN, static_cast<float>(touch.targetX), static_cast<float>(touch.targetY), static_cast<uint32_t>(touch.identifier), w, h));
            }
        }
        break;
    case EMSCRIPTEN_EVENT_TOUCHMOVE:
        for (int i = 0; i < touchEvent->numTouches; ++i) {
            const auto& touch = touchEvent->touches[i];
            if (touch.isChanged) {
                self->m_eventQueue.Push(self->m_MTouch.OnEvent(ActionType::MOVE, static_cast<float>(touch.targetX), static_cast<float>(touch.targetY), static_cast<uint32_t>(touch.identifier), w, h));
            }
        }
        break;
    case EMSCRIPTEN_EVENT_TOUCHEND:
        for (int i = 0; i < touchEvent->numTouches; ++i) {
            const auto& touch = touchEvent->touches[i];
            if (touch.isChanged) {
                self->m_eventQueue.Push(self->m_MTouch.OnEvent(ActionType::UP, static_cast<float>(touch.targetX), static_cast<float>(touch.targetY), static_cast<uint32_t>(touch.identifier), w, h));
            }
        }
        break;
    case EMSCRIPTEN_EVENT_TOUCHCANCEL:
        self->m_MTouch.Reset();
        break;
    }
    return EM_TRUE;
}
} // namespace prev::window::impl::emscripten

#endif // __EMSCRIPTEN__
