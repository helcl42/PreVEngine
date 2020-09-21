#include "Device.h"

namespace prev::core::device {
std::shared_ptr<Queue> Device::AddQueue(VkQueueFlags flags, VkSurfaceKHR surface)
{
    ASSERT(!m_handle, "Can't add queues after device is already in use. ");

    uint32_t familyIndex = m_gpu.FindQueueFamily(flags, surface);
    if (familyIndex < 0) {
        LOGW("Could not create queue with requested properties.");
        return nullptr;
    }

    uint32_t max = m_gpu.GetQueueFamilies().at(familyIndex).queueCount;
    uint32_t queueIndex = FamilyQueueCount(familyIndex);
    if (queueIndex == max) {
        LOGW("No more queues available from this family.");
        return nullptr;
    }

    auto queue = std::make_shared<Queue>(nullptr, familyIndex, queueIndex, flags, surface, m_handle, m_gpu);
    m_queues.push_back(queue);

    LOGI("Queue: %d  flags: [ %s%s%s%s]%s\n", queueIndex, (flags & 1) ? "GRAPHICS " : "", (flags & 2) ? "COMPUTE " : "", (flags & 4) ? "TRANSFER " : "", (flags & 8) ? "SPARSE " : "", surface ? " (can present)" : "");

    return queue;
}

PhysicalDevice& Device::GetGPU()
{
    return m_gpu;
}

Device::operator VkDevice()
{
    if (m_handle == nullptr) // make it lazy or const ??!
    {
        Create();
    }
    return m_handle;
}

uint32_t Device::FamilyQueueCount(uint32_t family) const
{
    uint32_t count = 0;
    for (auto& q : m_queues) {
        if (q->family == family) {
            count++;
        }
    }
    return count;
}

void Device::Create()
{
    if (m_handle != nullptr) {
        Destroy();
    }

    std::vector<float> priorities(m_queues.size(), 0.0f);
    std::vector<VkDeviceQueueCreateInfo> infoList;

    const auto queueFamilies = m_gpu.GetQueueFamilies();

    const uint32_t queueFamiliesCount = static_cast<uint32_t>(queueFamilies.size());
    for (uint32_t i = 0; i < queueFamiliesCount; i++) {
        uint32_t queueCount = FamilyQueueCount(i);
        if (queueCount > 0) {
            VkDeviceQueueCreateInfo info = {};
            info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            info.queueFamilyIndex = static_cast<uint32_t>(i);
            info.queueCount = queueCount;
            info.pQueuePriorities = priorities.data();
            infoList.push_back(info);
        }
    }

    DeviceExtensions& extensions = m_gpu.GetExtensions();
    VkDeviceCreateInfo deviceCreateInfo = { VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
    deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(infoList.size());
    deviceCreateInfo.pQueueCreateInfos = infoList.data();
    deviceCreateInfo.enabledExtensionCount = extensions.PickCount();
    deviceCreateInfo.ppEnabledExtensionNames = extensions.GetPickList();
    deviceCreateInfo.pEnabledFeatures = &m_gpu.GetEnabledFeatures();
    VKERRCHECK(vkCreateDevice(m_gpu, &deviceCreateInfo, nullptr, &m_handle)); // create device

    for (auto& q : m_queues) {
        q->device = m_handle;
        vkGetDeviceQueue(m_handle, q->family, q->index, &q->handle); // get queue handles
    }
}

void Device::Destroy()
{
    if (m_handle == nullptr) {
        return;
    }

    vkDeviceWaitIdle(m_handle);
    vkDestroyDevice(m_handle, nullptr);

    m_handle = nullptr;
}

Device::Device(PhysicalDevice& inGpu)
    : m_handle(nullptr)
    , m_gpu(inGpu)
{
    m_queues.reserve(16);

    LOGI("Logical Device using GPU: %s\n", m_gpu.GetProperties().deviceName);

#ifdef ENABLE_VALIDATION
    m_gpu.GetExtensions().Print();
#endif
}

Device::~Device()
{
    Destroy();
    LOGI("Logical device destroyed\n");
}

void Device::Print() const
{
    printf("Logical Device used queues:\n");
    for (size_t i = 0; i < m_queues.size(); i++) {
        const auto& q = m_queues.at(i);
        printf("\t%zd: family=%d index=%d presentable=%s flags=", i, q->family, q->index, (q->surface == VK_NULL_HANDLE ? "False" : "True"));

        const char* famillyNames[] = { "GRAPHICS", "COMPUTE", "TRANSFER", "SPARSE", "PROTECTED" };
        for (int j = 0; j < 5; j++) {
            if (q->flags & 1 << j) {
                printf("%s ", famillyNames[j]);
            }
        }
        printf("\n");
    }
}
} // namespace prev::core::device
