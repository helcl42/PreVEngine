#ifndef __INPUT_CONVERTOR_H__
#define __INPUT_CONVERTOR_H__

#include "../input/keyboard/KeyboardEvents.h"
#include "../input/mouse/MouseEvents.h"
#include "../input/touch/TouchEvents.h"

#include "impl/WindowImpl.h"

namespace prev::window {
class InputConvertor {
public:
    static prev::input::keyboard::KeyActionType GetKeyActionType(const impl::ActionType action);

    static prev::input::mouse::MouseActionType GetMouseActionType(const impl::ActionType action);

    static prev::input::mouse::MouseButtonType GetMouseButtonType(const impl::ButtonType button);

    static prev::input::touch::TouchActionType GetTouchActionType(const impl::ActionType action);
};
} // namespace prev::window

#endif