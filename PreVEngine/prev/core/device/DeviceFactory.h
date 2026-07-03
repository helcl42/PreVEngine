#ifndef __DEVICE_FACTORY_H__
#define __DEVICE_FACTORY_H__

#include "Device.h"
#include "Adapter.h"

#include <memory>
#include <string>
#include <vector>

namespace prev::core::device {
class DeviceFactory final {
public:
    std::unique_ptr<Device> Create(const Adapter& adapter, const std::vector<std::string>& extensions = {}, const std::vector<std::string>& nativeExtensions = {}) const;
};
} // namespace prev::core::device

#endif
