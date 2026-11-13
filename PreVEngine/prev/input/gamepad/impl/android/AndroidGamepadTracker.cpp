#include "AndroidGamepadTracker.h"

#if defined(TARGET_PLATFORM_ANDROID)

#include <algorithm>

namespace prev::input::gamepad::impl::android {
namespace {
    constexpr const char* DEFAULT_NAME{ "Gamepad" };

    float Value(const MotionAxisValues& values, const MotionAxis axis)
    {
        return values[static_cast<size_t>(axis)];
    }
} // namespace

void AndroidGamepadTracker::OnKey(const int32_t deviceId, const int32_t keyCode, const bool pressed)
{
    Slot* slot{ Acquire(deviceId, DEFAULT_NAME) };
    if (!slot) {
        return;
    }
    if (const auto button{ MapKey(keyCode) }) {
        slot->input.SetButton(*button, pressed);
    } else if (const auto trigger{ MapDigitalTrigger(keyCode) }) {
        if (!slot->hasAnalogTriggers) {
            slot->input.SetAxis(*trigger, pressed ? 1.0f : 0.0f);
        }
    }
}

void AndroidGamepadTracker::OnMotion(const int32_t deviceId, const MotionAxisValues& values)
{
    Slot* slot{ Acquire(deviceId, DEFAULT_NAME) };
    if (!slot) {
        return;
    }
    GamepadInput& input{ slot->input };

    input.SetAxis(GamepadAxis::LEFT_X, std::clamp(Value(values, MotionAxis::X), -1.0f, 1.0f));
    input.SetAxis(GamepadAxis::LEFT_Y, std::clamp(-Value(values, MotionAxis::Y), -1.0f, 1.0f));
    const MotionAxis rightX{ slot->rightStickOnRxRy ? MotionAxis::RX : MotionAxis::Z };
    const MotionAxis rightY{ slot->rightStickOnRxRy ? MotionAxis::RY : MotionAxis::RZ };
    input.SetAxis(GamepadAxis::RIGHT_X, std::clamp(Value(values, rightX), -1.0f, 1.0f));
    input.SetAxis(GamepadAxis::RIGHT_Y, std::clamp(-Value(values, rightY), -1.0f, 1.0f));

    const float left{ std::max(Value(values, MotionAxis::LTRIGGER), Value(values, MotionAxis::BRAKE)) };
    const float right{ std::max(Value(values, MotionAxis::RTRIGGER), Value(values, MotionAxis::GAS)) };
    if (left > 0.0f || right > 0.0f) {
        slot->hasAnalogTriggers = true;
    }
    if (slot->hasAnalogTriggers) {
        input.SetAxis(GamepadAxis::LEFT_TRIGGER, std::clamp(left, 0.0f, 1.0f));
        input.SetAxis(GamepadAxis::RIGHT_TRIGGER, std::clamp(right, 0.0f, 1.0f));
    }

    // Pads sending d-pad keys never move the hat: stick motion must not clear their d-pad.
    const float hatX{ Value(values, MotionAxis::HAT_X) };
    const float hatY{ Value(values, MotionAxis::HAT_Y) };
    if (hatX != 0.0f || hatY != 0.0f) {
        slot->hasHat = true;
    }
    if (slot->hasHat) {
        input.SetButton(GamepadButton::DPAD_LEFT, hatX < -0.5f);
        input.SetButton(GamepadButton::DPAD_RIGHT, hatX > 0.5f);
        input.SetButton(GamepadButton::DPAD_UP, hatY < -0.5f);
        input.SetButton(GamepadButton::DPAD_DOWN, hatY > 0.5f);
    }
}

void AndroidGamepadTracker::SetDeviceList(const std::vector<AndroidInputDevice>& devices)
{
    const auto isListed = [&devices](const int32_t deviceId) {
        return std::any_of(devices.begin(), devices.end(), [deviceId](const AndroidInputDevice& device) { return device.deviceId == deviceId; });
    };

    for (auto& slot : m_slots) {
        if (slot.used && slot.seenByScan && !isListed(slot.deviceId)) {
            slot = Slot{};
        }
    }

    for (const auto& device : devices) {
        Slot* slot{ Acquire(device.deviceId, device.name) };
        if (slot) {
            slot->seenByScan = true;
            slot->rightStickOnRxRy = device.rightStickOnRxRy;
            if (!device.name.empty()) {
                slot->input.name = device.name;
            }
        }
    }
}

void AndroidGamepadTracker::Snapshot(GamepadInputs& inputs) const
{
    for (uint32_t i = 0; i < MAX_GAMEPAD_COUNT; ++i) {
        inputs[i] = m_slots[i].used ? m_slots[i].input : GamepadInput{};
    }
}

bool AndroidGamepadTracker::IsTracked(const int32_t deviceId) const
{
    return std::any_of(m_slots.begin(), m_slots.end(), [deviceId](const Slot& slot) { return slot.used && slot.deviceId == deviceId; });
}

AndroidGamepadTracker::Slot* AndroidGamepadTracker::Find(const int32_t deviceId)
{
    for (auto& slot : m_slots) {
        if (slot.used && slot.deviceId == deviceId) {
            return &slot;
        }
    }
    return nullptr;
}

AndroidGamepadTracker::Slot* AndroidGamepadTracker::Acquire(const int32_t deviceId, const std::string& name)
{
    Slot* existing{ Find(deviceId) };
    if (existing) {
        return existing;
    }
    for (auto& slot : m_slots) {
        if (!slot.used) {
            slot = Slot{};
            slot.used = true;
            slot.deviceId = deviceId;
            slot.input.connected = true;
            slot.input.name = name.empty() ? DEFAULT_NAME : name;
            return &slot;
        }
    }
    return nullptr;
}
} // namespace prev::input::gamepad::impl::android

#endif
