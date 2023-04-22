#include "Device.h"

#include <algorithm>
#include <iterator>

namespace prev::core::device {
Device::Device(const std::shared_ptr<PhysicalDevice>& gpu, const QueueMetadataStorage& queuesMetadata)
    : m_gpu(gpu)
{
    LOGI("Logical Device using GPU: %s\n", m_gpu->GetProperties().deviceName);

#ifdef ENABLE_VALIDATION
    m_gpu->GetExtensions().Print();
#endif

    const auto distinctQueueFamilyIndices{ queuesMetadata.GetDistinctQueueFamiies() };

    std::vector<float> priorities(distinctQueueFamilyIndices.size(), 0.0f);
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfoList;
    for (const auto queueFamilyIndex : distinctQueueFamilyIndices) {
        const auto queueCount{ queuesMetadata.GetQueueFamilyCount(queueFamilyIndex) };
        VkDeviceQueueCreateInfo info = { VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };
        info.queueFamilyIndex = queueFamilyIndex;
        info.queueCount = queueCount;
        info.pQueuePriorities = priorities.data();
        queueCreateInfoList.push_back(info);
    }

    const auto& extensions{ m_gpu->GetExtensions() };
    VkDeviceCreateInfo deviceCreateInfo = { VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
    deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfoList.size());
    deviceCreateInfo.pQueueCreateInfos = queueCreateInfoList.data();
    deviceCreateInfo.enabledExtensionCount = extensions.PickCount();
    deviceCreateInfo.ppEnabledExtensionNames = extensions.GetPickList();
    deviceCreateInfo.pEnabledFeatures = &m_gpu->GetEnabledFeatures();
    VKERRCHECK(vkCreateDevice(*m_gpu, &deviceCreateInfo, nullptr, &m_handle)); // create device

    for (const auto& [queueGroupKey, queueGroupList] : queuesMetadata.queueGroups) {
        for (const auto& groupItem : queueGroupList) {
            VkQueue vkQueue;
            vkGetDeviceQueue(m_handle, groupItem.family, groupItem.index, &vkQueue);
            m_queues[queueGroupKey].push_back(std::make_shared<Queue>(vkQueue, groupItem.family, groupItem.index, groupItem.flags, groupItem.surface, m_handle));
        }
    }
}

Device::~Device()
{
    vkDeviceWaitIdle(m_handle);
    vkDestroyDevice(m_handle, nullptr);

    LOGI("Logical device destroyed\n");
}

void Device::Print() const
{
    const std::map<QueueType, std::string> queuNames = {
        { QueueType::PRESENT, "PRESENT" },
        { QueueType::GRAPHICS, "GRAPHICS" },
        { QueueType::COMPUTE, "COMPUTE" },
        { QueueType::TRANSFER, "TRANSFER" },
        { QueueType::SPARSE, "SPARSE" },
        { QueueType::PROTECTED, "PROTECTED" }
    };

    LOGI("Logical Device used queues:\n");
    for (const auto& [qGroupKey, gQroupList] : m_queues) {
        for (const auto& qGroupItem : gQroupList) {
            LOGI("Queue: %s index: %d family: %d flags: [ %s%s%s%s%s]%s\n", queuNames.at(qGroupKey).c_str(), qGroupItem->index, qGroupItem->family, (qGroupItem->flags & 1) ? "GRAPHICS " : "", (qGroupItem->flags & 2) ? "COMPUTE " : "", (qGroupItem->flags & 4) ? "TRANSFER " : "", (qGroupItem->flags & 8) ? "SPARSE " : "", (qGroupItem->flags & 16) ? "PROTECTED" : "", qGroupItem->surface ? " (can present)" : "");
        }
    }
}

std::shared_ptr<Queue> Device::GetQueue(const QueueType queueType, const uint32_t index) const
{
    const auto queuesIter{ m_queues.find(queueType) };
    if (queuesIter == m_queues.cend()) {
        LOGE("Trying to retrieve invalid QueueType.\n");
        return nullptr;
    }

    const auto& queuesGroup{ queuesIter->second };
    if (index >= queuesGroup.size()) {
        LOGE("Trying access queue at invalid index %ud.\n", index);
        return nullptr;
    }

    return queuesGroup.at(index);
}

std::vector<std::shared_ptr<Queue>> Device::GetQueues(const QueueType queueType) const
{
    const auto queuesIter{ m_queues.find(queueType) };
    if (queuesIter == m_queues.cend()) {
        LOGE("Trying to retrieve invalid QueueType.\n");
        return {};
    }
    return queuesIter->second;
}

std::map<QueueType, std::vector<std::shared_ptr<Queue>>> Device::GetAllQueues() const
{
    return m_queues;
}

std::vector<QueueType> Device::GetAllQueueTypes() const
{
    std::vector<QueueType> result;
    std::transform(m_queues.begin(), m_queues.end(), std::back_inserter(result), [](const auto& pair) { return pair.first; });
    return result;
}

std::shared_ptr<PhysicalDevice> Device::GetGPU() const
{
    return m_gpu;
}

Device::operator VkDevice() const
{
    return m_handle;
}
} // namespace prev::core::device
