#ifndef __DEVICE_EXTENSIONS_H__
#define __DEVICE_EXTENSIONS_H__

#include "../../common/PickList.h"

#include "../Core.h"

namespace prev::core::device {
class DeviceExtensions final : public prev::common::PickList<VkExtensionProperties> {
public:
    DeviceExtensions() = default;

    DeviceExtensions(const VkPhysicalDevice gpu, const char* layerName = nullptr);

    DeviceExtensions(const DeviceExtensions& other);

    DeviceExtensions& operator=(const DeviceExtensions& other);

public:
    std::string GetNameByIndex(const uint32_t index) const override;

    std::string GetName() const override;
};
} // namespace prev::core::device

#endif