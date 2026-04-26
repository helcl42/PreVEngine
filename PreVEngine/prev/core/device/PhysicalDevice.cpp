#include "PhysicalDevice.h"
#include "../../common/Logger.h"
#include "../../util/MathUtils.h"

namespace prev::core::device {
PhysicalDevice::PhysicalDevice(GfxAdapter adapter)
    : m_adapter{ adapter }
{
}

GfxAdapterInfo PhysicalDevice::GetInfo() const
{
    GfxAdapterInfo info{};
    gfxAdapterGetInfo(m_adapter, &info);
    return info;
}

GfxDeviceLimits PhysicalDevice::GetLimits() const
{
    GfxDeviceLimits limits{};
    gfxAdapterGetLimits(m_adapter, &limits);
    return limits;
}

std::vector<GfxQueueFamilyProperties> PhysicalDevice::GetQueueFamilies() const
{
    uint32_t count{ 0 };
    gfxAdapterEnumerateQueueFamilies(m_adapter, &count, nullptr);
    std::vector<GfxQueueFamilyProperties> families(count);
    gfxAdapterEnumerateQueueFamilies(m_adapter, &count, families.data());
    return families;
}

int32_t PhysicalDevice::FindQueueFamily(GfxQueueFlags flags, GfxQueueFlags unwantedFlags, GfxSurface surface) const
{
    const auto families{ GetQueueFamilies() };
    for (uint32_t i = 0; i < static_cast<uint32_t>(families.size()); ++i) {
        const auto& f{ families[i] };
        if (!prev::util::math::HasAllFlagsSet(f.flags, flags)) {
            continue;
        }
        if (prev::util::math::HasAnyFlagsSet(f.flags, unwantedFlags)) {
            continue;
        }
        if (surface) {
            bool canPresent{ false };
            gfxAdapterGetQueueFamilySurfaceSupport(m_adapter, i, surface, &canPresent);
            if (!canPresent) {
                continue;
            }
        }
        return static_cast<int32_t>(i);
    }
    return -1;
}

void PhysicalDevice::Print() const
{
    const auto info{ GetInfo() };
    LOGI("Physical Device: %s (vendor: 0x%X device: 0x%X)", info.name, info.vendorID, info.deviceID);
}

PhysicalDevice::operator GfxAdapter() const
{
    return m_adapter;
}
} // namespace prev::core::device
