#include "PhysicalDevices.h"

namespace prev::core::device {
PhysicalDevices::PhysicalDevices(const VkInstance instance)
{
    uint32_t gpuCount = 0;
    VKERRCHECK(vkEnumeratePhysicalDevices(instance, &gpuCount, nullptr)); // Get number of gpu's
    if (gpuCount == 0) {
        LOGW("No GPU devices found."); // Vulkan driver missing?
    }

    std::vector<VkPhysicalDevice> vkGpus(gpuCount);
    vkEnumeratePhysicalDevices(instance, &gpuCount, vkGpus.data());

    m_gpuList.resize(gpuCount);
    for (size_t i = 0; i < vkGpus.size(); i++) {
        m_gpuList[i] = PhysicalDevice(vkGpus.at(i));
    }
}

PhysicalDevice* PhysicalDevices::FindPresentable(VkSurfaceKHR surface)
{
    for (auto& gpu : m_gpuList) {
        if (gpu.FindQueueFamily(0, 0, surface) >= 0) {
            return &gpu;
        }
    }

    LOGW("No devices can present to this surface. (Is DRI3 enabled?)\n");
    return nullptr;
}

void PhysicalDevices::Print(bool showQueues)
{
    printf("Physical Devices: %zd\n", GetCount());

    size_t j = 0;
    for (const auto& gpu : m_gpuList) {
        const VkPhysicalDeviceProperties& props = gpu.GetProperties();
        std::string devType[] = { "OTHER", "INTEGRATED", "DISCRETE", "VIRTUAL", "CPU" };
        std::string vendor = gpu.GetVendorName();
        printf("\t%zd: %s %s %s\n", j, devType[props.deviceType].c_str(), vendor.c_str(), props.deviceName);

        if (showQueues) {
            const auto queueFamilies = gpu.GetQueueFamilies();
            for (size_t i = 0; i < queueFamilies.size(); i++) {
                const VkQueueFamilyProperties& props = queueFamilies.at(i);

                uint32_t flags = props.queueFlags;
                printf("\t\tQueue-family:%zd  count:%2d  flags:[ %s%s%s%s]\n", i, props.queueCount, (flags & 1) ? "GRAPHICS " : "", (flags & 2) ? "COMPUTE " : "", (flags & 4) ? "TRANSFER " : "", (flags & 8) ? "SPARSE " : "");
            }
        }
        j++;
    }
}

size_t PhysicalDevices::GetCount() const
{
    return m_gpuList.size();
}

const PhysicalDevice& PhysicalDevices::operator[](const size_t i) const
{
    return m_gpuList.at(i);
}
} // namespace prev::core::device