#include "PhysicalDevices.h"

#include "../../common/Logger.h"

namespace prev::core::device {
PhysicalDevices::PhysicalDevices(const VkInstance instance)
{
    uint32_t gpuCount{ 0 };
    VKERRCHECK(vkEnumeratePhysicalDevices(instance, &gpuCount, nullptr)); // Get number of gpu's
    if (gpuCount == 0) {
        LOGW("No GPU devices found."); // Vulkan driver missing?
    }

    std::vector<VkPhysicalDevice> vkGpus(gpuCount);
    vkEnumeratePhysicalDevices(instance, &gpuCount, vkGpus.data());

    m_gpuList.resize(gpuCount);
    for (size_t i = 0; i < vkGpus.size(); ++i) {
        m_gpuList[i] = PhysicalDevice(vkGpus[i]);
    }
}

std::optional<PhysicalDevice> PhysicalDevices::FindPresentable(const VkSurfaceKHR surface, const int32_t hintIndex) const
{
    if (hintIndex < 0 || hintIndex >= static_cast<int32_t>(m_gpuList.size())) {
        for (const auto& gpu : m_gpuList) {
            if (gpu.FindQueueFamily(0, 0, surface) >= 0) {
                return gpu;
            }
        }
        LOGW("No devices can present to this surface.");
    } else {
        const auto& gpu{ m_gpuList[hintIndex] };
        if (gpu.FindQueueFamily(0, 0, surface) >= 0) {
            return gpu;
        }
        LOGW("Devices at index %d can not present to this surface.", hintIndex);
    }
    return {};
}

void PhysicalDevices::Print(bool showQueues) const
{
    LOGI("Physical Devices: %zd", GetCount());

    size_t j{ 0 };
    for (const auto& gpu : m_gpuList) {
        LOGI("GPU: %zd:", j);
        gpu.Print(true);
        ++j;
    }
}

size_t PhysicalDevices::GetCount() const
{
    return m_gpuList.size();
}

const PhysicalDevice& PhysicalDevices::operator[](const size_t i) const
{
    return m_gpuList[i];
}
} // namespace prev::core::device