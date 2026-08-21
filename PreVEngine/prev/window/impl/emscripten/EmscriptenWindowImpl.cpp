#include "EmscriptenWindowImpl.h"

#ifdef __EMSCRIPTEN__

#include <emscripten.h>
#include <emscripten/html5.h>

#include <cmath>

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

    EmscriptenFullscreenChangeEvent fullscreenStatus{};
    if (emscripten_get_fullscreen_status(&fullscreenStatus) == EMSCRIPTEN_RESULT_SUCCESS) {
        m_info.fullScreen = fullscreenStatus.isFullscreen;
    }
    ApplyDisplaySize();

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

    ObserveDevicePixelBox();

    // Push initial events
    m_eventQueue.Push(OnInitEvent());
    m_eventQueue.Push(OnResizeEvent(m_info.size.width, m_info.size.height));
    m_eventQueue.Push(OnFocusEvent(true));
}

EmscriptenWindowImpl::~EmscriptenWindowImpl()
{
    const char* canvas{ m_canvasSelector.c_str() };
    emscripten_set_mousemove_callback(canvas, nullptr, true, nullptr);
    emscripten_set_mousedown_callback(canvas, nullptr, true, nullptr);
    emscripten_set_mouseup_callback(canvas, nullptr, true, nullptr);
    emscripten_set_keydown_callback(EMSCRIPTEN_EVENT_TARGET_DOCUMENT, nullptr, true, nullptr);
    emscripten_set_keyup_callback(EMSCRIPTEN_EVENT_TARGET_DOCUMENT, nullptr, true, nullptr);
    emscripten_set_resize_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, nullptr, true, nullptr);
    emscripten_set_blur_callback(canvas, nullptr, true, nullptr);
    emscripten_set_focus_callback(canvas, nullptr, true, nullptr);
    emscripten_set_fullscreenchange_callback(EMSCRIPTEN_EVENT_TARGET_DOCUMENT, nullptr, true, nullptr);
    emscripten_set_touchstart_callback(canvas, nullptr, true, nullptr);
    emscripten_set_touchmove_callback(canvas, nullptr, true, nullptr);
    emscripten_set_touchend_callback(canvas, nullptr, true, nullptr);
    emscripten_set_touchcancel_callback(canvas, nullptr, true, nullptr);

    // clang-format off
    EM_ASM({
        if (window.__canvasDevicePxObserver) {
            window.__canvasDevicePxObserver.disconnect();
            window.__canvasDevicePxObserver = null;
        }
        window.__canvasDevicePx = null;
    });
    // clang-format on
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
    ApplyDisplaySize(); // the surface always tracks the display; layout only decides where it shows
}

void EmscriptenWindowImpl::ObserveDevicePixelBox()
{
    // clang-format off
    EM_ASM({
        var canvas = document.querySelector(UTF8ToString($0));
        if (!canvas || !window.ResizeObserver) {
            return;
        }
        if (window.__canvasDevicePxObserver) {
            window.__canvasDevicePxObserver.disconnect();
        } else {
            document.addEventListener('fullscreenchange', function() { window.__canvasDevicePx = null; });
        }
        try {
            window.__canvasDevicePxObserver = new ResizeObserver(function(entries) {
                for (var i = 0; i < entries.length; ++i) {
                    var box = entries[i].devicePixelContentBoxSize;
                    if (!box || !box[0]) {
                        continue;
                    }
                    var w = box[0].inlineSize;
                    var h = box[0].blockSize;
                    var prev = window.__canvasDevicePx;
                    if (prev && prev.w === w && prev.h === h) {
                        continue;
                    }
                    window.__canvasDevicePx = { w: w, h: h };
                    if (document.fullscreenElement) {
                        window.dispatchEvent(new Event('resize')); // windowed this would feed back
                    }
                }
            });
            window.__canvasDevicePxObserver.observe(canvas, { box: 'device-pixel-content-box' });
        } catch (e) {
        }
    }, m_canvasSelector.c_str());
    // clang-format on
}

// Windowed, the render surface is the display's physical resolution (screen CSS size x devicePixelRatio):
// page layout cannot inflate or shrink the render target, and a canvas still hidden at boot sizes
// correctly. The element shows the surface wherever layout puts it; input maps by surface / element box.
void EmscriptenWindowImpl::ApplyDisplaySize()
{
    const double devicePixelRatio{ emscripten_get_device_pixel_ratio() };
    EmscriptenFullscreenChangeEvent status{};
    if (emscripten_get_fullscreen_status(&status) != EMSCRIPTEN_RESULT_SUCCESS || status.screenWidth <= 0 || status.screenHeight <= 0) {
        return; // keep the current size; a resize event will retry
    }

    double width{ static_cast<double>(status.screenWidth) };
    double height{ static_cast<double>(status.screenHeight) };

    // Fullscreen: measure the element - screen.* overshoots the area the page actually gets (a phone
    // keeps its navigation strip), and the UA letterboxes the resulting aspect mismatch.
    double boxWidth{};
    double boxHeight{};
    if (status.isFullscreen
        && emscripten_get_element_css_size(m_canvasSelector.c_str(), &boxWidth, &boxHeight) == EMSCRIPTEN_RESULT_SUCCESS
        && boxWidth > 0.0 && boxHeight > 0.0) {
        width = boxWidth;
        height = boxHeight;
    }

    uint32_t surfaceWidth{ static_cast<uint32_t>(std::lround(width * devicePixelRatio)) };
    uint32_t surfaceHeight{ static_cast<uint32_t>(std::lround(height * devicePixelRatio)) };

    if (status.isFullscreen) {
        const int exactWidth{ EM_ASM_INT({ return window.__canvasDevicePx ? window.__canvasDevicePx.w : 0; }) };
        const int exactHeight{ EM_ASM_INT({ return window.__canvasDevicePx ? window.__canvasDevicePx.h : 0; }) };
        if (exactWidth > 0 && exactHeight > 0) {
            surfaceWidth = static_cast<uint32_t>(exactWidth);
            surfaceHeight = static_cast<uint32_t>(exactHeight);
        }
    }

    m_info.size = { surfaceWidth, surfaceHeight };
    emscripten_set_canvas_element_size(m_canvasSelector.c_str(), m_info.size.width, m_info.size.height);
}

