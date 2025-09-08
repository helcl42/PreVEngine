#ifndef __DEVICE_EXTENSIONS_H__
#define __DEVICE_EXTENSIONS_H__

#include "../../common/PickList.h"

#include "../Core.h"

namespace prev::core::device {
class DeviceExtensions final : public prev::common::PickList {
public:
    DeviceExtensions() = default;

    DeviceExtensions(const VkPhysicalDevice gpu, const char* layerName = nullptr);
};
} // namespace prev::core::device

#endif