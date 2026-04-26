#ifndef __DEVICE_FACTORY_H__
#define __DEVICE_FACTORY_H__

#include "Device.h"
#include "PhysicalDevice.h"

#include <memory>
#include <string>
#include <vector>

namespace prev::core::device {
class DeviceFactory final {
public:
    std::unique_ptr<Device> Create(const PhysicalDevice& gpu, const std::vector<std::string>& extensions = {}) const;
};
} // namespace prev::core::device

#endif
