#include "GameControllerGamepadBackend.h"

#if defined(TARGET_PLATFORM_MACOS) || defined(TARGET_PLATFORM_IOS)

#import <Foundation/Foundation.h>
#import <GameController/GameController.h>

#include <algorithm>

namespace prev::input::gamepad::impl::apple {
namespace {
    bool IsPressed(GCControllerButtonInput* button)
    {
        return button != nil && button.isPressed;
    }

    void ReadExtendedGamepad(GCExtendedGamepad* pad, GamepadInput& input)
    {
        input.SetButton(GamepadButton::SOUTH, IsPressed(pad.buttonA));
        input.SetButton(GamepadButton::EAST, IsPressed(pad.buttonB));
        input.SetButton(GamepadButton::WEST, IsPressed(pad.buttonX));
        input.SetButton(GamepadButton::NORTH, IsPressed(pad.buttonY));
        input.SetButton(GamepadButton::LEFT_SHOULDER, IsPressed(pad.leftShoulder));
        input.SetButton(GamepadButton::RIGHT_SHOULDER, IsPressed(pad.rightShoulder));
        input.SetButton(GamepadButton::LEFT_STICK, IsPressed(pad.leftThumbstickButton));
        input.SetButton(GamepadButton::RIGHT_STICK, IsPressed(pad.rightThumbstickButton));
        input.SetButton(GamepadButton::START, IsPressed(pad.buttonMenu));
        input.SetButton(GamepadButton::BACK, IsPressed(pad.buttonOptions));
        if (@available(macOS 11.0, iOS 14.0, *)) {
            input.SetButton(GamepadButton::GUIDE, IsPressed(pad.buttonHome));
        }

        input.SetButton(GamepadButton::DPAD_UP, IsPressed(pad.dpad.up));
        input.SetButton(GamepadButton::DPAD_DOWN, IsPressed(pad.dpad.down));
        input.SetButton(GamepadButton::DPAD_LEFT, IsPressed(pad.dpad.left));
        input.SetButton(GamepadButton::DPAD_RIGHT, IsPressed(pad.dpad.right));

        input.SetAxis(GamepadAxis::LEFT_X, pad.leftThumbstick.xAxis.value);
        input.SetAxis(GamepadAxis::LEFT_Y, pad.leftThumbstick.yAxis.value);
        input.SetAxis(GamepadAxis::RIGHT_X, pad.rightThumbstick.xAxis.value);
        input.SetAxis(GamepadAxis::RIGHT_Y, pad.rightThumbstick.yAxis.value);
        input.SetAxis(GamepadAxis::LEFT_TRIGGER, pad.leftTrigger.value);
        input.SetAxis(GamepadAxis::RIGHT_TRIGGER, pad.rightTrigger.value);
    }
} // namespace

GameControllerGamepadBackend::~GameControllerGamepadBackend()
{
    for (auto& slot : m_slots) {
        [static_cast<GCController*>(slot) release];
        slot = nullptr;
    }
}

void GameControllerGamepadBackend::Poll(GamepadInputs& inputs)
{
    @autoreleasepool {
        NSArray<GCController*>* controllers{ [GCController controllers] };

        for (auto& slot : m_slots) {
            if (slot && ![controllers containsObject:static_cast<GCController*>(slot)]) {
                [static_cast<GCController*>(slot) release];
                slot = nullptr;
            }
        }

        for (GCController* controller in controllers) {
            if (controller.extendedGamepad == nil || std::find(m_slots.begin(), m_slots.end(), static_cast<void*>(controller)) != m_slots.end()) {
                continue;
            }
            const auto freeSlot{ std::find(m_slots.begin(), m_slots.end(), nullptr) };
            if (freeSlot == m_slots.end()) {
                break;
            }
            *freeSlot = [controller retain];
        }

        for (uint32_t i = 0; i < MAX_GAMEPAD_COUNT; ++i) {
            inputs[i] = GamepadInput{};
            GCController* controller{ static_cast<GCController*>(m_slots[i]) };
            if (controller == nil || controller.extendedGamepad == nil) {
                continue;
            }
            inputs[i].connected = true;
            NSString* vendorName{ controller.vendorName };
            inputs[i].name = vendorName ? std::string{ [vendorName UTF8String] } : std::string{ "Game controller" };
            ReadExtendedGamepad(controller.extendedGamepad, inputs[i]);
        }
    }
}
} // namespace prev::input::gamepad::impl::apple

#endif
