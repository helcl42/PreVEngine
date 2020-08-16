#ifndef __INPUT_CONVERTOR_H__
#define __INPUT_CONVERTOR_H__

#include "../input/keyboard/KeyboardEvents.h"
#include "../input/mouse/MouseEvents.h"
#include "../input/touch/TouchEvents.h"

#include "WindowImpl.h"

namespace prev::window {
class InputConvertor {
public:
    static prev::input::keyboard::KeyActionType GetKeyActionType(const ActionType action);

    static prev::input::mouse::MouseActionType GetMouseActionType(const ActionType action);

    static prev::input::mouse::MouseButtonType GetMouseButtonType(const ButtonType button);

    static prev::input::touch::TouchActionType GetTouchActionType(const ActionType action);
};
} // namespace prev::window

#endif