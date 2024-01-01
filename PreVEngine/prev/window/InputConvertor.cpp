#include "InputConvertor.h"

#include <stdexcept>

namespace prev::window {
prev::input::keyboard::KeyActionType InputConvertor::GetKeyActionType(const impl::ActionType action)
{
    switch (action) {
    case impl::ActionType::DOWN:
        return prev::input::keyboard::KeyActionType::PRESS;
    case impl::ActionType::UP:
        return prev::input::keyboard::KeyActionType::RELEASE;
    default:
        throw std::runtime_error("Invalid key action");
    }
}

prev::input::mouse::MouseActionType InputConvertor::GetMouseActionType(const impl::ActionType action)
{
    switch (action) {
    case impl::ActionType::DOWN:
        return prev::input::mouse::MouseActionType::PRESS;
    case impl::ActionType::UP:
        return prev::input::mouse::MouseActionType::RELEASE;
    case impl::ActionType::MOVE:
        return prev::input::mouse::MouseActionType::MOVE;
    default:
        throw std::runtime_error("Invalid mouse button action");
    }
}

prev::input::mouse::MouseButtonType InputConvertor::GetMouseButtonType(const impl::ButtonType button)
{
    switch (button) {
    case impl::ButtonType::NONE:
        return prev::input::mouse::MouseButtonType::NONE;
    case impl::ButtonType::LEFT:
        return prev::input::mouse::MouseButtonType::LEFT;
    case impl::ButtonType::MIDDLE:
        return prev::input::mouse::MouseButtonType::MIDDLE;
    case impl::ButtonType::RIGHT:
        return prev::input::mouse::MouseButtonType::RIGHT;
    default:
        throw std::runtime_error("Invalid mouse button type");
    }
}

prev::input::touch::TouchActionType InputConvertor::GetTouchActionType(const impl::ActionType action)
{
    switch (action) {
    case impl::ActionType::DOWN:
        return prev::input::touch::TouchActionType::DOWN;
    case impl::ActionType::UP:
        return prev::input::touch::TouchActionType::UP;
    case impl::ActionType::MOVE:
        return prev::input::touch::TouchActionType::MOVE;
    default:
        throw std::runtime_error("Invalid touch action");
    }
}
} // namespace prev::window