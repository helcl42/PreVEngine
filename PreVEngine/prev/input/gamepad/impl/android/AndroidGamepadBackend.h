#ifndef __ANDROID_GAMEPAD_BACKEND_H__
#define __ANDROID_GAMEPAD_BACKEND_H__

#include "../../IGamepadBackend.h"

#if defined(TARGET_PLATFORM_ANDROID)

#include "AndroidGamepadEvents.h"
#include "AndroidGamepadTracker.h"

#include "../../../../event/EventHandler.h"

#include <jni.h>

#include <chrono>
#include <optional>

namespace prev::input::gamepad::impl::android {
class AndroidGamepadBackend final : public IGamepadBackend {
public:
    AndroidGamepadBackend() = default;

    ~AndroidGamepadBackend() override;

    AndroidGamepadBackend(const AndroidGamepadBackend& other) = delete;

    AndroidGamepadBackend& operator=(const AndroidGamepadBackend& other) = delete;

    AndroidGamepadBackend(AndroidGamepadBackend&& other) = delete;

    AndroidGamepadBackend& operator=(AndroidGamepadBackend&& other) = delete;

public:
    void Poll(GamepadInputs& inputs) override;

public:
    void operator()(const AndroidGamepadKeyEvent& keyEvent);

    void operator()(const AndroidGamepadMotionEvent& motionEvent);

private:
    void ScanIfNewlyTracked(const int32_t deviceId, const bool wasTracked);

    bool ScanDevices(std::vector<AndroidInputDevice>& outDevices);

    bool LoadInputDeviceJni(JNIEnv* env);

    std::optional<AndroidInputDevice> ReadDevice(JNIEnv* env, const jobject device, const jint deviceId) const;

private:
    using Clock = std::chrono::steady_clock;

    struct InputDeviceJni {
        jclass inputDeviceClass{ nullptr }; // global ref

        jmethodID getDeviceIds{ nullptr };

        jmethodID getDevice{ nullptr };

        jmethodID getSources{ nullptr };

        jmethodID getName{ nullptr };

        jmethodID isVirtual{ nullptr };

        jmethodID getMotionRange{ nullptr };
    };

    InputDeviceJni m_jni{};

    AndroidGamepadTracker m_tracker{};

    Clock::time_point m_nextScan{};

    bool m_scanAvailable{ true };

    prev::event::EventHandler<AndroidGamepadBackend, AndroidGamepadKeyEvent> m_keyEventHandler{ *this };

    prev::event::EventHandler<AndroidGamepadBackend, AndroidGamepadMotionEvent> m_motionEventHandler{ *this };
};
} // namespace prev::input::gamepad::impl::android

#endif

#endif // !__ANDROID_GAMEPAD_BACKEND_H__
