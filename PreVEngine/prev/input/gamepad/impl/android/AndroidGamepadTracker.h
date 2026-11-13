#ifndef __ANDROID_GAMEPAD_TRACKER_H__
#define __ANDROID_GAMEPAD_TRACKER_H__

#include "../../IGamepadBackend.h"

#if defined(TARGET_PLATFORM_ANDROID)

#include "AndroidGamepadMapping.h"

#include <string>
#include <vector>

namespace prev::input::gamepad::impl::android {
struct AndroidInputDevice {
    int32_t deviceId{};

    std::string name{};

    bool rightStickOnRxRy{ false };
};

class AndroidGamepadTracker final {
public:
    void OnKey(const int32_t deviceId, const int32_t keyCode, const bool pressed);

    void OnMotion(const int32_t deviceId, const MotionAxisValues& values);

    void SetDeviceList(const std::vector<AndroidInputDevice>& devices);

    void Snapshot(GamepadInputs& inputs) const;

    bool IsTracked(const int32_t deviceId) const;

private:
    struct Slot {
        bool used{ false };

        int32_t deviceId{ -1 };

        bool seenByScan{ false };

        bool hasAnalogTriggers{ false };

        bool hasHat{ false };

        bool rightStickOnRxRy{ false };

        GamepadInput input{};
    };

    Slot* Find(const int32_t deviceId);

    Slot* Acquire(const int32_t deviceId, const std::string& name);

private:
    std::array<Slot, MAX_GAMEPAD_COUNT> m_slots{};
};
} // namespace prev::input::gamepad::impl::android

#endif

#endif // !__ANDROID_GAMEPAD_TRACKER_H__
