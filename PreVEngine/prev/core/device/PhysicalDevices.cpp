#include "PhysicalDevices.h"

#include "../../common/Logger.h"

namespace prev::core::device {
PhysicalDevices::PhysicalDevices(GfxInstance instance)
{
    uint32_t count{ 0 };
    gfxInstanceEnumerateAdapters(instance, &count, nullptr);
    if (count == 0) {
        LOGW("No GPU adapters found.");
        return;
    }
    std::vector<GfxAdapter> adapters(count);
    gfxInstanceEnumerateAdapters(instance, &count, adapters.data());

    m_adapters.resize(count);
    for (size_t i = 0; i < count; ++i) {
        m_adapters[i] = std::make_unique<PhysicalDevice>(adapters[i]);
    }
}

std::optional<PhysicalDevice> PhysicalDevices::Find(GfxSurface surface, int32_t hintIndex) const
{
    if (m_adapters.empty()) {
        return {};
    }

    auto canPresent = [&](const PhysicalDevice& adapter) -> bool {
        if (!surface) {
            return true;
        }
        uint32_t familyCount{ 0 };
        gfxAdapterEnumerateQueueFamilies(static_cast<GfxAdapter>(adapter), &familyCount, nullptr);
        for (uint32_t i = 0; i < familyCount; ++i) {
            bool supported{ false };
            gfxAdapterGetQueueFamilySurfaceSupport(static_cast<GfxAdapter>(adapter), i, surface, &supported);
            if (supported) {
                return true;
            }
        }
        return false;
    };

    if (hintIndex >= 0 && hintIndex < static_cast<int32_t>(m_adapters.size())) {
        if (canPresent(*m_adapters[hintIndex])) {
            return *m_adapters[hintIndex];
        }
        LOGW("Adapter at index %d cannot present to the given surface.", hintIndex);
        return {};
    }
    // Prefer discrete GPU that can present
    for (const auto& adapter : m_adapters) {
        const auto info{ adapter->GetInfo() };
        if (info.adapterType == GFX_ADAPTER_TYPE_DISCRETE_GPU && canPresent(*adapter)) {
            return *adapter;
        }
    }
    // Fall back to first adapter that can present
    for (const auto& adapter : m_adapters) {
        if (canPresent(*adapter)) {
            return *adapter;
        }
    }
    LOGW("No adapter found that can present to the given surface.");
    return {};
}

void PhysicalDevices::Print() const
{
    LOGI("Physical Devices: %zu", GetCount());
    for (size_t i = 0; i < m_adapters.size(); ++i) {
        LOGI("Adapter %zu:", i);
        m_adapters[i]->Print();
    }
}

size_t PhysicalDevices::GetCount() const
{
    return m_adapters.size();
}

const PhysicalDevice& PhysicalDevices::operator[](size_t i) const
{
    return *m_adapters[i];
}
} // namespace prev::core::device
