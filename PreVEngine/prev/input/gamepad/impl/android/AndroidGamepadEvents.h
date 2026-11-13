#ifndef __ANDROID_GAMEPAD_EVENTS_H__
#define __ANDROID_GAMEPAD_EVENTS_H__

#include "AndroidGamepadMapping.h"

namespace prev::input::gamepad::impl::android {
struct AndroidGamepadKeyEvent {
    int32_t deviceId{};

    int32_t keyCode{};

    bool pressed{};
};

struct AndroidGamepadMotionEvent {
    int32_t deviceId{};

    MotionAxisValues values{}; // sampled in MOTION_AXIS_IDS order
};
} // namespace prev::input::gamepad::impl::android

#endif // !__ANDROID_GAMEPAD_EVENTS_H__
