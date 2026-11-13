#include "EvdevGamepadBackend.h"

#if defined(TARGET_PLATFORM_LINUX)

#include "EvdevMapping.h"

#include "../../GamepadUtils.h"

#include "../../../../common/Logger.h"

#include <dirent.h>
#include <fcntl.h>
#include <linux/input.h>
#include <sys/inotify.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <cerrno>
#include <climits>
#include <cstdlib>
#include <cstring>
#include <vector>

namespace prev::input::gamepad::impl::evdev {
namespace {
    constexpr const char* INPUT_DIRECTORY{ "/dev/input" };

    constexpr size_t BITS_PER_LONG{ sizeof(unsigned long) * CHAR_BIT };

    constexpr size_t BitArraySize(const size_t bitCount)
    {
        return (bitCount + BITS_PER_LONG - 1) / BITS_PER_LONG;
    }

    bool TestBit(const unsigned long* bits, const size_t bit)
    {
        return (bits[bit / BITS_PER_LONG] >> (bit % BITS_PER_LONG)) & 1UL;
    }

    bool IsEventNode(const char* name)
    {
        return std::strncmp(name, "event", 5) == 0;
    }

    // Steam lists the physical pads its virtual ones stand in for (an SDL convention).
    bool IsIgnoredBySteam(const input_id& id)
    {
        static const std::vector<UsbId> ignored{ ParseUsbIdList(std::getenv("SDL_GAMECONTROLLER_IGNORE_DEVICES")) };
        static const std::vector<UsbId> ignoredExcept{ ParseUsbIdList(std::getenv("SDL_GAMECONTROLLER_IGNORE_DEVICES_EXCEPT")) };
        return IsIgnored(ignored, ignoredExcept, UsbId{ id.vendor, id.product });
    }
} // namespace

EvdevGamepadBackend::EvdevGamepadBackend()
{
    m_inotifyFd = inotify_init1(IN_NONBLOCK | IN_CLOEXEC);
    if (m_inotifyFd >= 0) {
        // IN_ATTRIB: udev creates the node first and grants access a moment later.
        if (inotify_add_watch(m_inotifyFd, INPUT_DIRECTORY, IN_CREATE | IN_ATTRIB | IN_DELETE | IN_MOVED_TO) < 0) {
            LOGW("Gamepad: cannot watch %s (%s) - hotplug disabled", INPUT_DIRECTORY, std::strerror(errno));
            close(m_inotifyFd);
            m_inotifyFd = -1;
        }
    } else {
        LOGW("Gamepad: inotify unavailable (%s) - hotplug disabled", std::strerror(errno));
    }

    ScanDirectory();
}

EvdevGamepadBackend::~EvdevGamepadBackend()
{
    for (auto& device : m_devices) {
        Close(device);
    }
    if (m_inotifyFd >= 0) {
        close(m_inotifyFd);
    }
}

void EvdevGamepadBackend::Poll(GamepadInputs& inputs)
{
    ProcessHotplug();

    for (auto& device : m_devices) {
        if (device.fd >= 0 && !ReadEvents(device)) {
            Close(device);
            m_rescanRequested = true;
        }
    }

    if (m_rescanRequested) {
        m_rescanRequested = false;
        ScanDirectory();
    }

    for (uint32_t i = 0; i < MAX_GAMEPAD_COUNT; ++i) {
        inputs[i] = m_devices[i].fd >= 0 ? m_devices[i].input : GamepadInput{};
    }
}

void EvdevGamepadBackend::ScanDirectory()
{
    DIR* directory{ opendir(INPUT_DIRECTORY) };
    if (!directory) {
        return;
    }
    while (const dirent* entry = readdir(directory)) {
        if (IsEventNode(entry->d_name)) {
            TryOpen(std::string{ INPUT_DIRECTORY } + "/" + entry->d_name);
        }
    }
    closedir(directory);
}

void EvdevGamepadBackend::ProcessHotplug()
{
    if (m_inotifyFd < 0) {
        return;
    }

    alignas(inotify_event) char buffer[4096];
    while (true) {
        const ssize_t length{ read(m_inotifyFd, buffer, sizeof(buffer)) };
        if (length <= 0) {
            break;
        }
        for (ssize_t offset = 0; offset < length;) {
            const auto* event{ reinterpret_cast<const inotify_event*>(buffer + offset) };
            offset += static_cast<ssize_t>(sizeof(inotify_event) + event->len);

            if (event->mask & IN_Q_OVERFLOW) {
                m_rescanRequested = true;
                continue;
            }
            if (event->len == 0 || !IsEventNode(event->name)) {
                continue;
            }
            if (event->mask & (IN_CREATE | IN_ATTRIB | IN_MOVED_TO)) {
                TryOpen(std::string{ INPUT_DIRECTORY } + "/" + event->name);
            }
        }
    }
}

void EvdevGamepadBackend::TryOpen(const std::string& path)
{
    if (IsOpen(path)) {
        return;
    }

    Device* freeSlot{ nullptr };
    for (auto& device : m_devices) {
        if (device.fd < 0) {
            freeSlot = &device;
            break;
        }
    }
    if (!freeSlot) {
        return;
    }

    const int fd{ open(path.c_str(), O_RDONLY | O_NONBLOCK | O_CLOEXEC) };
    if (fd < 0) {
        return;
    }

    unsigned long keyBits[BitArraySize(KEY_CNT)]{};
    unsigned long absBits[BitArraySize(ABS_CNT)]{};
    unsigned long propBits[BitArraySize(INPUT_PROP_CNT)]{};
    ioctl(fd, EVIOCGBIT(EV_KEY, sizeof(keyBits)), keyBits);
    ioctl(fd, EVIOCGBIT(EV_ABS, sizeof(absBits)), absBits);
    ioctl(fd, EVIOCGPROP(sizeof(propBits)), propBits);

    input_id id{};
    const bool isGamepad{ TestBit(keyBits, BTN_SOUTH) && !TestBit(propBits, INPUT_PROP_ACCELEROMETER) };
    if (!isGamepad || ioctl(fd, EVIOCGID, &id) < 0 || IsIgnoredBySteam(id)) {
        close(fd);
        return;
    }

    char name[128]{};
    if (ioctl(fd, EVIOCGNAME(sizeof(name) - 1), name) < 0) {
        std::strncpy(name, "Unknown gamepad", sizeof(name) - 1);
    }

    Device& device{ *freeSlot };
    device = Device{};
    device.fd = fd;
    device.path = path;
    device.positionalFaceButtons = UsesPositionalFaceButtons(id.vendor);
    device.rightStickOnZRz = IsRightStickOnZRz(TestBit(absBits, ABS_Z), TestBit(absBits, ABS_RZ), TestBit(absBits, ABS_RX), TestBit(absBits, ABS_RY));
    device.input.connected = true;
    device.input.name = name;

    for (uint16_t code = 0; code < device.absRanges.size(); ++code) {
        input_absinfo info{};
        if (TestBit(absBits, code) && ioctl(fd, EVIOCGABS(code), &info) >= 0) {
            device.absRanges[code] = AxisRange{ info.minimum, info.maximum };
        }
    }

    SyncFromDevice(device);

    LOGI("Gamepad: opened %s (%s, %04x:%04x)", path.c_str(), name, id.vendor, id.product);
}

void EvdevGamepadBackend::Close(Device& device)
{
    if (device.fd >= 0) {
        close(device.fd);
    }
    device = Device{};
}

bool EvdevGamepadBackend::IsOpen(const std::string& path) const
{
    for (const auto& device : m_devices) {
        if (device.fd >= 0 && device.path == path) {
            return true;
        }
    }
    return false;
}

bool EvdevGamepadBackend::ReadEvents(Device& device)
{
    bool dropped{ false };
    input_event events[64];
    while (true) {
        const ssize_t length{ read(device.fd, events, sizeof(events)) };
        if (length < 0) {
            if (errno != EAGAIN && errno != EINTR) {
                return false;
            }
            break;
        }
        if (length == 0) {
            break;
        }

        const size_t count{ static_cast<size_t>(length) / sizeof(input_event) };
        for (size_t i = 0; i < count; ++i) {
            const input_event& event{ events[i] };
            if (event.type == EV_SYN && event.code == SYN_DROPPED) {
                dropped = true;
            } else if (event.type == EV_KEY) {
                if (event.value != 2) { // 2 = autorepeat
                    ApplyKey(device, event.code, event.value != 0);
                }
            } else if (event.type == EV_ABS) {
                ApplyAbs(device, event.code, event.value);
            }
        }
    }

    if (dropped) {
        SyncFromDevice(device);
    }
    return true;
}

void EvdevGamepadBackend::SyncFromDevice(Device& device)
{
    for (uint16_t code = 0; code < device.absRanges.size(); ++code) {
        input_absinfo info{};
        if (HasAbs(device, code) && ioctl(device.fd, EVIOCGABS(code), &info) >= 0) {
            ApplyAbs(device, code, info.value);
        }
    }

    unsigned long pressedKeys[BitArraySize(KEY_CNT)]{};
    if (ioctl(device.fd, EVIOCGKEY(sizeof(pressedKeys)), pressedKeys) >= 0) {
        for (uint16_t code = BTN_MISC; code < KEY_CNT; ++code) {
            ApplyKey(device, code, TestBit(pressedKeys, code));
        }
    }
}

bool EvdevGamepadBackend::HasAbs(const Device& device, const uint16_t code)
{
    return code < device.absRanges.size() && device.absRanges[code].max > device.absRanges[code].min;
}

void EvdevGamepadBackend::ApplyKey(Device& device, const uint16_t code, const bool pressed)
{
    if (const auto button{ MapKey(code, device.positionalFaceButtons) }) {
        device.input.SetButton(*button, pressed);
    } else if (const auto trigger{ MapDigitalTrigger(code) }) {
        const bool left{ *trigger == GamepadAxis::LEFT_TRIGGER };
        const bool hasAnalogTrigger{ (!device.rightStickOnZRz && HasAbs(device, left ? ABS_Z : ABS_RZ)) || HasAbs(device, left ? ABS_BRAKE : ABS_GAS) };
        if (!hasAnalogTrigger) {
            device.input.SetAxis(*trigger, pressed ? 1.0f : 0.0f);
        }
    }
}

void EvdevGamepadBackend::ApplyAbs(Device& device, const uint16_t code, const int32_t value)
{
    const auto mapping{ MapAbs(code, device.rightStickOnZRz) };
    if (!mapping || !HasAbs(device, code)) {
        return;
    }
    const AxisRange& range{ device.absRanges[code] };
    switch (mapping->role) {
    case AbsRole::STICK_X:
        device.input.SetAxis(mapping->axis, util::NormalizeCentered(value, range.min, range.max));
        break;
    case AbsRole::STICK_Y:
        device.input.SetAxis(mapping->axis, -util::NormalizeCentered(value, range.min, range.max));
        break;
    case AbsRole::TRIGGER:
        device.input.SetAxis(mapping->axis, util::NormalizeUnsigned(value, range.min, range.max));
        break;
    case AbsRole::HAT_X:
        device.input.SetButton(GamepadButton::DPAD_LEFT, value < 0);
        device.input.SetButton(GamepadButton::DPAD_RIGHT, value > 0);
        break;
    case AbsRole::HAT_Y:
        device.input.SetButton(GamepadButton::DPAD_UP, value < 0);
        device.input.SetButton(GamepadButton::DPAD_DOWN, value > 0);
        break;
    }
}
} // namespace prev::input::gamepad::impl::evdev

#endif
