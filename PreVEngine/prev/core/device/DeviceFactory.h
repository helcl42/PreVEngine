#ifndef __DEVICE_FACTORY_H__
#define __DEVICE_FACTORY_H__

#include "PhysicalDevice.h"

#include <memory>

namespace prev::core::device {
class Device;

class DeviceFactory {
public:
    DeviceFactory() = default;

    ~DeviceFactory() = default;

public:
    std::unique_ptr<Device> Create(const PhysicalDevice& gpu, const VkSurfaceKHR surface) const;
};
} // namespace prev::core::device

#endif // !__DEVICE_FACTORY_H__
