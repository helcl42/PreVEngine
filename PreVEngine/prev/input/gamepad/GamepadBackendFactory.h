#ifndef __GAMEPAD_BACKEND_FACTORY_H__
#define __GAMEPAD_BACKEND_FACTORY_H__

#include "IGamepadBackend.h"

#include <memory>

namespace prev::input::gamepad {
class GamepadBackendFactory final {
public:
    std::unique_ptr<IGamepadBackend> Create() const;
};
} // namespace prev::input::gamepad

#endif // !__GAMEPAD_BACKEND_FACTORY_H__
