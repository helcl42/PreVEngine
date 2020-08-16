#include "InputConvertor.h"

namespace prev {
prev::input::keyboard::KeyActionType InputConvertor::GetKeyActionType(const ActionType action)
{
    switch (action) {
    case ActionType::DOWN:
        return prev::input::keyboard::KeyActionType::PRESS;
    case ActionType::UP:
        return prev::input::keyboard::KeyActionType::RELEASE;
    default:
        throw std::runtime_error("Invalid key action");
    }
}

prev::input::mouse::MouseActionType InputConvertor::GetMouseActionType(const ActionType action)
{
    switch (action) {
    case ActionType::DOWN:
        return prev::input::mouse::MouseActionType::PRESS;
    case ActionType::UP:
        return prev::input::mouse::MouseActionType::RELEASE;
    case ActionType::MOVE:
        return prev::input::mouse::MouseActionType::MOVE;
    default:
        throw std::runtime_error("Invalid mouse button action");
    }
}

prev::input::mouse::MouseButtonType InputConvertor::GetMouseButtonType(const ButtonType button)
{
    switch (button) {
    case ButtonType::NONE:
        return prev::input::mouse::MouseButtonType::NONE;
    case ButtonType::LEFT:
        return prev::input::mouse::MouseButtonType::LEFT;
    case ButtonType::MIDDLE:
        return prev::input::mouse::MouseButtonType::MIDDLE;
    case ButtonType::RIGHT:
        return prev::input::mouse::MouseButtonType::RIGHT;
    default:
        throw std::runtime_error("Invalid mouse button type");
    }
}

prev::input::touch::TouchActionType InputConvertor::GetTouchActionType(const ActionType action)
{
    switch (action) {
    case ActionType::DOWN:
        return prev::input::touch::TouchActionType::DOWN;
    case ActionType::UP:
        return prev::input::touch::TouchActionType::UP;
    case ActionType::MOVE:
        return prev::input::touch::TouchActionType::MOVE;
    default:
        throw std::runtime_error("Invalid touch action");
    }
}
} // namespace prev