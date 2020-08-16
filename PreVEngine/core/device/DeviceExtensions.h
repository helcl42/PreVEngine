#ifndef __DEVICE_EXTENSIONS_H__
#define __DEVICE_EXTENSIONS_H__

#include "../../common/PickList.h"
#include "../instance/Validation.h"

namespace prev {
class DeviceExtensions final : public prev::common::PickList {
public:
    DeviceExtensions() = default;

    DeviceExtensions& operator=(const DeviceExtensions& other);

    DeviceExtensions(const DeviceExtensions& other);

public:
    void Init(VkPhysicalDevice phy, const char* layerName = nullptr);

public:
    const char* GetNameByIndex(uint32_t inx) const override;

    uint32_t GetPickedCount() const override;

    std::string GetName() const override;

private:
    std::vector<VkExtensionProperties> m_itemList;
};
} // namespace prev

#endif