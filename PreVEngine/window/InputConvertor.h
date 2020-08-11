#ifndef __INPUT_CONVERTOR_H__
#define __INPUT_CONVERTOR_H__

#include "../input/keyboard/KeyboardEvents.h"
#include "../input/mouse/MouseEvents.h"
#include "../input/touch/TouchEvents.h"

#include "WindowImpl.h"

namespace PreVEngine {
class InputConvertor {
public:
    static KeyActionType GetKeyActionType(const ActionType action);

    static MouseActionType GetMouseActionType(const ActionType action);

    static MouseButtonType GetMouseButtonType(const ButtonType button);

    static TouchActionType GetTouchActionType(const ActionType action);
};
} // namespace PreVEngine

#endif