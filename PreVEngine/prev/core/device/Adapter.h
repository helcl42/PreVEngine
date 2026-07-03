#ifndef __ADAPTER_H__
#define __ADAPTER_H__

#include "../Core.h"

#include <cstdint>
#include <string>
#include <vector>

namespace prev::core::device {
class Adapter {
public:
    Adapter(GfxAdapter adapter);

    ~Adapter() = default;

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