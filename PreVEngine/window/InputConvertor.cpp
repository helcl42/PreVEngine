#include "InputConvertor.h"

namespace prev {
KeyActionType InputConvertor::GetKeyActionType(const ActionType action)
{
    switch (action) {
    case ActionType::DOWN:
        return KeyActionType::PRESS;
    case ActionType::UP:
        return KeyActionType::RELEASE;
    default:
        throw std::runtime_error("Invalid key action");
    }
}

MouseActionType InputConvertor::GetMouseActionType(const ActionType action)
{
    switch (action) {
    case ActionType::DOWN:
        return MouseActionType::PRESS;
    case ActionType::UP:
        return MouseActionType::RELEASE;
    case ActionType::MOVE:
        return MouseActionType::MOVE;
    default:
        throw std::runtime_error("Invalid mouse button action");
    }
}

MouseButtonType InputConvertor::GetMouseButtonType(const ButtonType button)
{
    switch (button) {
    case ButtonType::NONE:
        return MouseButtonType::NONE;
    case ButtonType::LEFT:
        return MouseButtonType::LEFT;
    case ButtonType::MIDDLE:
        return MouseButtonType::MIDDLE;
    case ButtonType::RIGHT:
        return MouseButtonType::RIGHT;
    default:
        throw std::runtime_error("Invalid mouse button type");
    }
}

TouchActionType InputConvertor::GetTouchActionType(const ActionType action)
{
    switch (action) {
    case ActionType::DOWN:
        return TouchActionType::DOWN;
    case ActionType::UP:
        return TouchActionType::UP;
    case ActionType::MOVE:
        return TouchActionType::MOVE;
    default:
        throw std::runtime_error("Invalid touch action");
    }
}
} // namespace prev