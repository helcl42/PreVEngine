#ifndef __EVDEV_MAPPING_H__
#define __EVDEV_MAPPING_H__

#include "../../GamepadCommon.h"

#include <linux/input.h>

#include <cstdint>
#include <cstdlib>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

namespace prev::input::gamepad::impl::evdev {
constexpr uint16_t VENDOR_SONY{ 0x054c };
constexpr uint16_t VENDOR_NINTENDO{ 0x057e };

// 0x133/0x134 are BTN_X/BTN_Y by letter for Xbox-style drivers, but BTN_NORTH/BTN_WEST by position for hid-playstation and hid-nintendo.
inline bool UsesPositionalFaceButtons(const uint16_t vendorId)
{
    return vendorId == VENDOR_SONY || vendorId == VENDOR_NINTENDO;
}

inline std::optional<GamepadButton> MapKey(const uint16_t code, const bool positionalFaceButtons)
{
    switch (code) {
    case BTN_SOUTH:
        return GamepadButton::SOUTH;
    case BTN_EAST:
        return GamepadButton::EAST;
    case BTN_X:
        return positionalFaceButtons ? GamepadButton::NORTH : GamepadButton::WEST;
    case BTN_Y:
        return positionalFaceButtons ? GamepadButton::WEST : GamepadButton::NORTH;
    case BTN_TL:
        return GamepadButton::LEFT_SHOULDER;
    case BTN_TR:
        return GamepadButton::RIGHT_SHOULDER;
    case BTN_SELECT:
        return GamepadButton::BACK;
    case BTN_START:
        return GamepadButton::START;
    case BTN_MODE:
        return GamepadButton::GUIDE;
    case BTN_THUMBL:
        return GamepadButton::LEFT_STICK;
    case BTN_THUMBR:
        return GamepadButton::RIGHT_STICK;
    case BTN_DPAD_UP:
        return GamepadButton::DPAD_UP;
    case BTN_DPAD_DOWN:
        return GamepadButton::DPAD_DOWN;
    case BTN_DPAD_LEFT:
        return GamepadButton::DPAD_LEFT;
    case BTN_DPAD_RIGHT:
        return GamepadButton::DPAD_RIGHT;
    default:
        return std::nullopt;
    }
}

inline std::optional<GamepadAxis> MapDigitalTrigger(const uint16_t code)
{
    switch (code) {
    case BTN_TL2:
        return GamepadAxis::LEFT_TRIGGER;
    case BTN_TR2:
        return GamepadAxis::RIGHT_TRIGGER;
    default:
        return std::nullopt;
    }
}

enum class AbsRole : uint32_t {
    STICK_X,
    STICK_Y,
    TRIGGER,
    HAT_X,
    HAT_Y,
};

struct AbsMapping {
    AbsRole role{};

    GamepadAxis axis{};
};

// Generic HID pads have no RX/RY: their right stick is on Z/RZ and their triggers are buttons.
inline bool IsRightStickOnZRz(const bool hasZ, const bool hasRz, const bool hasRx, const bool hasRy)
{
    return hasZ && hasRz && !hasRx && !hasRy;
}

inline std::optional<AbsMapping> MapAbs(const uint16_t code, const bool rightStickOnZRz)
{
    switch (code) {
    case ABS_X:
        return AbsMapping{ AbsRole::STICK_X, GamepadAxis::LEFT_X };
    case ABS_Y:
        return AbsMapping{ AbsRole::STICK_Y, GamepadAxis::LEFT_Y };
    case ABS_RX:
        return AbsMapping{ AbsRole::STICK_X, GamepadAxis::RIGHT_X };
    case ABS_RY:
        return AbsMapping{ AbsRole::STICK_Y, GamepadAxis::RIGHT_Y };
    case ABS_Z:
        return rightStickOnZRz ? AbsMapping{ AbsRole::STICK_X, GamepadAxis::RIGHT_X } : AbsMapping{ AbsRole::TRIGGER, GamepadAxis::LEFT_TRIGGER };
    case ABS_RZ:
        return rightStickOnZRz ? AbsMapping{ AbsRole::STICK_Y, GamepadAxis::RIGHT_Y } : AbsMapping{ AbsRole::TRIGGER, GamepadAxis::RIGHT_TRIGGER };
    case ABS_BRAKE:
        return AbsMapping{ AbsRole::TRIGGER, GamepadAxis::LEFT_TRIGGER };
    case ABS_GAS:
        return AbsMapping{ AbsRole::TRIGGER, GamepadAxis::RIGHT_TRIGGER };
    case ABS_HAT0X:
        return AbsMapping{ AbsRole::HAT_X, {} };
    case ABS_HAT0Y:
        return AbsMapping{ AbsRole::HAT_Y, {} };
    default:
        return std::nullopt;
    }
}

struct UsbId {
    uint16_t vendor{};

    uint16_t product{};
};

inline std::vector<UsbId> ParseUsbIdList(const char* value)
{
    std::vector<UsbId> result;
    if (!value) {
        return result;
    }
    std::stringstream stream{ value };
    std::string item;
    while (std::getline(stream, item, ',')) {
        const auto slash{ item.find('/') };
        if (slash == std::string::npos) {
            continue;
        }
        const unsigned long vendor{ std::strtoul(item.substr(0, slash).c_str(), nullptr, 0) };
        const unsigned long product{ std::strtoul(item.substr(slash + 1).c_str(), nullptr, 0) };
        result.push_back({ static_cast<uint16_t>(vendor), static_cast<uint16_t>(product) });
    }
    return result;
}

inline bool IsIgnored(const std::vector<UsbId>& ignored, const std::vector<UsbId>& ignoredExcept, const UsbId& id)
{
    const auto contains = [&id](const std::vector<UsbId>& list) {
        for (const auto& entry : list) {
            if (entry.vendor == id.vendor && entry.product == id.product) {
                return true;
            }
        }
        return false;
    };
    if (!ignoredExcept.empty()) {
        return !contains(ignoredExcept);
    }
    return contains(ignored);
}
} // namespace prev::input::gamepad::impl::evdev

#endif // !__EVDEV_MAPPING_H__
