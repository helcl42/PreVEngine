#include "PhysicalDevices.h"

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
        m_gpuList[i] = std::make_shared<PhysicalDevice>(vkGpus[i]);
    }
}

std::shared_ptr<PhysicalDevice> PhysicalDevices::FindPresentable(const VkSurfaceKHR surface, const int32_t hintIndex) const
{
    if (hintIndex < 0 || hintIndex >= static_cast<int32_t>(m_gpuList.size())) {
        for (const auto& gpu : m_gpuList) {
            if (gpu->FindQueueFamily(0, 0, surface) >= 0) {
                return gpu;
            }
        }
        LOGW("No devices can present to this surface.\n");
    } else {
        const auto& gpu{ m_gpuList[hintIndex] };
        if (gpu->FindQueueFamily(0, 0, surface) >= 0) {
            return gpu;
        }
        LOGW("Devices at index %d can not present to this surface.\n", hintIndex);
    }
    return nullptr;
}

void PhysicalDevices::Print(bool showQueues) const
{
    LOGI("Physical Devices: %zd\n", GetCount());

    size_t j{ 0 };
    for (const auto& gpu : m_gpuList) {
        LOGI("GPU: %zd:\n", j);
        gpu->Print(true);
        ++j;
    }
}

size_t PhysicalDevices::GetCount() const
{
    return m_gpuList.size();
}

const PhysicalDevice& PhysicalDevices::operator[](const size_t i) const
{
    return *m_gpuList.at(i);
}
} // namespace prev::core::device