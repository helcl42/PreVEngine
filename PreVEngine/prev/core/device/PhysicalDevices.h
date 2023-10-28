#ifndef __PHYSICAL_DEVICES_H__
#define __PHYSICAL_DEVICES_H__

#include "PhysicalDevice.h"

#include <memory>
#include <vector>

namespace prev::core::device {
class PhysicalDevices {
public:
    PhysicalDevices(const VkInstance instance);

public:
    std::shared_ptr<PhysicalDevice> FindPresentable(const VkSurfaceKHR surface, const int32_t hintIndex = -1) const; // Returns first device able to present to surface, or null if none.

    void Print(bool showQueues = false) const;

    size_t GetCount() const;

    const PhysicalDevice& operator[](const size_t i) const;

private:
    std::vector<std::shared_ptr<PhysicalDevice>> m_gpuList;
};
} // namespace prev::core::device

#endif
