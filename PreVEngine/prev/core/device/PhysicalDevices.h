#ifndef __PHYSICAL_DEVICES_H__
#define __PHYSICAL_DEVICES_H__

#include "PhysicalDevice.h"

#include <memory>
#include <optional>
#include <vector>

namespace prev::core::device {
class PhysicalDevices {
public:
    PhysicalDevices(GfxInstance instance);

public:
    // Returns an adapter at hintIndex, or the first discrete/high-performance one if hintIndex < 0.
    // If surface is provided, only adapters that can present to it are considered.
    std::optional<PhysicalDevice> Find(GfxSurface surface, int32_t hintIndex = -1) const;

    void Print() const;

    size_t GetCount() const;

    const PhysicalDevice& operator[](size_t i) const;

private:
    std::vector<std::unique_ptr<PhysicalDevice>> m_adapters;
};
} // namespace prev::core::device

#endif
