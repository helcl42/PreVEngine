#ifndef __PHYSICAL_DEVICE_H__
#define __PHYSICAL_DEVICE_H__

#include "../Core.h"

#include <cstdint>
#include <string>
#include <vector>

namespace prev::core::device {
class PhysicalDevice {
public:
    PhysicalDevice(GfxAdapter adapter);

    ~PhysicalDevice() = default;

public:
    GfxAdapterInfo GetInfo() const;

    GfxDeviceLimits GetLimits() const;

    std::vector<GfxQueueFamilyProperties> GetQueueFamilies() const;

    // Returns the index of a queue family matching flags, or -1 if not found.
    // Pass a valid surface to require present support.
    int32_t FindQueueFamily(GfxQueueFlags flags, GfxQueueFlags unwantedFlags = 0, GfxSurface surface = nullptr) const;

    void Print() const;

public:
    operator GfxAdapter() const;

private:
    GfxAdapter m_adapter;
};
} // namespace prev::core::device

#endif