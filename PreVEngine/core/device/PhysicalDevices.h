#ifndef __PHYSICAL_DEVICES_H__
#define __PHYSICAL_DEVICES_H__

#include "PhysicalDevice.h"

#include <vector>

namespace PreVEngine {
class PhysicalDevices {
public:
    PhysicalDevices(const VkInstance instance);

    PhysicalDevice* FindPresentable(VkSurfaceKHR surface); // Returns first device able to present to surface, or null if none.

    void Print(bool showQueues = false);

public:
    size_t GetCount() const;

    const PhysicalDevice& operator[](const size_t i) const;

private:
    std::vector<PhysicalDevice> m_gpuList;
};
} // namespace PreVEngine

#endif