void EmscriptenWindowImpl::GetInputScale(double& outX, double& outY) const
{
    outX = outY = 1.0;
    double boxWidth, boxHeight;
    if (emscripten_get_element_css_size(m_canvasSelector.c_str(), &boxWidth, &boxHeight) == EMSCRIPTEN_RESULT_SUCCESS && boxWidth > 0.0 && boxHeight > 0.0) {
        outX = static_cast<double>(m_info.size.width) / boxWidth;
        outY = static_cast<double>(m_info.size.height) / boxHeight;
    }
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

    double scaleX, scaleY;
    self->GetInputScale(scaleX, scaleY);
    int32_t x, y;
    if (self->m_hasFocus && self->m_mouseLocked) {
        x = static_cast<int32_t>(std::lround(mouseEvent->movementX * scaleX));
        y = static_cast<int32_t>(std::lround(mouseEvent->movementY * scaleY));
    } else {
        x = static_cast<int32_t>(std::lround(mouseEvent->targetX * scaleX));
        y = static_cast<int32_t>(std::lround(mouseEvent->targetY * scaleY));
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

    uint8_t keyCode = DOM_TO_HID[keyEvent->keyCode & 0xFF];
    // Legacy DOM keyCode is one value for both keys of a modifier pair, so the table maps each to its LEFT
    // HID usage; use the event location to pick the right-hand variant (HID right = left + 4).
    if (keyEvent->location == 2 /* DOM_KEY_LOCATION_RIGHT */ && keyCode >= 224 && keyCode <= 227) {
        keyCode += 4;
    }

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
    const auto previous{ self->m_info.size };
    self->ApplyDisplaySize(); // rotation swaps the display dimensions
    if (self->m_info.size.width != previous.width || self->m_info.size.height != previous.height) {
        self->m_eventQueue.Push(self->OnResizeEvent(self->m_info.size.width, self->m_info.size.height));
    }
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
    const auto previous{ self->m_info.size };
    self->ApplyDisplaySize(); // same display either way; fullscreen only changes where the element shows
    self->m_eventQueue.Push(self->OnFocusEvent(true));
    if (self->m_info.size.width != previous.width || self->m_info.size.height != previous.height) {
        self->m_eventQueue.Push(self->OnResizeEvent(self->m_info.size.width, self->m_info.size.height));
    }
    return EM_TRUE;
}

EM_BOOL EmscriptenWindowImpl::TouchCallback(int eventType, const EmscriptenTouchEvent* touchEvent, void* userData)
{
    auto* self = static_cast<EmscriptenWindowImpl*>(userData);

    const float w = static_cast<float>(self->m_info.size.width);
    const float h = static_cast<float>(self->m_info.size.height);
    double scaleX, scaleY;
    self->GetInputScale(scaleX, scaleY);

    switch (eventType) {
    case EMSCRIPTEN_EVENT_TOUCHSTART:
        for (int i = 0; i < touchEvent->numTouches; ++i) {
            const auto& touch = touchEvent->touches[i];
            if (touch.isChanged) {
                self->m_eventQueue.Push(self->m_MTouch.OnEvent(ActionType::DOWN, static_cast<float>(touch.targetX * scaleX), static_cast<float>(touch.targetY * scaleY), static_cast<uint32_t>(touch.identifier), w, h));
            }
        }
        break;
    case EMSCRIPTEN_EVENT_TOUCHMOVE:
        for (int i = 0; i < touchEvent->numTouches; ++i) {
            const auto& touch = touchEvent->touches[i];
            if (touch.isChanged) {
                self->m_eventQueue.Push(self->m_MTouch.OnEvent(ActionType::MOVE, static_cast<float>(touch.targetX * scaleX), static_cast<float>(touch.targetY * scaleY), static_cast<uint32_t>(touch.identifier), w, h));
            }
        }
        break;
    case EMSCRIPTEN_EVENT_TOUCHEND:
        for (int i = 0; i < touchEvent->numTouches; ++i) {
            const auto& touch = touchEvent->touches[i];
            if (touch.isChanged) {
                self->m_eventQueue.Push(self->m_MTouch.OnEvent(ActionType::UP, static_cast<float>(touch.targetX * scaleX), static_cast<float>(touch.targetY * scaleY), static_cast<uint32_t>(touch.identifier), w, h));
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
