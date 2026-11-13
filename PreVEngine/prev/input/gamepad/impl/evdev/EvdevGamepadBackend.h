#ifndef __EVDEV_GAMEPAD_BACKEND_H__
#define __EVDEV_GAMEPAD_BACKEND_H__

#include "../../IGamepadBackend.h"

#if defined(TARGET_PLATFORM_LINUX)

#include <linux/input.h>

#include <array>
#include <string>

namespace prev::input::gamepad::impl::evdev {
class EvdevGamepadBackend final : public IGamepadBackend {
public:
    EvdevGamepadBackend();

    ~EvdevGamepadBackend() override;

    EvdevGamepadBackend(const EvdevGamepadBackend& other) = delete;

    EvdevGamepadBackend& operator=(const EvdevGamepadBackend& other) = delete;

    EvdevGamepadBackend(EvdevGamepadBackend&& other) = delete;

    EvdevGamepadBackend& operator=(EvdevGamepadBackend&& other) = delete;

public:
    void Poll(GamepadInputs& inputs) override;

private:
    struct AxisRange {
        int32_t min{ 0 };

        int32_t max{ 0 };
    };

    struct Device {
        int fd{ -1 };

        std::string path{};

        bool positionalFaceButtons{ false };

        bool rightStickOnZRz{ false };

        std::array<AxisRange, ABS_CNT> absRanges{};

        GamepadInput input{};
    };

private:
    void ScanDirectory();

    void ProcessHotplug();

    void TryOpen(const std::string& path);

    void Close(Device& device);

    bool IsOpen(const std::string& path) const;

    bool ReadEvents(Device& device);

    static void SyncFromDevice(Device& device);

    static bool HasAbs(const Device& device, const uint16_t code);

    static void ApplyKey(Device& device, const uint16_t code, const bool pressed);

    static void ApplyAbs(Device& device, const uint16_t code, const int32_t value);

private:
    int m_inotifyFd{ -1 };

    bool m_rescanRequested{ false };

    std::array<Device, MAX_GAMEPAD_COUNT> m_devices{};
};
} // namespace prev::input::gamepad::impl::evdev

#endif

#endif // !__EVDEV_GAMEPAD_BACKEND_H__
