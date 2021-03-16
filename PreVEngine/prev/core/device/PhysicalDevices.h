#ifndef __PHYSICAL_DEVICES_H__
#define __PHYSICAL_DEVICES_H__

#include "PhysicalDevice.h"

#include <vector>
#include <memory>

namespace prev::core::device {
class PhysicalDevices {
public:
    PhysicalDevices(const VkInstance instance);

    std::shared_ptr<PhysicalDevice> FindPresentable(VkSurfaceKHR surface) const; // Returns first device able to present to surface, or null if none.

    void Print(bool showQueues = false) const;

public:
    size_t GetCount() const;

    const PhysicalDevice& operator[](const size_t i) const;

private:
    std::vector<std::shared_ptr<PhysicalDevice>> m_gpuList;
};
} // namespace prev::core::device

#endif
