﻿#include "Device.h"

#include "../../common/Logger.h"
#include "../../util/VkUtils.h"

#include <algorithm>
#include <iterator>
#include <sstream>
#include <stdexcept>

namespace prev::core::device {
// QueueMetadata
bool operator<(const QueueMetadata& a, const QueueMetadata& b)
{
    if (a.family >= b.family) {
        return false;
    }
    if (a.index >= b.index) {
        return false;
    }
    return true;
}

// QueuesMetadata
void QueuesMetadata::Add(const std::vector<QueueType>& queueTypes, const QueueMetadata& queueMetadata)
{
    for (const auto& queueType : queueTypes) {
        queueGroups[queueType].insert(queueMetadata);
    }
}

std::vector<uint32_t> QueuesMetadata::GetQueueFamilies() const
{
    std::set<uint32_t> distinctQueueFamilies;
    for (const auto& [queueType, queues] : queueGroups) {
        for (const auto& queue : queues) {
            distinctQueueFamilies.insert(queue.family);
        }
    }
    return std::vector<uint32_t>(distinctQueueFamilies.cbegin(), distinctQueueFamilies.cend());
}

uint32_t QueuesMetadata::GetQueueFamilyCount(const uint32_t family) const
{
    std::set<uint32_t> distinctQueues;
    for (const auto& [queueType, queues] : queueGroups) {
        for (const auto& queue : queues) {
            if (queue.family == family) {
                distinctQueues.insert(queue.index);
            }
        }
    }
    return static_cast<uint32_t>(distinctQueues.size());
}

std::vector<QueueMetadata> QueuesMetadata::GetAllQueues() const
{
    std::vector<QueueMetadata> result;
    for (const auto& group : queueGroups) {
        result.insert(result.end(), group.second.cbegin(), group.second.cend());
    }
    return result;
}

bool QueuesMetadata::HasAny(const QueueType queueType) const
{
    return queueGroups.find(queueType) != queueGroups.cend();
}

// Device
Device::Device(const PhysicalDevice& gpu, const QueuesMetadata& queuesMetadata)
    : m_gpu{ gpu }
{
    LOGI("Logical Device based on GPU: %s", m_gpu.GetProperties().deviceName);
    m_gpu.GetExtensions().Print();

    const auto queueFamilyIndices{ queuesMetadata.GetQueueFamilies() };

    const float DefaultQueuePriority{ 1.0f };
    std::vector<std::vector<float>> allQueuesPriorities(queueFamilyIndices.size());
    for (size_t i = 0; i < queueFamilyIndices.size(); ++i) {
        const auto queueFamilyIndex{ queueFamilyIndices[i] };
        const auto queueCount{ queuesMetadata.GetQueueFamilyCount(queueFamilyIndex) };
        allQueuesPriorities[i] = std::vector<float>(queueCount, DefaultQueuePriority);
    }

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfoList;
    for (size_t i = 0; i < queueFamilyIndices.size(); ++i) {
        const auto queueFamilyIndex{ queueFamilyIndices[i] };
        const auto queueCount{ queuesMetadata.GetQueueFamilyCount(queueFamilyIndex) };
        const auto& priorities{ allQueuesPriorities[i] };

        VkDeviceQueueCreateInfo info = { VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };
        info.queueFamilyIndex = queueFamilyIndex;
        info.queueCount = queueCount;
        info.pQueuePriorities = priorities.data();
        queueCreateInfoList.push_back(info);
    }

    const auto& extensions{ m_gpu.GetExtensions() };
    const auto& features{ m_gpu.GetEnabledFeatures2() };

    VkDeviceCreateInfo deviceCreateInfo = { VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
    deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfoList.size());
    deviceCreateInfo.pQueueCreateInfos = queueCreateInfoList.data();
    deviceCreateInfo.enabledExtensionCount = extensions.GetPickCount();
    deviceCreateInfo.ppEnabledExtensionNames = extensions.GetPickListRaw();
    deviceCreateInfo.pEnabledFeatures = nullptr;
    deviceCreateInfo.pNext = &features;
    VKERRCHECK(vkCreateDevice(m_gpu, &deviceCreateInfo, nullptr, &m_handle)); // create device

#ifdef ENABLE_VK_LOADER
    volkLoadDevice(m_handle);
#endif

    for (const auto& [queueGroupKey, queueGroupList] : queuesMetadata.queueGroups) {
        for (const auto& groupItem : queueGroupList) {
            VkQueue qHandle{};
            vkGetDeviceQueue(m_handle, groupItem.family, groupItem.index, &qHandle);
            m_queues[queueGroupKey].push_back(Queue{ qHandle, groupItem.family, groupItem.index, groupItem.flags, groupItem.surface });
        }
    }
}

Device::~Device()
{
    vkDeviceWaitIdle(m_handle);
    vkDestroyDevice(m_handle, nullptr);

    LOGI("Logical device destroyed");
}

void Device::WaitIdle() const
{
    vkDeviceWaitIdle(m_handle);
}

bool Device::HasQueue(const QueueType queueType, const uint32_t index) const
{
    const auto queuesIter{ m_queues.find(queueType) };
    if (queuesIter == m_queues.cend()) {
        LOGW("Trying to retrieve non existing QueueType.");
        return false;
    }

    const auto& queuesGroup{ queuesIter->second };
    if (index >= queuesGroup.size()) {
        LOGW("Trying access queue at invalid index %ud.", index);
        return false;
    }
    return true;
}

const Queue& Device::GetQueue(const QueueType queueType, const uint32_t index) const
{
    const auto queuesIter{ m_queues.find(queueType) };
    if (queuesIter == m_queues.cend()) {
        throw std::runtime_error("Trying to retrieve non existing QueueType.");
    }

    const auto& queuesGroup{ queuesIter->second };
    if (index >= queuesGroup.size()) {
        throw std::runtime_error("Trying access queue at invalid index " + std::to_string(index) + ".");
    }
    return queuesGroup[index];
}

const std::map<QueueType, std::vector<Queue>>& Device::GetAllQueues() const
{
    return m_queues;
}

std::vector<QueueType> Device::GetAllQueueTypes() const
{
    std::vector<QueueType> result;
    std::transform(m_queues.begin(), m_queues.end(), std::back_inserter(result), [](const auto& pair) { return pair.first; });
    return result;
}

const PhysicalDevice& Device::GetGPU() const
{
    return m_gpu;
}

void Device::Print() const
{
    auto queueTypeToString = [](const QueueType type) {
        static const std::map<QueueType, std::string> queueNames = {
            { QueueType::PRESENT, "PRESENT" },
            { QueueType::GRAPHICS, "GRAPHICS" },
            { QueueType::COMPUTE, "COMPUTE" },
            { QueueType::TRANSFER, "TRANSFER" },
            { QueueType::SPARSE, "SPARSE" },
            { QueueType::PROTECTED, "PROTECTED" }
        };
        return queueNames.at(type);
    };

    auto canPresentToString = [](const VkSurfaceKHR surface) -> std::string {
        return surface ? "(can present)" : "";
    };

    LOGI("Logical Device used queues:");
    for (const auto& [qGroupKey, gQroupList] : m_queues) {
        for (const auto& qGroupItem : gQroupList) {
            LOGI("Queue purpose: %s family: %d index: %d flags: [ %s] %s", queueTypeToString(qGroupKey).c_str(), qGroupItem.family, qGroupItem.index, prev::util::vk::QueueFlagsToString(qGroupItem.flags).c_str(), canPresentToString(qGroupItem.surface).c_str());
        }
    }
}

Device::operator VkDevice() const
{
    return m_handle;
}
} // namespace prev::core::device
